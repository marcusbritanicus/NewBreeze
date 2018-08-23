/*
    *
    * * NBTermWidget.cpp - QTermWidget reimplementation for NBTerminal
    *
*/

#include "NBTermWidget.hpp"

NBTermWidget::NBTermWidget( QWidget *parent ) : QTermWidget( 0, parent ) {

	QSettings settings( "NewBreeze", "NBTerminal" );
	if ( not exists( settings.fileName() ) ) {
		settings.setValue( "Shell", "/bin/bash" );
		settings.setValue( "ColorScheme", "WhiteOnBlack" );
		settings.setValue( "Font", QFont( "monospace", 10 ) );
		settings.setValue( "Session/ShowMaximized", true );

		settings.sync();
	}

	/* Set the enivronment variable TERM as xterm */
	QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
	procEnv.insert( "TERM", "xterm-256color" );
	setEnvironment( procEnv.toStringList() );

	if ( not watcher->files().contains( settings.fileName() ) )
		watcher->addPath( settings.fileName() );

	connect( watcher, SIGNAL( fileChanged( QString ) ), this, SLOT( reloadSettings() ) );

	setColorScheme( settings.value( "ColorScheme" ).toString() );
	setScrollBarPosition( QTermWidget::ScrollBarRight );

	setTerminalFont( settings.value( "Font" ).value<QFont>() );
	setHistorySize( 10000 );										// Infinte scroll back

	setShellProgram( settings.value( "Shell" ).toString() );

	setMotionAfterPasting( 2 );
	setFlowControlEnabled( true );
	setFlowControlWarningEnabled( true );

	startShellProgram();
	watcher->addPath( QString( "/proc/%1/" ).arg( getShellPID() ) );
	oldCWD = QFileInfo( QString( "/proc/%1/cwd" ).arg( getShellPID() ) ).symLinkTarget();
	connect( watcher, SIGNAL( directoryChanged( const QString& ) ), this, SLOT( handleFSWSignals( QString ) ) );

	setFocus();
};

NBTermWidget::NBTermWidget( QString wDir, QWidget *parent ) : QTermWidget( 0, parent ) {

	QSettings settings( "NewBreeze", "NBTerminal" );
	if ( not exists( settings.fileName() ) ) {
		settings.setValue( "Shell", "/bin/bash" );
		settings.setValue( "ColorScheme", "WhiteOnBlack" );
		settings.setValue( "Font", QFont( "monospace", 10 ) );
		settings.setValue( "Session/ShowMaximized", true );

		settings.sync();
	}

	/* Set the enivronment variable TERM as xterm */
	QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
	procEnv.insert( "TERM", "xterm-256color" );
	setEnvironment( procEnv.toStringList() );

	if ( not watcher->files().contains( settings.fileName() ) )
		watcher->addPath( settings.fileName() );

	connect( watcher, SIGNAL( fileChanged( QString ) ), this, SLOT( reloadSettings() ) );

	setColorScheme( settings.value( "ColorScheme" ).toString() );
	setScrollBarPosition( QTermWidget::ScrollBarRight );

	setTerminalFont( settings.value( "Font" ).value<QFont>() );
	setHistorySize( 10000 );										// Infinte scroll back

	setWorkingDirectory( wDir );
	setShellProgram( settings.value( "Shell" ).toString() );

	setMotionAfterPasting( 2 );
	setFlowControlEnabled( true );
	setFlowControlWarningEnabled( true );

	startShellProgram();

	setFocus();
};

NBTermWidget::NBTermWidget( QString wDir, QString cmd, QWidget *parent ) : QTermWidget( 0, parent ) {

	QSettings settings( "NewBreeze", "NBTerminal" );
	if ( not exists( settings.fileName() ) ) {
		settings.setValue( "Shell", "/bin/bash" );
		settings.setValue( "ColorScheme", "WhiteOnBlack" );
		settings.setValue( "Font", QFont( "monospace", 10 ) );
		settings.setValue( "Session/ShowMaximized", true );

		settings.sync();
	}

	/* Set the enivronment variable TERM as xterm */
	QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
	procEnv.insert( "TERM", "xterm-256color" );
	setEnvironment( procEnv.toStringList() );

	if ( not watcher->files().contains( settings.fileName() ) )
		watcher->addPath( settings.fileName() );

	connect( watcher, SIGNAL( fileChanged( QString ) ), this, SLOT( reloadSettings() ) );

	setColorScheme( settings.value( "ColorScheme" ).toString() );
	setScrollBarPosition( QTermWidget::ScrollBarRight );

	setTerminalFont( settings.value( "Font" ).value<QFont>() );
	setHistorySize( 10000 );

	setWorkingDirectory( wDir );
	// Do not use interactive mode.
	QStringList shArgs = QStringList() << "-l" << "-c" << cmd;

	setShellProgram( settings.value( "Shell" ).toString() );
	if ( not cmd.isEmpty() )
		setArgs( shArgs );

	setMotionAfterPasting( 2 );
	setFlowControlEnabled( true );
	setFlowControlWarningEnabled( true );

	startShellProgram();

	setFocus();
};

QString NBTermWidget::currentWorkingDirectory() {

	QString cwd = QString( "/proc/%1/cwd" ).arg( getShellPID() );
	return QFileInfo( cwd ).symLinkTarget();
};

void NBTermWidget::reloadSettings() {

	QSettings settings( "NewBreeze", "NBTerminal" );

	setColorScheme( settings.value( "ColorScheme" ).toString() );
	setTerminalFont( settings.value( "Font" ).value<QFont>() );
};

void NBTermWidget::handleFSWSignals( QString ) {

	if ( QFileInfo( QString( "/proc/%1/cwd" ).arg( getShellPID() ) ).symLinkTarget() == oldCWD )
		return;

	oldCWD = QFileInfo( QString( "/proc/%1/cwd" ).arg( getShellPID() ) ).symLinkTarget();
	emit chDir( oldCWD );
};
