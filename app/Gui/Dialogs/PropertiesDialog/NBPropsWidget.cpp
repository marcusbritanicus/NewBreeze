/*
	*
	* NBPropsWidget.cpp - Show the node properties of the selected object(s)
	*
*/

#include "NBPropsWidget.hpp"
#include "NBIconManager.hpp"
#include "NBGuiFunctions.hpp"

NBPropertiesWidget::NBPropertiesWidget( QStringList paths, QWidget *parent ) : QWidget( parent ) {

	pathsList << paths;
	createGUI();

	runner = 0;
};

NBPropertiesWidget::~NBPropertiesWidget() {

	runner->stop();

	delete runner;
};

void NBPropertiesWidget::createGUI() {

	nameLbl = new QLabel();

	iconLbl = new NBClickLabel( QPixmap( 36, 36 ), this );
	iconLbl->setFixedSize( QSize( 48, 48 ) );
	iconLbl->setAlignment( Qt::AlignCenter );
	iconLbl->setStyleSheet( "border: 1px solid gray; border-radius: 3px;" );
	connect( iconLbl, SIGNAL( clicked() ), this, SLOT( changeDirIcon() ) );

	QWidget *sep1 = new QWidget();
	sep1->setFixedHeight( 1 );
	sep1->setStyleSheet( "background-color: gray;" );

	sizeLbl = new QLabel();
	typeLbl = new QLabel();
	contLbl = new QLabel();

	QWidget *sep2 = new QWidget();
	sep2->setFixedHeight( 1 );
	sep2->setStyleSheet( "background-color: gray;" );

	aDateLbl = new QLabel();
	mDateLbl = new QLabel();
	cDateLbl = new QLabel();

	NBDriveInfo *dInfo = new NBDriveInfo( NBDeviceManager::deviceInfoForPath( pathsList.at( 0 ) ), this );

	QGridLayout *nameLyt = new QGridLayout();
	nameLyt->addWidget( nameLbl, 0, 0, Qt::AlignVCenter | Qt::AlignLeft );
	nameLyt->addWidget( iconLbl, 0, 1 );

	nameLyt->addWidget( sep1, 2, 0, 1, 2 );

	QGridLayout *infoLyt = new QGridLayout();
	infoLyt->addWidget( new QLabel( "Size" ), 0, 0, Qt::AlignVCenter | Qt::AlignLeft );
	infoLyt->addWidget( sizeLbl, 0, 1, Qt::AlignVCenter | Qt::AlignRight );
	infoLyt->addWidget( new QLabel( "Type" ), 1, 0, Qt::AlignVCenter | Qt::AlignLeft );
	infoLyt->addWidget( typeLbl, 1, 1, Qt::AlignVCenter | Qt::AlignRight );
	infoLyt->addWidget( new QLabel( "Contents" ), 2, 0, Qt::AlignVCenter | Qt::AlignLeft );
	infoLyt->addWidget( contLbl, 2, 1, Qt::AlignVCenter | Qt::AlignRight );

	infoLyt->addWidget( new QLabel( " " ), 3, 0, 1, 2 );

	infoLyt->addWidget( new QLabel( "Last Accessed" ), 4, 0, Qt::AlignVCenter | Qt::AlignLeft );
	infoLyt->addWidget( aDateLbl, 4, 1, Qt::AlignVCenter | Qt::AlignRight );
	infoLyt->addWidget( new QLabel( "Last Modified" ), 5, 0, Qt::AlignVCenter | Qt::AlignLeft );
	infoLyt->addWidget( mDateLbl, 5, 1, Qt::AlignVCenter | Qt::AlignRight );
	infoLyt->addWidget( new QLabel( "Last Status Change" ), 6, 0, Qt::AlignVCenter | Qt::AlignLeft );
	infoLyt->addWidget( cDateLbl, 6, 1, Qt::AlignVCenter | Qt::AlignRight );

	infoLyt->addWidget( sep2, 7, 0, 1, 2 );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addLayout( nameLyt );
	baseLyt->addLayout( infoLyt );
	baseLyt->addWidget( dInfo );

	setLayout( baseLyt );

	update();
};

