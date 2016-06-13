/*
	*
	* NBInfoBar.cpp - Custom InfoBar for NB
	*
*/

#include "NBInfoBar.hpp"

inline QString getPermissions( QString path ) {

	QString text;
	// Read Permissions
	if ( isReadable( path ) )
		text += "<font color='#008000'><b>R</b></font>";

	else
		text += "<font color='#800000'><b>R</b></font>";

	// Write Permissions
	if ( isWritable( path ) )
		text += " <font color='#008000'><b>W</b></font>";

	else
		text += " <font color='#800000'><b>W</b></font>";

	// Execute Permissions
	if ( not isDir( path ) ) {
		if ( access( path.toLocal8Bit().data(), X_OK ) == 0 )
			text += " <font color='#008000'><b>X</b></font>";

		else
			text += " <font color='#800000'><b>X</b></font>";
	}

	else {
		if ( access( path.toLocal8Bit().data(), X_OK ) != 0 )
			text = "<font color='#800000'><b>R W</b></font>";
	}

	return text;
};

NBInfoBar::NBInfoBar( QWidget *parent ) : QWidget( parent ) {

	/* Basic Settings */
	setFixedHeight( 64 );
	setContentsMargins( QMargins() );
	setFocusPolicy( Qt::NoFocus );

	/* Widgets */
	iconLbl = new QLabel();
	iconLbl->setFixedSize( QSize( 52, 52 ) );
	iconLbl->setAlignment( Qt::AlignCenter );

	infoLbl = new NBInfoLabel();
	infoLbl->setMinimumWidth( 700 );

	/* Layouts */
	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setSpacing( 0 );

	baseLyt->addWidget( iconLbl );
	baseLyt->addWidget( infoLbl );
	baseLyt->addStretch( 0 );

	QWidget *base = new QWidget( this );
	base->setObjectName( "base" );
	base->setLayout( baseLyt );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setContentsMargins( QMargins() );
	lyt->setSpacing( 0 );
	lyt->addWidget( base );

	setLayout( lyt );

	/* Styling */
	setStyleSheet( "QWidget#base { border-top: 1px solid gray; }" );
};

void NBInfoBar::updateInfoBarCF( QString folderPath ) {

	// Icon
	QString icoStr = QSettings( QDir( folderPath ).filePath( ".directory" ), QSettings::NativeFormat ).value( "Desktop Entry/Icon" ).toString();
	if ( icoStr.isNull() )
		setIcon( QIcon::fromTheme( "folder", QIcon( ":/icons/folder.png" ) ) );

	else
		setIcon( QIcon::fromTheme( icoStr, QIcon( icoStr ) ) );

	// Name
	QString name;
	if ( folderPath.split( "/", QString::SkipEmptyParts ).count() )
		name = folderPath.split( "/", QString::SkipEmptyParts ).takeLast();

	else
		name = "/ (root)";

	// Size
	int folders = 0, files = 0, others = 0;
	QDir dir( folderPath );
	dir.setFilter( QDir::Dirs | QDir::Files | QDir::System | QDir::NoDotAndDotDot );
	foreach( QString entry, dir.entryList() ) {
		if ( isDir( QDir( folderPath ).filePath( entry ) ) )
			folders++;

		else if ( isFile( QDir( folderPath ).filePath( entry ) ) )
			files++;

		else
			others++;
	}
	QString sizeTxt;
	if ( folders and files and others )
		sizeTxt += QString( "%1 Folders, %2 Files, %3 Others" ).arg( folders ).arg( files ).arg( others );

	else if ( folders and files )
		sizeTxt += QString( "%1 Folders, %2 Files" ).arg( folders ).arg( files );

	else if ( folders and others )
		sizeTxt += QString( "%1 Folders, %2 Others" ).arg( folders ).arg( others );

	else if ( files and others )
		sizeTxt += QString( "%1 Files, %2 Others" ).arg( files ).arg( others );

	else if ( folders )
		sizeTxt += QString( "%1 Folders" ).arg( folders );

	else if ( files )
		sizeTxt += QString( "%1 Files" ).arg( files );

	else if ( others )
		sizeTxt += QString( "%1 Others" ).arg( others );

	else
		sizeTxt += QString( "%1 items" ).arg( QDir( folderPath ).entryList().count() );

	infoLbl->setText( name, sizeTxt, "Folder", getPermissions( folderPath ) );
};

