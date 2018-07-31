/*
	*
	* NBInfoPanel.cpp - InfoPanel class for NewBreeze
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

	/* Basic Settings */
	setContentsMargins( QMargins() );
	setFocusPolicy( Qt::NoFocus );

	/* Widgets */
	iconLbl = new QLabel();
	iconLbl->setFixedWidth( 256 );
	iconLbl->setMinimumHeight( 256 );
	iconLbl->setAlignment( Qt::AlignCenter );

	nameLbl = new QLabel();
	nameLbl->setFixedWidth( 256 );
	nameLbl->setWordWrap( true );
	nameLbl->setAlignment( Qt::AlignCenter );

	sizeLbl = new QLabel();
	sizeLbl->setFixedWidth( 256 );
	sizeLbl->setWordWrap( true );
	sizeLbl->setAlignment( Qt::AlignCenter );

	pathLbl = new QLabel();
	pathLbl->setFixedWidth( 256 );
	pathLbl->setWordWrap( true );
	pathLbl->setAlignment( Qt::AlignCenter );

	permLbl = new QLabel();
	permLbl->setFixedWidth( 256 );
	permLbl->setWordWrap( true );
	permLbl->setAlignment( Qt::AlignCenter );

	mimeLbl = new QLabel();
	mimeLbl->setFixedWidth( 256 );
	mimeLbl->setWordWrap( true );
	mimeLbl->setAlignment( Qt::AlignCenter );

	/* Layouts */
	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setAlignment( Qt::AlignCenter );

	baseLyt->addWidget( iconLbl );
	baseLyt->addWidget( nameLbl );
	baseLyt->addWidget( sizeLbl );
	baseLyt->addWidget( pathLbl );
	baseLyt->addWidget( permLbl );
	baseLyt->addWidget( mimeLbl );
	baseLyt->addStretch( 0 );

	setLayout( baseLyt );
};

