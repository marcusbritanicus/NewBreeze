/*
	*
	* NBSGeneralWidget.cpp - General Settings Widget for NewBreeze
	*
*/

#include "NBSIconsWidget.hpp"

NBIconThemeWidget::NBIconThemeWidget( QWidget *parent ) :QWidget( parent ) {

	createGUI();
};

void NBIconThemeWidget::createGUI() {

	iconThemesWidget = new NBIconThemeChooserWidget( this );
	folderViewWidget = new NBIconThemeViewerWidget( this );

	connect( iconThemesWidget, SIGNAL( reloadIcons() ), folderViewWidget, SIGNAL( setupModel() ) );

	QVBoxLayout *wLyt = new QVBoxLayout();
	wLyt->addWidget( iconThemesWidget );
	wLyt->addWidget( folderViewWidget );

	setLayout( wLyt );
};

NBIconThemeChooserWidget::NBIconThemeChooserWidget( QWidget *parent ) : QWidget( parent ) {

	QHBoxLayout *wLyt = new QHBoxLayout();

	prevBtn = new NBButton( QIcon::fromTheme( "arrow-left", QIcon( ":/icons/arrow-left.png" ) ), this );
	prevBtn->setFocusPolicy( Qt::NoFocus );
	prevBtn->setShortcut( tr( "Ctrl+P" ) );

	nextBtn = new NBButton( QIcon::fromTheme( "arrow-right", QIcon( ":/icons/arrow-right.png" ) ), this );
	nextBtn->setFocusPolicy( Qt::NoFocus );
	nextBtn->setShortcut( tr( "Ctrl+N" ) );

	themeCB = new QComboBox();
	themeCB->setFocusPolicy( Qt::NoFocus );

	loadThemes();

	connect( prevBtn, SIGNAL( clicked() ), this, SLOT( previousTheme() ) );
	connect( nextBtn, SIGNAL( clicked() ), this, SLOT( nextTheme() ) );
	connect( themeCB, SIGNAL( currentIndexChanged( int ) ), this, SLOT( switchTheme( int ) ) );

	wLyt->addStretch( 0 );
	wLyt->addWidget( prevBtn );
	wLyt->addWidget( themeCB );
	wLyt->addWidget( nextBtn );
	wLyt->addStretch( 0 );

	setLayout( wLyt );
};

void NBIconThemeChooserWidget::loadThemes() {

	themesList.clear();

	QDir iconDir( "/usr/share/icons" );
	iconDir.setFilter( QDir::Dirs );

	foreach ( QString theme, iconDir.entryList() ) {
		if ( QFileInfo( iconDir.filePath( theme ) ).exists() ) {
			if ( QSettings( iconDir.filePath( theme + "/index.theme" ), QSettings::IniFormat ).value( "Icon Theme/Directories" ).toStringList().count() )
				themesList << theme;
		}
	}

	themeCB->addItems( themesList );
	current = themesList.indexOf( Settings->General.IconTheme );
	themeCB->setCurrentIndex( current );
};

void NBIconThemeChooserWidget::switchTheme( int idx ) {

	current = idx;

	QIcon::setThemeName( themesList[ current ] );
	Settings->setValue( "IconTheme", themesList[ current ] );

	emit reloadIcons();
};

void NBIconThemeChooserWidget::nextTheme() {

	if ( current + 1 == themesList.count() ) {
		current = 0;
		themeCB->setCurrentIndex( current );
	}
	else {
		current++;
		themeCB->setCurrentIndex( current );
	}

	QIcon::setThemeName( themesList[ current ] );
	Settings->setValue( "IconTheme", themesList[ current ] );
	emit reloadIcons();
};

void NBIconThemeChooserWidget::previousTheme() {

	if ( current == 0 ) {
		current = themesList.count() - 1;
		themeCB->setCurrentIndex( current );
	}
	else {
		current--;
		themeCB->setCurrentIndex( current );
	}

	QIcon::setThemeName( themesList[ current ] );
	Settings->setValue( "IconTheme", themesList[ current ] );
	emit reloadIcons();
};

NBIconThemeModel::NBIconThemeModel( QObject *parent ) : QAbstractListModel( parent ) {

	QTimer::singleShot( 10, this, SLOT( setupModel() ) );
};

int NBIconThemeModel::rowCount( const QModelIndex &parent ) const {

	if ( parent == QModelIndex() )
		return mimeTypeList.count();

	else
		return 0;
};

QVariant NBIconThemeModel::data( const QModelIndex &index, int role ) const {

	int mimeIndex = index.row();

	switch( role ) {
		case Qt::DisplayRole:
			return mimeNameList.at( mimeIndex );

		case Qt::DecorationRole:
			return mimeIconList.at( mimeIndex );

		case Qt::ToolTipRole :
			return mimeTypeList.at( mimeIndex );

		default:
			return QVariant();
	}
};

void NBIconThemeModel::setupModel() {

	mimeNameList.clear();
	mimeIconList.clear();
	mimeTypeList.clear();

	beginResetModel();
	Q_FOREACH( QMimeType mime, mimeDb.allMimeTypes() ) {
		QIcon ico1 = QIcon::fromTheme( mime.iconName() );
		QIcon ico2 = QIcon::fromTheme( mime.genericIconName() );

		/* We will now show mimes with out icons */
		if ( ico1.isNull() and ico2.isNull() ) {

			continue;
		}

		/* These have mimes */
		else {
			if ( mime.preferredSuffix().count() )
				mimeNameList << mime.preferredSuffix().toUpper();

			else
				mimeNameList << mime.comment();

			if ( not ico1.isNull() )
				mimeIconList << ico1;

			else
				mimeIconList << ico2;

			mimeTypeList << mime.name();
		}
		qApp->processEvents();
	}
	endResetModel();

	emit layoutChanged();
};

NBIconThemeViewerWidget::NBIconThemeViewerWidget( QWidget *parent ) : QListView( parent ) {

	// View, Sizes and Resize Modes
	setViewMode( QListView::IconMode );
	setGridSize( QSize( 120, 96 ) );
	setIconSize( QSize( 48, 48 ) );
	setFlow( QListView::LeftToRight );

	setSpacing( 9 );
	setResizeMode( QListView::Adjust );
	setMovement( QListView::Static );

	// DragAndDrop
	setDragDropMode( QAbstractItemView::NoDragDrop );

	// Selection
	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setSelectionRectVisible( true );

	// Wrapping
	setWordWrap( true );
	setWrapping( true );

	// Uniform Item Sizes
	setUniformItemSizes( true );

	NBIconThemeModel *model = new NBIconThemeModel( this );
	connect( this, SIGNAL( setupModel() ), model, SLOT( setupModel() ) );

	setModel( model );
};