void NBInfoBar::updateInfoBarSingle( QString itemPath ) {

	// Icon
	QString icoStr = NBIconProvider::icon( itemPath );
	setIcon( QIcon::fromTheme( icoStr, QIcon( icoStr ) ) );

	// Name
	QString name = itemPath.split( "/", QString::SkipEmptyParts ).takeLast();
	if ( isLink( itemPath ) )
		name += QString( "<tt> > %1</tt>" ).arg( readLink( itemPath ) );

	// Size
	QString sizeTxt;
	if ( isDir( itemPath ) )
		sizeTxt = QString( "%1 items" ).arg( nChildren( itemPath ) );

	else
		sizeTxt = formatSize( getSize( itemPath ) );

	// Type
	QString typeTxt = mimeDb.mimeTypeForFile( itemPath ).comment();

	// Perm
	QString permTxt = getPermissions( itemPath );

	// Info
	infoLbl->setText( name, sizeTxt, typeTxt, permTxt );
};

void NBInfoBar::updateInfoBarSelection( QString folderPath, QModelIndexList selection ) {

	int folders = 0, files = 0, others = 0;
	qint64 totsize = 0;

	foreach( QModelIndex entry, selection ) {
		QString itemPath = QDir( folderPath ).filePath( entry.data().toString() );

		if ( isDir( itemPath ) ) {
			folders++;
		}

		else if ( isFile( itemPath ) ) {
			files++;
			totsize += getSize( itemPath );
		}

		else {
			others++;
			totsize += getSize( itemPath );
		}
	}

	QString nameText, sizeText;
	QIcon icon;
	if ( folders and files and others ) {
		nameText += QString( "%1 Folders, %2 Files, %3 Others" ).arg( folders ).arg( files ).arg( others );
		sizeText = formatSize( totsize );
		icon = QIcon( ":/icons/others.png" );
	}

	else if ( folders and files ) {
		nameText += QString( "%1 Folders, %2 Files" ).arg( folders ).arg( files );
		sizeText = formatSize( totsize );
		icon = QIcon( ":/icons/others.png" );
	}

	else if ( folders and others ) {
		nameText += QString( "%1 Folders, %2 Others" ).arg( folders ).arg( others );
		sizeText = formatSize( totsize );
		icon = QIcon( ":/icons/others.png" );
	}

	else if ( files and others ) {
		nameText += QString( "%1 Files, %2 Others" ).arg( files ).arg( others );
		sizeText = formatSize( totsize );
		icon = QIcon( ":/icons/documents.png" );
	}

	else if ( folders ) {
		nameText += QString( "%1 Folders" ).arg( folders );
		icon = QIcon( ":/icons/folders.png" );
	}

	else if ( files ) {
		nameText += QString( "%1 Files" ).arg( files );
		sizeText = formatSize( totsize );
		icon = QIcon( ":/icons/documents.png" );
	}

	else if ( others ) {
		nameText += QString( "%1 Others" ).arg( others );
		if ( totsize )
			sizeText = formatSize( totsize );
		icon = QIcon( ":/icons/documents.png" );
	}

	else {
		nameText += QString( "%1 items" ).arg( QDir( folderPath ).entryList().count() );
		if ( totsize )
			sizeText = formatSize( totsize );
		icon = QIcon::fromTheme( "folder" );
	}

	// Icon
	setIcon( icon );

	// Info
	infoLbl->setText( nameText, sizeText, QString(), QString() );
};

void NBInfoBar::clear() {

	iconLbl->clear();
	infoLbl->clear();
};

void NBInfoBar::setIcon( QIcon icon ) {

	iconLbl->setPixmap( icon.pixmap( QSize( 48, 48 ) ) );
};

void NBInfoBar::setIcon( QPixmap icon ) {

	iconLbl->setPixmap( icon );
};