void NBPropertiesWidget::refreshSize() {

	if ( runner and runner->isRunning() )
		runner->stop();

	runner = new NBSizeRunner( pathsList );
	connect( runner, SIGNAL( currentCount( qint64, qint64, qint64 ) ), this, SLOT( update( qint64, qint64, qint64 ) ) );

	runner->start();
};

void NBPropertiesWidget::update() {

	QFileInfo info( pathsList.value( 0 ) );

	if ( pathsList.count() == 1 ) {

		/* Name and Path */
		QString fileName = info.fileName();

		nameLbl->setText( "<b><large>" + fileName + "</large></b><br>" + info.path() );

		/* Mime Type */
		QMimeType mType = mimeDb.mimeTypeForFile( pathsList.at( 0 ) );

		if ( info.isDir() ) {
			iconLbl->setPixmap( QIcon( NBIconManager::instance()->iconsForFile( "inode/directory", info.filePath() ).at( 0 ) ).pixmap( 36 ) );
			contLbl->setText( "1 folder" );
		}

		else {
			iconLbl->setPixmap( QIcon( NBIconManager::instance()->iconsForFile( mType.name(), info.filePath() ).at( 0 ) ).pixmap( 36 ) );
			contLbl->setText( "1 file" );
		}

		typeLbl->setText( QString( "%1 (%2)" ).arg( mType.name() ).arg( mType.comment() ) );
	}

	else {
		int d = 0, f = 0;
		Q_FOREACH(  QString path, pathsList ) {
			QFileInfo info( path );
			if ( info.isDir() )
				d++;

			else
				f++;
		}

		/* Files and Folders */
		if ( d and f ) {
			nameLbl->setText(
				QString(
					"<b><large>%1 file%2 and %3 folder%4</large></b><br>%5" ).arg( f ).arg( ( f == 1 ? "" : "s" ) ).arg( d ).arg( ( d == 1 ? "" : "s" ) ).arg( info.path() )
			);
			iconLbl->setPixmap( QIcon( ":/icons/others.png" ).pixmap( 36 ) );
			typeLbl->setText( "Files and Folders" );
		}

		/* Files and not folders */
		else if ( not d and f ) {
			nameLbl->setText( QString( "<b><large>%1 file%2</large></b><br>%3" ).arg( f ).arg( ( f == 1 ? "" : "s" ) ).arg( info.path() ) );
			iconLbl->setPixmap( QIcon( ":/icons/documents.png" ).pixmap( 36 ) );
			typeLbl->setText( "Files" );
		}

		/* Folders and not files */
		else {
			nameLbl->setText( QString( "<b><large>%1 folder%2</large></b><br>%3" ).arg( d ).arg( ( d == 1 ? "" : "s" ) ).arg( info.path() ) );
			iconLbl->setPixmap( QIcon( ":/icons/folders.png" ).pixmap( 36 ) );
			typeLbl->setText( "Folders" );
		}
	}

	sizeLbl->setText( "0 bytes (0 bytes)" );

	/* Updating the various times */
	if ( pathsList.count() == 1 ) {
		aDateLbl->setText( info.lastRead().toString( "MMM dd, yyyy" ) );
		mDateLbl->setText( info.lastModified().toString( "MMM dd, yyyy" ) );
		cDateLbl->setText( info.metadataChangeTime().toString( "MMM dd, yyyy" ) );
	}

	else {
		QDateTime aMin, mMin, cMin;
		QDateTime aMax, mMax, cMax;

		aMax = aMin = info.lastRead();
		aMax = mMin = info.lastModified();
		aMax = cMin = info.metadataChangeTime();

		Q_FOREACH( QString path, pathsList ) {
			struct stat timeSt;
			if ( stat( path.toLocal8Bit().data(), &timeSt ) == 0 ) {

				aMin = qMin( QDateTime::fromTime_t( timeSt.st_atim.tv_sec ), aMin );
				mMin = qMin( QDateTime::fromTime_t( timeSt.st_mtim.tv_sec ), mMin );
				cMin = qMin( QDateTime::fromTime_t( timeSt.st_ctim.tv_sec ), cMin );

				aMax = qMax( QDateTime::fromTime_t( timeSt.st_atim.tv_sec ), aMax );
				mMax = qMax( QDateTime::fromTime_t( timeSt.st_mtim.tv_sec ), mMax );
				cMax = qMax( QDateTime::fromTime_t( timeSt.st_ctim.tv_sec ), cMax );
			}
		}

		if ( aMin.date() == aMax.date() )
			aDateLbl->setText( aMax.toString( "MMM dd, yyyy" ) );
		else
			aDateLbl->setText( QString( "Between %1 and %2" ).arg( aMin.toString( "MMM dd, yyyy" ) ).arg( aMax.toString( "MMM dd, yyyy" ) ) );

		if ( mMin.date() == mMax.date() )
			mDateLbl->setText( mMax.toString( "MMM dd, yyyy" ) );
		else
			mDateLbl->setText( QString( "Between %1 and %2" ).arg( mMin.toString( "MMM dd, yyyy" ) ).arg( mMax.toString( "MMM dd, yyyy" ) ) );

		if ( cMin.date() == cMax.date() )
			cDateLbl->setText( cMax.toString( "MMM dd, yyyy" ) );
		else
			cDateLbl->setText( QString( "Between %1 and %2" ).arg( cMin.toString( "MMM dd, yyyy" ) ).arg( cMax.toString( "MMM dd, yyyy" ) ) );
	}
};

