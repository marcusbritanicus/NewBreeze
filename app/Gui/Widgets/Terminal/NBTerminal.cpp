/*
    *
    * * NBTerminalWidget.cpp - QNBTerminalWidget reimplementation for NBTerminalWidget
    *
*/

#include "NBTerminal.hpp"
#include "NBGuiWidgets.hpp"

NBTerminal::NBTerminal( QString wDir, QWidget *parent ) : QWidget( parent ) {

	if ( wDir.isEmpty() )
		currentPath = QDir::homePath();

	else
		currentPath = QString( wDir );

	QVBoxLayout *lyt = new QVBoxLayout();
	lyt->setContentsMargins( QMargins( 2, 0, 2, 0 ) );
	lyt->setSpacing( 0 );
	Terminal = new NBTerminalWidget( currentPath, this );

	lyt->addWidget( Terminal );
	setLayout( lyt );

	setFocusPolicy( Qt::NoFocus );
	setAttribute( Qt::WA_TranslucentBackground );
	Terminal->setTerminalOpacity( 0.9 );

	connect( Terminal, SIGNAL( finished() ), this, SLOT( openNewTerminal() ) );
	connect( Terminal, SIGNAL( chdir( QString ) ), SIGNAL( chdir( QString ) ) );
};

void NBTerminal::changeDir( QString dir ) {

	currentPath = QString( dir );
	Terminal->changeDir( dir );
	Terminal->clear();
};

void NBTerminal::show() {

	QWidget::show();
	Terminal->setFocus();

	emit shown( true );
};

void NBTerminal::hide() {

	QWidget::hide();
	emit shown( false );
};

void NBTerminal::openNewTerminal() {

	layout()->removeWidget( Terminal );
	delete Terminal;

	Terminal = new NBTerminalWidget( currentPath, this );
	connect( Terminal, SIGNAL( finished() ), this, SLOT( openNewTerminal() ) );

	qobject_cast<QVBoxLayout *>( layout() )->insertWidget( 1, Terminal );

	hide();
};

NBTerminalWidget::NBTerminalWidget( QString wDir, QWidget *parent ) : QTermWidget( 0, parent ) {

	/* Black on white color scheme */
	setColorScheme( "WhiteOnBlack" );

	/* Shotcuts */
	QAction *clearAct = new QAction( "Clear Terminal", this );
	clearAct->setShortcut( tr( "Ctrl+Shift+X" ) );
	connect( clearAct, SIGNAL( triggered() ), this, SLOT( clear() ) );

	/* Setup 'cd' detection */
	setupCwdAutoDetect();

	/* Setup the shell */
	setEnvironment( QProcess::systemEnvironment() << "HISTFILE=/dev/null" );
	setWorkingDirectory( wDir );
	setShellProgram( "/bin/bash -i " );
	setArgs( QStringList() << "--rcfile" << nb3rc  );

	/* Terminal flow control */
	setMotionAfterPasting( 2 );
	setFlowControlEnabled( true );
	setFlowControlWarningEnabled( true );

	startShellProgram();

	setFocus();
};

NBTerminalWidget::~NBTerminalWidget() {

	QFile::remove( cwdfn );
};

QString NBTerminalWidget::currentWorkingDirectory() {

	QString cwd = QString( "/proc/%1/cwd" ).arg( getShellPID() );
	return QFileInfo( cwd ).symLinkTarget();
};

void NBTerminalWidget::setupCwdAutoDetect() {

	/* Create a unique cwd storage file */
	cwdfn = "/tmp/.cwd" + QCryptographicHash5::hash( QTime::currentTime().toString().toUtf8(), QCryptographicHash5::Md5 ).toHex().left( 6 );

	QFile f( cwdfn );
	f.open( QFile::WriteOnly );
	f.close();

	/* NB3 internal use bashrc filename */
	nb3rc = QString( "/tmp/.nb3bashrc-%1" ).arg( geteuid() );

	/* Create nb3 bashrc file */
	QFile::copy( QDir::home().filePath( ".bashrc" ), nb3rc );
	QFile nb3f( nb3rc );
	nb3f.open( QFile::Append );
	nb3f.write( "\nfunction cd() { builtin cd \"$@\" && pwd > " + cwdfn.toLocal8Bit() + "; }\n" );
	nb3f.close();

	/* Auto detection */
	QFileSystemWatcher *fsw = new QFileSystemWatcher();
	fsw->addPath( cwdfn );
	connect( fsw, SIGNAL( fileChanged( QString) ), this, SLOT( handleCwdChange( QString ) ) );
};

void NBTerminalWidget::handleCwdChange( QString fn ) {

	if ( not exists( fn ) )
		return;

	QFile f( fn );
	f.open( QFile::ReadOnly );
	QString cwd = QString::fromLocal8Bit( f.readAll() );
	f.close();

	// Remove the trailing "\n"
	cwd.chop( 1 );

	/* If @oldcwd and @cwd are different */
	if ( oldcwd.compare( cwd ) ) {
		oldcwd = cwd;
		emit chdir( cwd );
	}
};
