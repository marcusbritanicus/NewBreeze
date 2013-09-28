/*
	*
	* NBGuiWidgets.cpp - The GUI classes for NewBreeze
	*
*/

#include <NBTools.hpp>
#include <NBSettingsWidgets.hpp>
#include <NBThumbsCache.hpp>

inline QStringList getShortcutList( QString actionName ) {

	if ( not shortcutSettings.value( actionName ).isNull() )
		return shortcutSettings.value( actionName ).toStringList();

	else
		return QStringList();
};

NBSettingChooserWidget::NBSettingChooserWidget() : QListWidget() {

	// View, Sizes and Resize Modes
	setViewMode( QListView::IconMode );
	setIconSize( QSize( 64, 64 ) );
	setFlow( QListView::TopToBottom );

	setFixedWidth( 126 );

	setResizeMode( QListView::Fixed );
	setMovement( QListView::Static );

	// DragAndDrop
	setDragDropMode( QAbstractItemView::NoDragDrop );

	// Selection
	setSelectionMode( QAbstractItemView::SingleSelection );
	setSelectionRectVisible( true );

	// Wrapping
	setWordWrap( true );
	setWrapping( true );

	// Tab Focus
	setFocusPolicy( Qt::TabFocus );
};

NBSCWidgetItem::NBSCWidgetItem( QIcon icon, QString text ) : QListWidgetItem() {

	setIcon( icon );
	setText( text );
	setSizeHint( QSize( 120, 96 ) );
};

NBIconThemeChooserWidget::NBIconThemeChooserWidget() : QWidget() {

	QHBoxLayout *wLyt = new QHBoxLayout();

	prevBtn = new NBToolButton( ":/icons/arrow-left.png" );
	nextBtn = new NBToolButton( ":/icons/arrow-right.png" );
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
	current = themesList.indexOf( Settings.General.IconTheme );
	themeCB->setCurrentIndex( current );
};

void NBIconThemeChooserWidget::switchTheme( int idx ) {

	current = idx;

	QIcon::setThemeName( themesList[ current ] );
	settings.setValue( "IconTheme", themesList[ current ] );
	Settings.General.IconTheme = themesList[ current ];

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
	settings.setValue( "IconTheme", themesList[ current ] );
	Settings.General.IconTheme = themesList[ current ];
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
	settings.setValue( "IconTheme", themesList[ current ] );
	Settings.General.IconTheme = themesList[ current ];
	emit reloadIcons();
};