void NBPropertiesWidget::update( qint64 files, qint64 folders, qint64 totalSize ) {

	QString contentsStr;

	/* Both files and folders */
	if( files and folders ) {
		if ( ( files == 1 ) and ( folders == 1 ) )
			contentsStr = QString( tr( "1 file, 1 sub-folder" ) );

		else if ( ( files > 1 ) and ( folders == 1 ) )
			contentsStr = QString( tr( "%1 files, 1 sub-folder" ) ).arg( files );

		else if ( ( files == 1 ) and ( folders > 1 ) )
			contentsStr = QString( tr( "1 file, %2 sub-folders" ) ).arg( folders );

		else
			contentsStr = QString( tr( "%1 files, %2 sub-folders" ) ).arg( files ).arg( folders );
	}

	/* Only files and no folders */
	else if ( files and !folders ) {
		if ( files == 1 )
			contentsStr = QString( tr( "%1 file" ) ).arg( files );

		else
			contentsStr = QString( tr( "%1 files" ) ).arg( files );
	}

	/* No files */
	else {
		/* No folders as well */
		if ( not folders )
			contentsStr = "Empty";

		/* One folder */
		else if ( folders == 1 )
			contentsStr = "1 sub-folder";

		/* n folders */
		else
			contentsStr = QString( tr( "%1 sub-folders" ) ).arg( folders );
	}

	sizeLbl->setText( QString( "%1 (%2 byte%3)" ).arg( formatSize( totalSize ) ).arg( totalSize ).arg( ( totalSize == 1 ? "" : "s" ) ) );
	contLbl->setText( contentsStr );
};

