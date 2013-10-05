/*
	*
	* NBBreadCrumbsBar.cpp - BreadCrumbs Navigation bar for NewBreeze
	*
*/

#include <NBBreadCrumbsBar.hpp>
#include <NBTools.hpp>

NBBreadCrumbMenu::NBBreadCrumbMenu( QWidget *parent, QString path ) : QLabel( parent ) {

	// Set the current working directory
	cwd = QDir( path );
	cwd.setFilter( QDir::NoDotAndDotDot | QDir::Dirs );

	// Create the GUI, i.e., an arrow
	setPixmap( QIcon::fromTheme( "arrow-right" ).pixmap( QSize( 16, 16 ) ) );
	menu = new QMenu();
	// menu->setStyleSheet( getStyleSheet( "NBMenu", Settings.General.Style ) );
	connect( menu, SIGNAL( aboutToHide() ), this, SLOT( onMenuHidden() ) );
};

void NBBreadCrumbMenu::createMenu() {

	menu->clear();
	if ( cwd.entryList().count()  ) {
		foreach( QString dir, cwd.entryList() ) {
			QAction *action = menu->addAction( QIcon::fromTheme( "folder" ), dir, this, SLOT( onMenuItemClicked() ) );
			Q_UNUSED( action );
		}
	}

	else {
		QAction *action = menu->addAction( "No folders" );
		action->setDisabled( true );
	}
};

void NBBreadCrumbMenu::mousePressEvent( QMouseEvent *mEvent ) {

	if ( menu->isVisible() ) {
		setPixmap( QIcon::fromTheme( "arrow-right" ).pixmap( QSize( 16, 16 ) ) );
		menu->hide();
	}

	else {
		createMenu();
		setPixmap( QIcon::fromTheme( "arrow-down" ).pixmap( QSize( 16, 16 ) ) );
		menu->popup( mapToGlobal( frameRect().bottomLeft() ) );
	}

	mEvent->accept();
};

void NBBreadCrumbMenu::onMenuHidden() {

	setPixmap( QIcon::fromTheme( "arrow-right" ).pixmap( QSize( 16, 16 ) ) );
};

void NBBreadCrumbMenu::onMenuItemClicked() {

	emit openThisLocation( cwd.filePath( qobject_cast<QAction *>( sender() )->text() ) );
};

NBBreadCrumb::NBBreadCrumb( QWidget *parent, QString path, bool current ) : QLabel( parent ) {

	cwd = QDir( path );
	cwd.setFilter( QDir::NoDotAndDotDot | QDir::Dirs );

	if ( cwd.isRoot() )
		setPixmap( QIcon::fromTheme( "drive-harddisk" ).pixmap( QSize( 16, 16 ) ) );

	else if ( cwd == QDir::home() )
		setPixmap( QIcon::fromTheme( "go-home" ).pixmap( QSize( 16, 16 ) ) );

	else
		setText( cwd.dirName() );

	// If we have a crumb containing current dir
	if ( current ) {
		if ( cwd.isRoot() or ( cwd == QDir::home() ) )
			setStyleSheet( "QLabel { border: 1px solid gray; border-radius: 5px; background: gray; }" );

		else {
			if ( ( Settings.General.Style == "LightGray" ) or ( Settings.General.Style == "TransLight" ) )
				setStyleSheet( "QLabel { font-weight: bold; color: black; }" );

			else
				setStyleSheet( "QLabel { font-weight: bold; color: white; }" );
		}
	}

	// The crumb is not the current dir
	else {
		if ( ( Settings.General.Style == "LightGray" ) or ( Settings.General.Style == "TransLight" ) )
			setStyleSheet( "QLabel { font-weight: none; color: black; } QLabel:hover{ color: darkblue; }" );

		else
			setStyleSheet( "QLabel { font-weight: none; color: white; } QLabel:hover{ color: darkblue; }" );
	}
};

void NBBreadCrumb::mousePressEvent( QMouseEvent *mEvent ) {

	emit openThisLocation( cwd.absolutePath() );
	mEvent->accept();
};

NBBreadCrumbsBar::NBBreadCrumbsBar( QWidget *parent, QString address ) : QWidget( parent ) {

	cwd = QDir( address );
	cwd.setFilter( QDir::NoDotAndDotDot | QDir::Dirs );

	autoLoad = true;
	hasTrailing = false;

	// We want this bar to be as small as possible. Hence we remove all the margins.
	setContentsMargins( QMargins() );

	// Load the address as a series of crumbs
	loadPath( cwd.absolutePath() );
};