NBIconThemeViewerWidget::NBIconThemeViewerWidget() : QListWidget() {

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

	clear();
	qApp->processEvents();

	// Update the icon cache only if this function was called for as a SIGNAL response
	// i.e. if it was invoked by a SIGNAL-SLOT connection.
	if ( qobject_cast<NBIconThemeChooserWidget*>( sender() ) ) {
		QLabel *lbl = new QLabel( "Reloading the icon cache. Please wait" );
		lbl->setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );
		lbl->setWindowModality( Qt::ApplicationModal );
		lbl->setAlignment( Qt::AlignCenter );
		lbl->setFont( QFont( "Envy Code R", 12 ) );
		lbl->setFixedSize( 450, 75 );

		QSize sSize = QDesktopWidget().size();
		lbl->move( ( sSize.width() - 450 ) / 2, ( sSize.height() - 100 ) / 2 );

		lbl->show();
		qApp->processEvents();

		NBIcon::loadCacheToMemory();

		lbl->close();
	}

	QStringList mimetypes;
	mimetypes << "inode/directory" << "text/plain" << "text/html" << "text/x-c++src" << "text/x-c++hdr";
	mimetypes << "text/x-csrc" << "text/x-chdr" << "text/x-python";
	mimetypes << "application/x-gzip" << "application/zip" << "application/x-bzip2" << "application/x-7z-compressed";
	mimetypes << "application/msword" << "application/vnd.ms-powerpoint" << "application/vnd.ms-excel";
	mimetypes << "application/pdf" << "application/x-executable" << "application/x-font-ttf";
	mimetypes << "audio/mp3" << "audio/ogg" << "audio/flac";
	mimetypes << "video/mp4" << "video/avi" << "video/x-flv";
	mimetypes << "image/png" << "image/jpeg" << "image/gif";

	QStringList suffixes;
	suffixes << "" << "txt" << "html" << "cpp" << "hpp" << "c" << "h" << "py" << "gz" << "zip" << "bz2" << "7z" << "doc" << "ppt" << "xls";
	suffixes << "pdf" << "" << "ttf" << "mp3" << "ogg" << "flac" << "mp4" << "avi" << "flv" << "png" << "jpg" << "gif";

	QStringList names;
	names << "Folder" << "Text File" << "Html File" << "C++ Src" << "C++ Hdr" << "C Src" << "C Hdr" << "Python File" << "GZip Archive";
	names << "Zip Archive" << "BZip2 Archive" << "7z Archive" << "MSWord" << "MS Powerpoint" << "MS Excel" << "PDF File";
	names << "Executable" << "Font File" << "MP3 File" << "OGG File" << "FLAC File" << "MP4 File" << "AVI File" << "FLV File";
	names << "PNG File" << "JPG File" << "GIF File";

	for( int i = 0; i < suffixes.count(); i++ ) {
		QIcon icon = NBIcon::icon( suffixes[ i ], mimetypes[ i ] );
		QListWidgetItem *item = new QListWidgetItem( icon, names[ i ] );

		addItem( item );
	}
};

NBShortcutsWidget::NBShortcutsWidget( QString name ) : QWidget() {

	action = QString( name );

	setKeyBindingBtn = new QPushButton( getShortcutList( name ).value( 0 ) );
	setKeyBindingBtn->setToolTip( "Click to set a new key binding" );
	connect( setKeyBindingBtn, SIGNAL( clicked() ), this, SLOT( changeKeySequence() ) );

	clearBindingBtn = new QToolButton();
	clearBindingBtn->setIcon( QIcon( ":/icons/edit-clear.png" ) );
	clearBindingBtn->setToolTip( "Click to clear the binding" );
	connect( clearBindingBtn, SIGNAL( clicked() ), this, SLOT( clearBinding() ) );

	resetBindingBtn = new QToolButton();
	resetBindingBtn->setIcon( QIcon( ":/icons/reload.png" ) );
	resetBindingBtn->setToolTip( "Click to restore the default key binding" );
	connect( resetBindingBtn, SIGNAL( clicked() ), this, SLOT( resetBinding() ) );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->addWidget( setKeyBindingBtn );
	lyt->addWidget( clearBindingBtn );
	lyt->addWidget( resetBindingBtn );

	setLayout( lyt );
};

void NBShortcutsWidget::changeKeySequence() {

	NBKeyBinder *keyBinder = new NBKeyBinder();
	keyBinder->exec();

	if ( keyBinder->result() == QDialog::Accepted ) {
		setKeyBindingBtn->setText( keyBinder->keyBinding() );
		shortcutSettings.setValue( action, keyBinder->keyBinding() );
		shortcutSettings.sync();

		Settings.readSettings();
	}
};

void NBShortcutsWidget::clearBinding() {

	setKeyBindingBtn->setText( "None" );
	shortcutSettings.setValue( action , QVariant() );
	shortcutSettings.sync();

	Settings.readSettings();
};

void NBShortcutsWidget::resetBinding() {

	QSettings defaultSettings( ":/Shortcuts.conf", QSettings::NativeFormat );

	setKeyBindingBtn->setText( defaultSettings.value( action ).toString() );
	shortcutSettings.setValue( action , defaultSettings.value( action ) );
	shortcutSettings.sync();

	Settings.readSettings();
};
