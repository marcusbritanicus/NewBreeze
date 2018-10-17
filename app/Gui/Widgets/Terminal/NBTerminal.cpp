/*
	*
	* NBTerminal.cpp - NBTerminal Class
	*
*/

#include <NBTerminal.hpp>
#include <NBTermWidget.hpp>
#include <NBTSettingsDialog.hpp>

NBTerminal::NBTerminal( QString wDir, QString cmd, QWidget *parent ) : QMainWindow( parent ) {

	QSettings settings( "NewBreeze", "NBTerminal" );
	if ( not exists( settings.fileName() ) ) {
		settings.setValue( "Shell", "/bin/bash" );
		settings.setValue( "ColorScheme", "WhiteOnBlack" );
		settings.setValue( "Font", QFont( "monospace", 10 ) );
		settings.setValue( "Session/ShowMaximized", true );

		settings.sync();
	}

	mWorkDir = QString( wDir );
	mCmd = QString( cmd );

	createGUI();
	setupActions();
	setWindowProperties();
};

NBTerminal::~NBTerminal() {

	for( int i = 0; i < TabWidget->count(); i++ ) {
		NBTermWidget *tw = qobject_cast<NBTermWidget*>( TabWidget->widget( i ) );
		tw->sendText( QString::fromLocal8Bit( "exit\n" ) );
	}
};

void NBTerminal::createGUI() {

	/* Out tab widget */
	TabWidget = new TtyTabWidget( this, false );

	/* Base Layout */
	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setSpacing( 0 );
	lyt->setContentsMargins( QMargins( 3, 5, 3, 3 ) );
	lyt->addWidget( TabWidget );

	/* Base Widget */
	QWidget *widget = new QWidget( this );
	widget->setObjectName( "base" );
	widget->setLayout( lyt );

	setCentralWidget( widget );

	/* Start a terminal session */
	startTerminal();

	/* Widget Properties */
	TabWidget->setFocusPolicy( Qt::NoFocus );
};

void NBTerminal::startTerminal() {

	TabWidget->newTerminal( mWorkDir, mCmd );
};

void NBTerminal::setupActions() {

	connect( TabWidget, SIGNAL( close() ), this, SLOT( close() ) );

	// New Terminal
	QAction *newTermAct = new QAction( "&New Terminal", this );
	newTermAct->setShortcuts( QList<QKeySequence>() << tr( "Ctrl+Shift+N" ) );

	connect( newTermAct, SIGNAL( triggered() ), TabWidget, SLOT( newTerminal() ) );
	addAction( newTermAct );

	// New Terminal in the current  directory
	QAction *newTermCwdAct = new QAction( "&New Terminal in CWD", this );
	newTermCwdAct->setShortcuts( QList<QKeySequence>() << tr( "Ctrl+Shift+T" ) );

	connect( newTermCwdAct, SIGNAL( triggered() ), TabWidget, SLOT( newTerminalCWD() ) );
	addAction( newTermCwdAct );

	// Clear Terminal
	QAction *clearTermAct = new QAction( "C&lear Terminal", this );
	clearTermAct->setShortcuts( QList<QKeySequence>() << tr( "Ctrl+Shift+X" ) );

	connect( clearTermAct, SIGNAL( triggered() ), TabWidget, SLOT( clearTerminal() ) );
	addAction( clearTermAct );

	// Copy Selection
	QAction *copyAct = new QAction( QIcon( ":/icons/edit-copy.png" ), "&Copy", this );
	copyAct->setShortcut( tr( "Ctrl+Shift+C" ) );

	connect( copyAct, SIGNAL( triggered() ), TabWidget, SLOT( copyToClipboard() ) );
	addAction( copyAct );

	// Paste Clipboard
	QAction *pasteAct = new QAction( QIcon( ":/icons/edit-paste.png" ), "&Paste", this );
	pasteAct->setShortcut( tr( "Ctrl+Shift+V" ) );

	connect( pasteAct, SIGNAL( triggered() ), TabWidget, SLOT( pasteClipboard() ) );
	addAction( pasteAct );

	// Previous Terminal
	QAction *prevTermAct = new QAction( "&prev Terminal", this );
	prevTermAct->setShortcuts( QList<QKeySequence>() << tr( "Ctrl+Shift+Tab" ) << tr( "Ctrl+PgUp" ) << tr( "Shift+Left" ) );

	connect( prevTermAct, SIGNAL( triggered() ), TabWidget, SLOT( prevTerminal() ) );
	addAction( prevTermAct );

	// Next Terminal
	QAction *nextTermAct = new QAction( "&Next Terminal", TabWidget );
	nextTermAct->setShortcuts( QList<QKeySequence>() << tr( "Ctrl+Tab" ) << tr( "Ctrl+PgDown" ) << tr( "Shift+Right" ) );

	connect( nextTermAct, SIGNAL( triggered() ), TabWidget, SLOT( nextTerminal() ) );
	addAction( nextTermAct );

	// Terminal Settings
	QAction *settingsAct = new QAction( "&Settings", TabWidget );
	settingsAct->setShortcuts( QList<QKeySequence>() << tr( "Ctrl+Shift+S" ) );

	connect( settingsAct, SIGNAL( triggered() ), this, SLOT( showSettings() ) );
	addAction( settingsAct );

	// Open FileManager here
	QAction *fmgrAct = new QAction( "Open &File Manager", this );
	fmgrAct->setShortcuts( QList<QKeySequence>() << tr( "Ctrl+Shift+O" ) );

	connect( fmgrAct, SIGNAL( triggered() ), this, SLOT( openFMgr() ) );
	addAction( fmgrAct );

	// closeTab NBTerminal
	QAction *closeTabAct = new QAction( "Close &Tab", this );
	closeTabAct->setShortcuts( QList<QKeySequence>() << tr( "Ctrl+Shift+W" ) );

	connect( closeTabAct, SIGNAL( triggered() ), TabWidget, SLOT( closeTab() ) );
	addAction( closeTabAct );
};