void NBInfoPanel::updatePanel( QString rootPath, QModelIndexList selection ) {

	nameLbl->clear();
	sizeLbl->clear();
	pathLbl->clear();
	permLbl->clear();
	mimeLbl->clear();

	if ( rootPath.startsWith( "NB://SuperStart" ) ) {
		if ( selection.isEmpty() ) {
			setIcon( QIcon( ":/icons/superstart.png" ) );
			nameLbl->setText( "<b>SuperStart</b>");
			pathLbl->setText( "Give a super start to your day!" );
		}

		else if ( selection.count() == 1 ) {
			/* Drives */
			if ( exists( selection.at( 0 ).data( Qt::UserRole + 2 ).toString() ) ) {
				QMimeType mType = mimeDb.mimeTypeForFile( selection.at( 0 ).data( Qt::UserRole + 2 ).toString() );
				setIcon( selection.at( 0 ).data( Qt::DecorationRole ).value<QIcon>() );
				if ( selection.at( 0 ).data( Qt::UserRole + 2 ).toString() == "/" )
					nameLbl->setText( "<b>/ (root)</b>" );
				else
					nameLbl->setText( "<b>" + baseName( selection.at( 0 ).data( Qt::UserRole + 2 ).toString() ) + "</b>" );

				/* Size */
				sizeLbl->setText( QString( "%1" ).arg( selection.at( 0 ).data( Qt::UserRole + 1 ).toString() ) );
				mimeLbl->setText( mType.comment() );

				/* Parent folder */
				QString dn( dirName( selection.at( 0 ).data( Qt::UserRole + 2 ).toString() ) );
				if ( dn == "/" )
					pathLbl->setText( "/ (root)" );
				else
					pathLbl->setText( dn );

				permLbl->setText( getPermissions( selection.at( 0 ).data( Qt::UserRole + 2 ).toString() ) );
			}

			/* Folders and Files */
			else {
				QMimeType mType = mimeDb.mimeTypeForFile( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() );
				// setIcon( icon( NBIconManager::instance()->iconsForFile( mType.name(), selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) ) );
				setIcon( selection.at( 0 ).data( Qt::DecorationRole ).value<QIcon>() );
				nameLbl->setText( "<b>" + baseName( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) + "</b>" );

				/* Size */
				QString sizeTxt;
				if ( isDir( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) )
					sizeTxt = QString( "%1 items" ).arg( nChildren( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) );

				else
					sizeTxt = formatSize( getSize( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) );

				sizeLbl->setText( sizeTxt );
				mimeLbl->setText( mType.comment() );
				pathLbl->setText( dirName( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) );
				permLbl->setText( getPermissions( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) );
			}
		}

		else {
			// No multiple selection possible
		}
	}

	else if ( rootPath.startsWith( "NB://Applications" ) ) {
		if ( selection.isEmpty() ) {
			setIcon( QIcon( ":/icons/applications.png" ) );
			nameLbl->setText( "<b>Applications</b>");
			pathLbl->setText( "Run an application without leaving NB3" );
		}

		else if ( selection.count() == 1 ) {

		}

		else {

		}
	}

	else if ( rootPath.startsWith( "NB://Catalogs" ) ) {
		if ( selection.isEmpty() ) {
			setIcon( QIcon( ":/icons/superstart.png" ) );
			nameLbl->setText( "<b>Catalogs</b>");
			pathLbl->setText( "Browse your Catalogs" );
		}

		else if ( selection.count() == 1 ) {
			QMimeType mType = mimeDb.mimeTypeForFile( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() );
			setIcon( selection.at( 0 ).data( Qt::DecorationRole ).value<QIcon>() );
			nameLbl->setText( "<b>" + baseName( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) + "</b>" );

			/* Size */
			QString sizeTxt;
			if ( isDir( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) )
				sizeTxt = QString( "%1 items" ).arg( nChildren( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) );

			else
				sizeTxt = formatSize( getSize( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) );

			sizeLbl->setText( sizeTxt );
			mimeLbl->setText( mType.comment() );
			pathLbl->setText( dirName( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) );
			permLbl->setText( getPermissions( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) );
		}

		else {
			// No multiple selection possible
		}
	}

	else {
		if ( selection.isEmpty() ) {
			setIcon( icon( NBIconManager::instance()->iconsForFile( "inode/directory", rootPath ) ) );
			if ( rootPath.count( "/" ) > 1 )
				nameLbl->setText( "<b>" + baseName( rootPath ) + "</b>" );

			else
				nameLbl->setText( "<b>/ (root)</b>" );

			/* Size */
			int folders = 0, files = 0, others = 0;
			QDir dir( rootPath );
			dir.setFilter( QDir::Dirs | QDir::NoDotAndDotDot );
			folders = dir.entryList().count();

			dir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
			files = dir.entryList().count();

			dir.setFilter( QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot );
			others = dir.entryList().count() - folders - files;

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

			sizeLbl->setText( sizeTxt );
			mimeLbl->setText( "Folder" );
			pathLbl->setText( dirName( rootPath ) );
			permLbl->setText( getPermissions( rootPath ) );
		}

		else if ( selection.count() == 1 ) {
			QMimeType mType = mimeDb.mimeTypeForFile( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() );
			setIcon( icon( NBIconManager::instance()->iconsForFile( mType.name(), selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) ) );
			nameLbl->setText( "<b>" + selection.at( 0 ).data().toString() + "</b>" );

			/* Size */
			QString sizeTxt;
			if ( isDir( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) )
				sizeTxt = QString( "%1 items" ).arg( nChildren( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) );

			else
				sizeTxt = formatSize( getSize( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) );

			sizeLbl->setText( sizeTxt );
			mimeLbl->setText( mType.comment() );
			pathLbl->setText( rootPath );
			permLbl->setText( getPermissions( selection.at( 0 ).data( Qt::UserRole + 7 ).toString() ) );
		}

		else {
			int folders = 0, files = 0, others = 0;
			qint64 totsize = 0;

			foreach( QModelIndex entry, selection ) {
				QString itemPath = entry.data( Qt::UserRole + 7 ).toString();
				if ( not exists( itemPath ) )
					continue;

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

			if ( folders )
				nameText += QString( "%1 folder%2" ).arg( folders ).arg( folders > 1 ? "s" : "" );

			if ( files ) {
				if ( folders )
					nameText += QString( ", %1 file%2" ).arg( files ).arg( files > 1 ? "s" : "" );

				else
					nameText += QString( "%1 file%2" ).arg( files ).arg( files > 1 ? "s" : "" );
			}

			if ( others ) {
				if ( files or folders )
					nameText += QString( ", %1 other%2" ).arg( others ).arg( others > 1 ? "s" : "" );

				else
					nameText += QString( "%1 other%2" ).arg( others ).arg( others > 1 ? "s" : "" );
			}

			if ( folders and not ( files + others ) )
				icon = QIcon( ":/icons/folders.png" );

			else if ( not folders and files + others )
				icon = QIcon( ":/icons/documents.png" );

			else
				icon = QIcon( ":/icons/others.png" );

			setIcon( icon );
			nameLbl->setText( nameText );
			sizeLbl->setText( totsize ? formatSize( totsize ) : "" );
			pathLbl->setText( rootPath );
		}
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
