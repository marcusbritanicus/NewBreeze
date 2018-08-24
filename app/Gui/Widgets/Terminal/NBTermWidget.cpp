/*
    *
    * * NBTermWidget.cpp - QTermWidget reimplementation for NBTerminal
    *
*/

#include "NBTermWidget.hpp"

NBTermWidget::NBTermWidget( QWidget *parent ) : QTermWidget( 0, parent ) {

	init();

	startShellProgram();
	oldCWD = QFileInfo( QString( "/proc/%1/cwd" ).arg( getShellPID() ) ).symLinkTarget() + "/";

	setFocus();
};

NBTermWidget::NBTermWidget( QString wDir, QWidget *parent ) : QTermWidget( 0, parent ) {

	init();

	setWorkingDirectory( wDir );

	startShellProgram();
	oldCWD = QFileInfo( QString( "/proc/%1/cwd" ).arg( getShellPID() ) ).symLinkTarget() + "/";

	setFocus();
};

NBTermWidget::NBTermWidget( QString wDir, QString cmd, QWidget *parent ) : QTermWidget( 0, parent ) {

	init();

	setWorkingDirectory( wDir );

	/* Do not use interactive mode */
	QStringList shArgs = QStringList() << "-l" << "-c" << cmd;
	if ( not cmd.isEmpty() )
		setArgs( shArgs );

	startShellProgram();
	oldCWD = QFileInfo( QString( "/proc/%1/cwd" ).arg( getShellPID() ) ).symLinkTarget() + "/";

	setFocus();
};

void NBTermWidget::init() {

	QSettings settings( "NewBreeze", "NBTerminal" );
	if ( not exists( settings.fileName() ) ) {
		settings.setValue( "Shell", "/bin/bash" );
		settings.setValue( "ColorScheme", "WhiteOnBlack" );
		settings.setValue( "Font", QFont( "monospace", 10 ) );
		settings.setValue( "Session/ShowMaximized", true );

		settings.sync();
	}

	connect( this, SIGNAL( tryChdir() ), this, SLOT( tryChdir() ) );

	/* Set the enivronment variable TERM as xterm */
	QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
	procEnv.insert( "TERM", "xterm-256color" );
	setEnvironment( procEnv.toStringList() );

	QFileSystemWatcher watcher;
	if ( not watcher.files().contains( settings.fileName() ) )
		watcher.addPath( settings.fileName() );

	connect( &watcher, SIGNAL( fileChanged( QString ) ), this, SLOT( reloadSettings() ) );

	setColorScheme( settings.value( "ColorScheme" ).toString() );
	setScrollBarPosition( QTermWidget::ScrollBarRight );

	QFont termFont = settings.value( "Font" ).value<QFont>();
	/* Minimum font size: 8 */
	if ( termFont.pointSize() < 8 )
		termFont.setPointSize( 8 );

	setTerminalFont( termFont );

	/* Unlimited scrollback */
	setHistorySize( -1 );

	setMotionAfterPasting( 2 );
	setFlowControlEnabled( true );
	setFlowControlWarningEnabled( true );

	setShellProgram( settings.value( "Shell" ).toString() );
};

QString NBTermWidget::currentWorkingDirectory() {

	QString cwd = QString( "/proc/%1/cwd" ).arg( getShellPID() );
	return QFileInfo( cwd ).symLinkTarget();
};

void NBTermWidget::changeDir( const QString &dir ) {

	oldCWD = dir;

	QString cmd = "cd " + dir + "\n";
	sendText( cmd );
};

void NBTermWidget::reloadSettings() {

	QSettings settings( "NewBreeze", "NBTerminal" );

	setColorScheme( settings.value( "ColorScheme" ).toString() );
	setTerminalFont( settings.value( "Font" ).value<QFont>() );
};

void NBTermWidget::tryChdir() {

	if ( not chTimer.isActive() )
		chTimer.start( 100, Qt::PreciseTimer, this );
};

void NBTermWidget::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == chTimer.timerId() ) {

		chTimer.stop();

		QString newCWD = QFileInfo( QString( "/proc/%1/cwd" ).arg( getShellPID() ) ).symLinkTarget() + "/";
		if ( newCWD == oldCWD )
			return;

		oldCWD = newCWD;
		emit chdir( oldCWD );

		return;
	}

	QTermWidget::timerEvent( tEvent );
};