void NBPropertiesWidget::changeDirIcon() {

	/* We don't work with multiple selection */
	if ( pathsList.count() > 1 )
		return;

	/* Icon can be changed only for folders */
	if ( not isDir( pathsList.at( 0 ) ) )
		return;

	/* Icon can only be changed if we have write access */
	if ( not isWritable( pathsList.at( 0 ) ) )
		return;

	QString iconName = NBFileDialog::getFileName(
			this,
			tr( "NewBreeze - Select Icon" ),
			"/usr/share/icons/",
			QStringList(
				QStringList() << "Images ( *.png *.bmp *.jpg *.svg *.gif *.ppm )" << "All Files (*)"
			),
			QString( "PNG (*.png)" )
	);

	if ( not iconName.isEmpty() ) {
		iconLbl->setPixmap( icon( QStringList() << iconName ).pixmap( 36 ) );

		QFile file( pathsList.at( 0 ) + "/.directory" );
		if ( not file.open( QFile::WriteOnly ) ) {
			QMessageBox::warning( this, "Error", "Unable to set icon" );
			return;
		}

		file.write( QString( "[Desktop Entry]\nIcon=%1" ).arg( iconName ).toUtf8() );
		file.flush();
		file.close();
	}
};

void NBSizeRunner::run() {

	files = 0;
	folders = 0;
	totalSize = 0;

	Q_FOREACH( QString path, mPaths ) {
		if ( mTerminate )
			return;

		if ( isDir( path ) ) {
			recurseProperties( path );
		}

		else {
			files++;
			totalSize += getSize( path );
		}
	}

	emit currentCount( files, folders, totalSize );
	mTerminate = false;
};

void NBSizeRunner::recurseProperties( QString path ) {

	QDirIterator it( path, QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden, QDirIterator::Subdirectories );
	while ( it.hasNext() ) {
		if ( mTerminate )
			return;

		it.next();
		if( it.fileInfo().isDir() ) {
			if ( it.filePath() == path )
				continue;

			folders++;
		}

		else {
			files++;
			totalSize += it.fileInfo().size();
		}

		if( ( folders + files ) % 32 == 0 )
			emit currentCount( files, folders, totalSize );
	}

	emit currentCount( files, folders, totalSize );
};

NBDriveInfo::NBDriveInfo( NBDeviceInfo info, QWidget *parent ) : QWidget ( parent ) {

	quint64 usedSize = info.bytesUsed();
	quint64 freeSize = info.bytesAvailable();
	quint64 totalSize = info.bytesTotal();

	percent = 100.0 * usedSize / totalSize;

	name = info.displayName();
	disk = info.device();
	icon = QString( ":/icons/%1.png" ).arg( info.deviceType() );

	sizeInfo = QString( "%1 free of %2 (%3% used)" ).arg( formatSize( freeSize ) ).arg( formatSize( totalSize ) ).arg( percent, 0, 'f', 2 );

	setFixedHeight( 64 );
};

void NBDriveInfo::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setRenderHints( QPainter::HighQualityAntialiasing | QPainter::Antialiasing | QPainter::TextAntialiasing );

	painter.save();
	painter.drawPixmap( QRect( 8, 8, 48, 48 ), QPixmap( icon ).scaled( QSize( 64, 64 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	painter.drawText( QRect( 74, 0, width(), height() ), Qt::AlignLeft | Qt::AlignVCenter, QString( "%1 (%2)\n%3" ).arg( name ).arg( disk ).arg( sizeInfo ) );
	painter.restore();

	painter.save();
	painter.setPen( Qt::darkGray );
	painter.setBrush( Qt::gray );
	painter.drawEllipse( QPointF( width() - 32.0, 32.0 ), 32.0, 32.0 );
	painter.restore();

	painter.save();
	qreal radius = 32 * sqrt( percent / 100 );
	painter.setPen( Qt::NoPen );
	painter.setBrush( percent / 100 < .9 ? Qt::darkGreen : Qt::darkRed );
	painter.drawEllipse( QPointF( width() - 32.0, 32.0 ), radius, radius );
	painter.restore();

	painter.save();
	painter.setPen( Qt::white );
	painter.setFont( QFont( font().family(), font().pointSize(), QFont::Bold ) );
	painter.drawText( QRect( width() - 64, 0, 64, 64 ), Qt::AlignCenter, QString( "%1%" ).arg( percent, 0, 'f', 2 ) );
	painter.restore();

	painter.end();
	pEvent->accept();
};
