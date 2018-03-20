/*
	*
	* NBTerminal.cpp - NBTerminal Class
	*
*/

#include "NBTerminal.hpp"
#include "NBTermWidget.hpp"
#include "NBTSettingsDialog.hpp"

NBTerminal::NBTerminal( QString workDir, QWidget *parent ) : QMainWindow( parent ) {

	createGUI();
	setupActions();
	setWindowProperties();

	TabWidget->newTerminal( workDir, QString() );
	TabWidget->setFocusPolicy( Qt::NoFocus );
};

NBTerminal::NBTerminal( QString workDir, QString cmd, QWidget *parent ) : QMainWindow( parent ) {

	createGUI();
	setupActions();
	setWindowProperties();

	TabWidget->newTerminal( workDir, cmd );
	TabWidget->setFocusPolicy( Qt::NoFocus );
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
	QWidget *widget = new QWidget();
	widget->setObjectName( "base" );
	widget->setLayout( lyt );

	setCentralWidget( widget );
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

	// Paste clipboard
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

	// Quit NBTerminal
	// QAction *quitAct = new QAction( "&Quit", this );
	// quitAct->setShortcuts( QList<QKeySequence>() << tr( "Ctrl+Shift+Q" ) );

	// connect( quitAct, SIGNAL( triggered() ), this, SLOT( close() ) );
	// addAction( quitAct );
};

void NBTerminal::setWindowProperties() {

	setWindowTitle( "NewBreeze Terminal" );
	setWindowIcon( QIcon::fromTheme( "utilities-terminal" ) );

	setGeometry( settings.value( "Session/Geometry" ).toRect() );
	setMinimumSize( 800, 600 );

	setStyleSheet( getStyleSheet( "NBTerminal" ) );

	/* Transparency */
	if ( settings.value( "EnableTransparency" ).toBool() )
		setAttribute( Qt::WA_TranslucentBackground );
};

void NBTerminal::showHide() {

	if ( isVisible() )
		hide();

	else {
	   show();
	   activateWindow();
	}
};

void NBTerminal::showSettings() {

	NBTSettingsDialog *settingsDlg = new NBTSettingsDialog();
	settingsDlg->exec();

	show();
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

	settings.setValue( "Session/Geometry", geometry() );
	settings.setValue( "Session/ShowMaximized", isMaximized() );
	cEvent->accept();
};
