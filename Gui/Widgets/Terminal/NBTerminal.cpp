/*
    *
    * * NBTerminalWidget.cpp - QNBTerminalWidget reimplementation for NBTerminalWidget
    *
*/

#include <NBTerminal.hpp>
#include <NBGuiWidgets.hpp>

NBTerminal::NBTerminal( QString wDir ) : QWidget() {

	if ( wDir.isEmpty() )
		currentPath = QDir::homePath();

	else
		currentPath = QString( wDir );

	QVBoxLayout *lyt = new QVBoxLayout();
	Terminal = new NBTerminalWidget( currentPath );

	lyt->addWidget( Separator::horizontal() );
	lyt->addWidget( Terminal );
	setLayout( lyt );

	setFocusPolicy( Qt::NoFocus );

	connect( Terminal, SIGNAL( finished() ), this, SLOT( openNewTerminal() ) );
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

	Terminal = new NBTerminalWidget( currentPath );
	connect( Terminal, SIGNAL( finished() ), this, SLOT( openNewTerminal() ) );

	qobject_cast<QVBoxLayout *>( layout() )->insertWidget( 1, Terminal );

	hide();
};

NBTerminalWidget::NBTerminalWidget( QString wDir ) : QTermWidget( 0 ) {

	if ( Settings->General.Style == QString( "TransDark" ) ) {
		setTerminalOpacity( 0.81 );
		setColorScheme( "WhiteOnBlack" );
	}

	else if ( Settings->General.Style == QString( "TransLight" ) ) {
		setTerminalOpacity( 0.81 );
		setColorScheme( "BlackOnWhite" );
	}

	else if ( Settings->General.Style == QString( "DullBlack" ) ) {
		setTerminalOpacity( 1 );
		setColorScheme( "DarkPastels" );
	}

	else {
		setTerminalOpacity( 1 );
		setColorScheme( "BlackOnLightGray" );
	}

	QAction *clearAct = new QAction( "Clear Terminal", this );
	clearAct->setShortcut( tr( "Ctrl+Shift+X" ) );
	connect( clearAct, SIGNAL( triggered() ), this, SLOT( clear() ) );

	setEnvironment( QProcess::systemEnvironment() << "HISTFILE=/dev/null" );
	setWorkingDirectory( wDir );
	setShellProgram( "/bin/bash" );
	setArgs( QStringList() << "-il" );

	setMotionAfterPasting( 2 );
	setFlowControlEnabled( true );
	setFlowControlWarningEnabled( true );

	startShellProgram();

	setFocus();
};

QString NBTerminalWidget::currentWorkingDirectory() {

	QString cwd = QString( "/proc/%1/cwd" ).arg( getShellPID() );
	return QFileInfo( cwd ).symLinkTarget();
};