void NBTerminal::setWindowProperties() {

	setWindowTitle( "NBTerminal" );
	setWindowIcon( QIcon::fromTheme( "utilities-terminal" ) );

	QSettings settings( "NewBreeze", "NBTerminal" );

	setGeometry( settings.value( "Session/Geometry" ).toRect() );
	setMinimumSize( 800, 600 );
};

void NBTerminal::showSettings() {

	NBTSettingsDialog *settingsDlg = new NBTSettingsDialog();
	settingsDlg->exec();
};

void NBTerminal::openFMgr() {

	QString cwd = qobject_cast<NBTermWidget*>( TabWidget->currentWidget() )->currentWorkingDirectory();
	QProcess::startDetached( "xdg-open", QStringList() << cwd );
};

void NBTerminal::closeEvent( QCloseEvent *cEvent ) {

	for( int i = 0; i < TabWidget->count(); i++ ) {
		NBTermWidget *tw = qobject_cast<NBTermWidget*>( TabWidget->widget( i ) );
		tw->sendText( QString::fromLocal8Bit( "exit\n" ) );
	}

	QSettings settings( "NewBreeze", "NBTerminal" );

	settings.setValue( "Session/Geometry", geometry() );
	settings.setValue( "Session/ShowMaximized", isMaximized() );
	cEvent->accept();
};

NBTerminalWidget::NBTerminalWidget( QString wDir, QWidget *parent ) : QWidget( parent ) {

	if ( wDir.isEmpty() )
		currentPath = QDir::homePath();

	else
		currentPath = QString( wDir );

	QVBoxLayout *lyt = new QVBoxLayout();
	lyt->setContentsMargins( QMargins( 2, 0, 2, 0 ) );
	lyt->setSpacing( 0 );
	Terminal = new NBTermWidget( currentPath, this );

	lyt->addWidget( Terminal );
	setLayout( lyt );

	setFocusPolicy( Qt::NoFocus );
	setAttribute( Qt::WA_TranslucentBackground );
	Terminal->setTerminalOpacity( 0.9 );

	connect( Terminal, SIGNAL( finished() ), this, SLOT( openNewTerminal() ) );
	connect( Terminal, SIGNAL( chdir( QString ) ), SIGNAL( chdir( QString ) ) );
};

void NBTerminalWidget::changeDir( QString dir ) {

	currentPath = QString( dir );
	Terminal->changeDir( dir );
	Terminal->clear();
};

void NBTerminalWidget::show() {

	QWidget::show();
	Terminal->setFocus();

	emit shown( true );
};

void NBTerminalWidget::hide() {

	QWidget::hide();
	emit shown( false );
};

void NBTerminalWidget::openNewTerminal() {

	layout()->removeWidget( Terminal );
	delete Terminal;

	Terminal = new NBTermWidget( currentPath, this );
	connect( Terminal, SIGNAL( finished() ), this, SLOT( openNewTerminal() ) );

	qobject_cast<QVBoxLayout *>( layout() )->insertWidget( 1, Terminal );

	hide();
};
