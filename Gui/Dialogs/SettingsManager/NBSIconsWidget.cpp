/*
	*
	* NBSGeneralWidget.cpp - General Settings Widget for NewBreeze
	*
*/

#include <NBSIconsWidget.hpp>

NBIconThemeWidget::NBIconThemeWidget( QWidget *parent ) :QWidget( parent ) {

	createGUI();
};

void NBIconThemeWidget::createGUI() {

	iconThemesWidget = new NBIconThemeChooserWidget( this );
	folderViewWidget = new NBIconThemeViewerWidget( this );

	connect( iconThemesWidget, SIGNAL( reloadIcons() ), folderViewWidget, SLOT( loadIcons() ) );

	QVBoxLayout *wLyt = new QVBoxLayout();
	wLyt->addWidget( iconThemesWidget );
	wLyt->addWidget( folderViewWidget );

	setLayout( wLyt );
};

void NBIconThemeWidget::setIconTheme() {

};

NBIconThemeChooserWidget::NBIconThemeChooserWidget( QWidget *parent ) : QWidget( parent ) {

	QHBoxLayout *wLyt = new QHBoxLayout();

	prevBtn = new NBButton( QIcon(":/icons/arrow-left.png" ), this );
	nextBtn = new NBButton( QIcon( ":/icons/arrow-right.png" ), this );
	themeCB = new QComboBox();

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

NBIconThemeViewerWidget::NBIconThemeViewerWidget( QWidget *parent ) : QListWidget( parent ) {

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

	loadIcons();
};

void NBIconThemeViewerWidget::loadIcons() {
	/*
		*
		* Icons to be loaded
		*

		*
		* inode/directory, text/plain, text/html, text/x-c++src, text/x-c++hdr,
		* text/x-csrc, text/x-chdr, text/x-python
		* application/x-gzip, application/zip, application/x-bzip2, application/x-7z-compressed
		* application/msword, application/vnd.ms-powerpoint, application/vnd.ms-excel
		* application/pdf, application/x-executable, application/x-font-ttf
		* audio/mp3, audio/ogg, audio/flac
		* video/mp4, video/avi, video/
		* image/png, image/jpeg, image/gif
		*
	*/

	QList<QMimeType> fileTypeList;
	fileTypeList << mimeDb.mimeTypeForFile( "/" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.txt" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.html" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.cpp" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.hpp" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.c" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.h" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.py" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.gz" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.zip" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.bz2" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.7z" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.doc" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.ppt" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.xls" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.pdf" );
	fileTypeList << mimeDb.mimeTypeForFile( "/bin/ls" );					// An executable which can surely be found in all linux based systems
	fileTypeList << mimeDb.mimeTypeForFile( "filename.ttf" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.mp3" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.mp4" );
	fileTypeList << mimeDb.mimeTypeForFile( "filename.png" );

	QStringList names;
	names << "Folder" << "Text File" << "Html File" << "C++ Src" << "C++ Hdr" << "C Src" << "C Hdr" << "Python File" << "GZip Archive";
	names << "Zip Archive" << "BZip2 Archive" << "7z Archive" << "MSWord" << "MS Powerpoint" << "MS Excel" << "PDF File";
	names << "Executable" << "Font File" << "Audio File" << "Video File" << "Image File" << "JPG File" << "GIF File";

	clear();

	for( int i = 0; i < fileTypeList.count(); i++ ) {
		QString icoStr = NBIconProvider::icon( QString( "filename" ), fileTypeList.at( i ) );
		QIcon icon = QIcon::fromTheme( icoStr, QIcon( icoStr ) );
		QListWidgetItem *item = new QListWidgetItem( icon, names.at( i ) );

		addItem( item );
	}
};
