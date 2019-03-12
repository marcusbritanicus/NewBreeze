/*
	*
	* NBSGeneralWidget.cpp - General Settings Widget for NewBreeze
	*
*/

#include "NBSIconsWidget.hpp"
#include "NBMimeTypeModel.hpp"

NBIconThemeWidget::NBIconThemeWidget( QWidget *parent ) : QWidget( parent ) {

	createGUI();

	iconViewWidget->select( Settings->value( "View/IconTheme", NBSettings::GlobalScope ) );
	iconViewWidget->setFocus();
};

void NBIconThemeWidget::createGUI() {

	iconViewWidget = new NBIconThemeView( this );
	connect( iconViewWidget, SIGNAL( pressed( const QModelIndex& ) ), this, SLOT( switchTheme( const QModelIndex& ) ) );

	QVBoxLayout *wLyt = new QVBoxLayout();
	wLyt->addWidget( new QLabel( "Choose the icon theme below." ) );
	wLyt->addWidget( iconViewWidget );

	setLayout( wLyt );
};

void NBIconThemeWidget::switchTheme( const QModelIndex &idx ) {

	QString theme = idx.data().toString();

	QIcon::setThemeName( theme );
	Settings->setValue( "View/IconTheme", theme, NBSettings::GlobalScope );
};

NBIconThemeModel::NBIconThemeModel( QObject *parent ) : QAbstractListModel( parent ) {

	setupModel();
};

int NBIconThemeModel::rowCount( const QModelIndex &parent ) const {

	if ( parent == QModelIndex() )
		return themesList.count();

	else
		return 0;
};

QVariant NBIconThemeModel::data( const QModelIndex &index, int role ) const {

	int mimeIndex = index.row();

	switch( role ) {
		case Qt::DisplayRole:
			return themesList.at( mimeIndex );

		case Qt::DecorationRole:
			return iconsMap.value( themesList.at( mimeIndex ) );

		case Qt::ToolTipRole :
			return themesList.at( mimeIndex );

		default:
			return QVariant();
	}
};

void NBIconThemeModel::setupModel() {

	themesList.clear();

	beginResetModel();
	QSettings mdb1( NBXdg::userDir( NBXdg::XDG_CACHE_HOME ) + "NewBreeze/mimetypes.db", QSettings::NativeFormat );
	QDir iconDir( "/usr/share/icons" );
	iconDir.setFilter( QDir::Dirs );

	Q_FOREACH( QString theme, iconDir.entryList() ) {
		if ( QFileInfo( iconDir.filePath( theme ) ).exists() ) {
			if ( theme == "hicolor" or theme == "mono" )
				continue;

			if ( QSettings( iconDir.filePath( theme + "/index.theme" ), QSettings::IniFormat ).value( "Icon Theme/Directories" ).toStringList().count() ) {
				themesList << theme;

				QIcon::setThemeName( theme );
				QImage icon( QSize( 144, 96 ), QImage::Format_ARGB32 );
				icon.fill( Qt::transparent );

				QPainter painter( &icon );
				painter.drawPixmap( QRect( 6, 6, 36, 36 ), QIcon::fromTheme( "application-x-executable" ).pixmap( 36 ) );
				painter.drawPixmap( QRect( 54, 6, 36, 36 ), QIcon::fromTheme( "folder" ).pixmap( 36 ) );
				painter.drawPixmap( QRect( 102, 6, 36, 36 ), QIcon::fromTheme( "x-office-document" ).pixmap( 36 ) );
				painter.drawPixmap( QRect( 6, 48, 36, 36 ), QIcon::fromTheme( "folder-trash" ).pixmap( 36 ) );
				painter.drawPixmap( QRect( 54, 48, 36, 36 ), QIcon::fromTheme( "system-help" ).pixmap( 36 ) );
				painter.drawPixmap( QRect( 102, 48, 36, 36 ), QIcon::fromTheme( "preferences-system" ).pixmap( 36 ) );

				painter.setPen( Qt::lightGray );
				painter.drawRect( QRect( 0, 0, 143, 95 ) );

				painter.end();

				iconsMap[ theme ] = QIcon( QPixmap::fromImage( icon ) );

				qApp->processEvents();
			}
		}
	}

	endResetModel();

	/* Hack: To restore the Icon Theme which is changed above */
	QIcon::setThemeName( Settings->value( "View/IconTheme", NBSettings::GlobalScope ) );

	emit layoutChanged();
};

NBIconThemeView::NBIconThemeView( QWidget *parent ) : QListView( parent ) {

	// View, Sizes and Resize Modes
	setViewMode( QListView::IconMode );
	setGridSize( QSize( 160, 128 ) );
	setIconSize( QSize( 144, 96 ) );
	setFlow( QListView::LeftToRight );

	setSpacing( 9 );
	setResizeMode( QListView::Adjust );
	setMovement( QListView::Static );

	// DragAndDrop
	setDragDropMode( QAbstractItemView::NoDragDrop );

	// Selection
	setSelectionMode( QAbstractItemView::SingleSelection );
	setSelectionRectVisible( true );

	// Wrapping
	setWordWrap( true );
	setWrapping( true );

	// Uniform Item Sizes
	setUniformItemSizes( true );

	model = new NBIconThemeModel( this );
	connect( model, SIGNAL( layoutChanged() ), this, SLOT( update() ) );

	setModel( model );
};

void NBIconThemeView::select( QString name ) {

	for( int i = 0; i < model->rowCount(); i++ ) {
		if ( model->index( i, 0, QModelIndex() ).data().toString() == name ) {
			setCurrentIndex( model->index( i, 0, QModelIndex() ) );
			selectionModel()->select( model->index( i, 0, QModelIndex() ), QItemSelectionModel::ClearAndSelect );
			break;
		}
	}
};
