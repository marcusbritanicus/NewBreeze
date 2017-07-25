/*
	*
	* NBInfoPanel.cpp - SideBar class for NewBreeze
	*
*/

#include "NBInfoPanel.hpp"
#include "NBGuiFunctions.hpp"
#include "NBIconManager.hpp"

inline static QString getPermissions( QString path ) {

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

NBInfoPanel::NBInfoPanel( QWidget *parent ) :QWidget( parent ) {

	setFixedWidth( 300 );

	/* Basic Settings */
	setContentsMargins( QMargins() );
	setFocusPolicy( Qt::NoFocus );

	/* Widgets */
	iconLbl = new QLabel();
	iconLbl->setFixedSize( QSize( 256, 256 ) );
	iconLbl->setAlignment( Qt::AlignCenter );

	infoLbl = new QLabel();
	infoLbl->setWordWrap( true );
	infoLbl->setAlignment( Qt::AlignCenter );

	/* Layouts */
	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setAlignment( Qt::AlignCenter );

	baseLyt->addWidget( iconLbl );
	baseLyt->addWidget( infoLbl );
	baseLyt->addStretch( 0 );

	setLayout( baseLyt );
};

void NBInfoPanel::updatePanel( QString rootPath, QModelIndexList selection ) {

	if ( selection.count() == 0 ) {
		// Icon
		if ( rootPath.startsWith( "NB://Super" ) )
			setIcon( QIcon( ":/icons/superstart.png" ) );

		else if ( rootPath.startsWith( "NB://App" ) )
			setIcon( QIcon( ":/icons/applications.png" ) );

		else if ( rootPath.startsWith( "NB://Cat" ) )
			setIcon( QIcon( ":/icons/catalogs.png" ) );

		else
			setIcon( icon( NBIconManager::instance()->iconsForFile( "", rootPath ) ) );

		// Name
		QString name;
		if ( rootPath.startsWith( "NB://" ) )
			name = rootPath;

		else if ( rootPath.split( "/", QString::SkipEmptyParts ).count() )
			name = rootPath.split( "/", QString::SkipEmptyParts ).takeLast();

		else
			name = "/ (root)";

		if ( rootPath.startsWith( "NB://" ) ) {
			infoLbl->setText( QString( "<center><b>%1</b><br>%2<br>%3<br>%4<br></center>" ).arg( name ).arg( QString() ).arg( QString() ).arg( QString() ) );
			return;
		}

		// Size
		int folders = 0, files = 0, others = 0;
		QDir dir( rootPath );
		dir.setFilter( QDir::Dirs | QDir::Files | QDir::System | QDir::NoDotAndDotDot );
		foreach( QString entry, dir.entryList() ) {
			if ( isDir( QDir( rootPath ).filePath( entry ) ) )
				folders++;

			else if ( isFile( QDir( rootPath ).filePath( entry ) ) )
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
			sizeTxt += QString( "%1 items" ).arg( QDir( rootPath ).entryList().count() );

		infoLbl->setText( QString( "<center><b>%1</b><br>%2<br>%3<br>%4<br></center>" ).arg( name ).arg( sizeTxt ).arg( "Folder" ).arg( getPermissions( rootPath ) ) );
	}

	else if ( selection.count() == 1 ) {
		QString itemPath = QDir( rootPath ).filePath( selection.at( 0 ).data().toString() );

		// Icon
		setIcon( icon( NBIconManager::instance()->iconsForFile( "", itemPath ) ) );

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
		infoLbl->setText( QString( "<center><b>%1</b><br>%2<br>%3<br>%4<br></center>" ).arg( name ).arg( sizeTxt ).arg( typeTxt ).arg( permTxt ) );
	}

	else {
		int folders = 0, files = 0, others = 0;
		qint64 totsize = 0;

		foreach( QModelIndex entry, selection ) {
			QString itemPath = QDir( rootPath ).filePath( entry.data().toString() );

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
			nameText += QString( "%1 items" ).arg( QDir( rootPath ).entryList().count() );
			if ( totsize )
				sizeText = formatSize( totsize );
			icon = QIcon::fromTheme( "folder" );
		}

		// Icon
		setIcon( icon );

		// Info
		infoLbl->setText( QString( "<center><b>%1</b><br>%2<br>%3<br>%4<br></center>" ).arg( nameText ).arg( sizeText ).arg( QString() ).arg( QString() ) );
	}
};

void NBInfoPanel::setIcon( QIcon icon ) {

	iconLbl->setPixmap( icon.pixmap( QSize( 128, 128 ) ) );
};

void NBInfoPanel::setIcon( QPixmap icon ) {

	iconLbl->setPixmap( icon );
};

void NBInfoPanel::paintEvent( QPaintEvent *pEvent ) {

	QWidget::paintEvent( pEvent );

	QPainter painter( this );
	painter.setRenderHint( QPainter::HighQualityAntialiasing );
	painter.setPen( Qt::darkGray );

	painter.drawLine( rect().topLeft(), rect().bottomLeft() );
	painter.end();

	pEvent->accept();
};