void NBBreadCrumbsBar::loadPath( QString path ) {

	cwd = QDir( path );

	// If a layout exists, delete it
	if ( layout() != 0 ) {
		// Delete all the existing widgets
		QLayoutItem *wItem;
		while( ( wItem = layout()->takeAt( 0 ) ) != 0 ) {
			delete wItem->widget();
			delete wItem;
		}
		delete layout();

		// Resize
		adjustSize();
		updateGeometry();
	}

	QHBoxLayout *crumbLyt = new QHBoxLayout();
	crumbLyt->setContentsMargins( QMargins() );

	// Create a QFileInfo object representing the current working directory.
	QFileInfo info( cwd.absolutePath() );

	while ( !info.isRoot() ) {
		// Create a crumb menu representing the path of the current directory
		NBBreadCrumbMenu *menu = new NBBreadCrumbMenu( this, info.absolutePath() );
		connect( menu, SIGNAL( openThisLocation( QString ) ), this, SLOT( handleCrumbAndMenuSignal( QString ) ) );

		// Create a crumb representing the path of the current directory
		NBBreadCrumb *crumb = new NBBreadCrumb( this, info.absolutePath() );
		connect( crumb, SIGNAL( openThisLocation( QString ) ), this, SLOT( handleCrumbAndMenuSignal( QString ) ) );

		/*
			*
			* Insert them at position 0. Crumb Menu first, and then the Crumb
			* This way, we have the Menu to be always on the right of the Crumb
			* Also on the subsequent run, we will have a Crumb and menu to the
			* right of the current pair,which represent the path of the current
			* Crumb and Menu path.
			*

			*
			* Example:
			* CWD:
			*		/home/user/Documents/Office
			*
			* Current Crumb and CrumbMenu Path:
			*		/home/user/Documents
			*
			* Next Crumb and CrumbMenu Path:
			*		/home/user
			*
		*/

		crumbLyt->insertWidget( 0, menu );
		crumbLyt->insertWidget( 0, crumb );

		// Update the QFileInfo object to represent the
		info = QFileInfo( info.absolutePath() );
	}

	// In the above loop we never included the current woring directoy.
	// Create a crumb and a crumb menu representing the path of the current directory.
	NBBreadCrumb *crumb = new NBBreadCrumb( this, cwd.absolutePath(), true );
	connect( crumb, SIGNAL( openThisLocation( QString ) ), this, SLOT( handleCrumbAndMenuSignal( QString ) ) );

	NBBreadCrumbMenu *menu = new NBBreadCrumbMenu( this, cwd.absolutePath() );
	connect( menu, SIGNAL( openThisLocation( QString ) ), this, SLOT( handleCrumbAndMenuSignal( QString ) ) );

	// Add the crumb first and then the menu
	crumbLyt->addWidget( crumb );
	crumbLyt->addWidget( menu );

	crumbLyt->addStretch( 0 );

	setLayout( crumbLyt );
	/*
		*
		* Final result:
		*	Now say we want a CrumbBar for the path: /home/user/Documents/Office/Writer/
		*   We will have:
		*   +------------------------------------------------+
		*   | # > home > @ > Documents > Office > *Writer* > |
		*   +------------------------------------------------+
		*
		*	Above, # is the root icon, @ is the home icon, and the text between * is in bold
		*
	*/
};

bool NBBreadCrumbsBar::autoLoadNewPath() {

	return autoLoad;
};

void NBBreadCrumbsBar::setAutoLoadNewPath( bool truth ) {

	autoLoad = truth;
};

void NBBreadCrumbsBar::setCurrentDirectory( QString path ) {

	// This is a very ugly hack :(

	QDir d( path );

	if ( cwd.absolutePath() == path )
		hasTrailing = false;

	else if ( cwd.absolutePath().contains( path ) )
		hasTrailing = true;

	else
		hasTrailing = false;

	if ( cwd.absolutePath().contains( path ) ) {
		foreach( NBBreadCrumb *crumb, findChildren<NBBreadCrumb *>() ) {
			if ( crumb->cwd.absolutePath() == d.absolutePath() )
				if ( d.isRoot() ) {
					crumb->setStyleSheet( "QLabel { border: 1px solid gray; border-radius: 5px; background: gray; }" );
				}

				else {
					if ( ( Settings.General.Style == "LightGray" ) or ( Settings.General.Style == "TransLight" ) )
						crumb->setStyleSheet( "QLabel { font-weight: bold; color: black; }" );

					else
						crumb->setStyleSheet( "QLabel { font-weight: bold; color: white; }" );
				}

			else {
				if ( ( Settings.General.Style == "LightGray" ) or ( Settings.General.Style == "TransLight" ) )
					crumb->setStyleSheet( "QLabel { font-weight: none; color: black; } QLabel:hover{ color: darkblue; }" );

				else
					crumb->setStyleSheet( "QLabel { font-weight: none; color: white; } QLabel:hover{ color: darkblue; }" );
			}
		}
	}

	else {
		loadPath( path );
	}
};

void NBBreadCrumbsBar::handleCrumbAndMenuSignal( QString string ) {

	if ( autoLoad ) {
		loadPath( string );
		emit openLocation( string );
	}

	else {
		emit openLocation( string );
	}
};
