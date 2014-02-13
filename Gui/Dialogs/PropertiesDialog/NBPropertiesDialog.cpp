/*
	*
	* NBPropertiesDialog.cpp - Properties of the selected object
	*
*/

#include <NBPropertiesDialog.hpp>

NBPropertiesBase::NBPropertiesBase( QStringList paths ) : QWidget() {

	pathsList << paths;

	// Name and Icon Labels
	iconLabel = new NBClickLabel( QPixmap() );
	iconLabel->setFixedSize( QSize( 48, 48 ) );
	connect( iconLabel, SIGNAL( clicked() ), this, SIGNAL( setDirIcon() ) );

	QString name;
	if ( pathsList.count() == 1 ) {
		QString iconName = NBIconProvider::icon( pathsList.at( 0 ) );
		iconLabel->setPixmap( QIcon::fromTheme( iconName, QIcon( iconName ) ).pixmap( 48, 48 ) );
		if ( isDir( pathsList.at( 0 ) ) )
			iconLabel->setClickable( true );

		else
			iconLabel->setClickable( false );

		name = QString(baseName( pathsList.at( 0 ) ) );
	}

	else {
		qint64 nFiles = 0, nFolders = 0;

		iconLabel->setClickable( false );
		Q_FOREACH( QString path, pathsList ) {
			if ( isDir( path ) )
				nFolders++;

			else
				nFiles++;
		}

		QString filesText = QString( "%1 file%2" ).arg( nFiles ).arg( ( nFiles > 1 ? "s" : "" ) );
		QString foldersText = QString( "%1 folder%2" ).arg( nFolders ).arg( ( nFolders > 1 ? "s" : "" ) );

		if ( nFiles and nFolders ) {
			iconLabel->setPixmap( QIcon( ":/icons/others.png" ).pixmap( QSize( 48, 48 ) ) );
			name = QString(filesText + ", " + foldersText );
		}

		else if ( nFiles and not nFolders ) {
			iconLabel->setPixmap( QIcon( ":/icons/documents.png" ).pixmap( QSize( 48, 48 ) ) );
			name = QString(filesText );
		}

		else {
			iconLabel->setPixmap( QIcon( ":/icons/folders.png" ).pixmap( QSize( 48, 48 ) ) );
			name = QString(foldersText );
		}

		setFixedHeight( 260 );
	}

	// Location Labels
	infoLabel = new QLabel();
	infoLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
	infoLabel->setFixedHeight( 48 );
	infoLabel->setText(
		QString( "<b>%1</b><br>%2" ).arg( name ).arg( dirName( pathsList.at( 0 ) ) )
	);

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );

	baseLyt->addWidget( iconLabel );
	baseLyt->addWidget( infoLabel );

	setLayout( baseLyt );

	setFixedHeight( 48 );
};

NBPropertiesWidget::NBPropertiesWidget( QStringList paths ) : QWidget() {

	pathsList << paths;
	terminate = false;

	createGUI();
};

NBPropertiesWidget::~NBPropertiesWidget() {

	terminate = true;
	thread.waitForFinished();
};

void NBPropertiesWidget::createGUI() {

	QFileInfo info( pathsList.at( 0 ) );

	QString typeStr;
	if ( pathsList.count() == 1 ) {

		if ( isDir( pathsList.at( 0 ) ) )
			typeStr = QString( "inode/directory<br>folder" );

		else
			typeStr = QString(
				"%1<br>%2"
			).arg( getMimeType( pathsList.at( 0 ) ) ).arg( mimeDb.mimeTypeForFile( pathsList.at( 0 ) ).comment() );
	}

	else {
		qint64 sFiles = 0, sFolders = 0;
		Q_FOREACH( QString path, pathsList ) {
			if ( isDir( path ) )
				sFolders++;

			else
				sFiles++;
		}

		if ( sFiles and sFolders )
			typeStr = QString( "Files and Folders" );

		else if ( sFiles and not sFolders )
			typeStr = QString( "Files" );

		else
			typeStr = QString( "Folders" );
	}

	sizeTypeStr = QString(
		"<table width='100%'>"
		"	<tr>"
		"		<td width='150px'>Size</td>"
		"		<td align='right' style='padding-bottom: 5px;'>%1 (%2 bytes)</td>"
		"	</tr>"
		"	<tr>"
		"		<td width='150px'>Type</td>"
		"		<td align='right' style='padding-bottom: 5px;'>" + typeStr + "</td>"
		"	</tr>"
		"	<tr>"
		"		<td>Contents</td>"
		"		<td align='right' style='padding-bottom: 0px;'>%3</td>"
		"	</tr>"
		"</table>"
	);

	sizeTypeLabel = new QLabel( sizeTypeStr );

	timeLabel = new QLabel();
	QString timeStr = QString(
		"<table width='100%'>"
		"	<tr>"
		"		<td>Last Accessed</td>"
		"		<td align='right' style='padding-bottom: 5px;'>%1</td>"
		"	</tr>"
		"	<tr>"
		"		<td>Last Modified</td>"
		"		<td align='right' style='padding-bottom: 5px;'>%2</td>"
		"	</tr>"
		"	<tr>"
		"		<td>Last Status Change</td>"
		"		<td align='right' style='padding-bottom: 0px;'>%3</td>"
		"	</tr>"
		"</table>"
	);

	if ( pathsList.count() > 1 ) {

		ctimeMin = QDateTime::currentDateTime().addDays( 1 );
		mtimeMin = QDateTime::currentDateTime().addDays( 1 );
		atimeMin = QDateTime::currentDateTime().addDays( 1 );

		ctimeMax = QDateTime( QDate( 1900, 1, 1 ) );
		mtimeMax = QDateTime( QDate( 1900, 1, 1 ) );
		atimeMax = QDateTime( QDate( 1900, 1, 1 ) );

		Q_FOREACH( QString path, pathsList ) {
			struct stat timeSt;
			if ( stat( qPrintable( path ), &timeSt ) == 0 ) {

				ctimeMin = qMin( QDateTime::fromTime_t( timeSt.st_ctim.tv_sec ), ctimeMin );
				mtimeMin = qMin( QDateTime::fromTime_t( timeSt.st_mtim.tv_sec ), mtimeMin );
				atimeMin = qMin( QDateTime::fromTime_t( timeSt.st_atim.tv_sec ), atimeMin );

				ctimeMax = qMax( QDateTime::fromTime_t( timeSt.st_ctim.tv_sec ), ctimeMax );
				mtimeMax = qMax( QDateTime::fromTime_t( timeSt.st_mtim.tv_sec ), mtimeMax );
				atimeMax = qMax( QDateTime::fromTime_t( timeSt.st_atim.tv_sec ), atimeMax );
			}
		}

		QString aDate, mDate, cDate;

		if ( atimeMin.date() == atimeMax.date() )
			aDate = atimeMin.toString( "MMM dd, yyyy" );
		else
			aDate = QString( "Between %1 and %2" ).arg( atimeMin.toString( "MMM dd, yyyy" ) ).arg( atimeMax.toString( "MMM dd, yyyy" ) );

		if ( mtimeMin.date() == mtimeMax.date() )
			mDate = mtimeMin.toString( "MMM dd, yyyy" );
		else
			mDate = QString( "Between %1 and %2" ).arg( mtimeMin.toString( "MMM dd, yyyy" ) ).arg( mtimeMax.toString( "MMM dd, yyyy" ) );

		if ( ctimeMin.date() == ctimeMax.date() )
			cDate = ctimeMin.toString( "MMM dd, yyyy" );
		else
			cDate = QString( "Between %1 and %2" ).arg( ctimeMin.toString( "MMM dd, yyyy" ) ).arg( ctimeMax.toString( "MMM dd, yyyy" ) );

		timeLabel->setText( timeStr.arg( aDate ).arg( mDate ).arg( cDate ) );
	}

	else {
		QString cDate = QFileInfo( pathsList.at( 0 ) ).lastRead().toString( "MMM dd, yyyy" );
		QString mDate = QFileInfo( pathsList.at( 0 ) ).lastModified().toString( "MMM dd, yyyy" );
		QString aDate = QFileInfo( pathsList.at( 0 ) ).created().toString( "MMM dd, yyyy" );

		timeLabel->setText( timeStr.arg( aDate ).arg( mDate ).arg( cDate ) );
	}

	QVBoxLayout *fileLyt = new QVBoxLayout();
	fileLyt->addWidget( sizeTypeLabel );
	fileLyt->addWidget( Separator::horizontal() );
	fileLyt->addWidget( timeLabel );

	NBDriveLabel *driveIcon;
	QLabel *driveName = new QLabel();
	NBDriveInfo *driveInfo;

	NBDeviceManager devMgr;
	NBDeviceInfo deviceInfo = devMgr.deviceInfoForPath( pathsList.at( 0 ) );

	driveIcon = new NBDriveLabel( deviceInfo.driveLabel() );
	driveName->setText( deviceInfo.driveName() );
	driveInfo = new NBDriveInfo( deviceInfo.usedSpace(), deviceInfo.driveSize() );

	QVBoxLayout *driveLyt = new QVBoxLayout();
	QGridLayout *subLytDrive1 = new QGridLayout();
	QHBoxLayout *subLytDrive2 = new QHBoxLayout();

	subLytDrive1->addWidget( driveIcon, 0, 0, Qt::AlignLeft );
	subLytDrive1->addWidget( driveName, 0, 1, Qt::AlignRight );
	subLytDrive2->addWidget( driveInfo );

	driveLyt->addLayout( subLytDrive1 );
	driveLyt->addLayout( subLytDrive2 );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->addLayout( fileLyt );
	baseLyt->addWidget( Separator::horizontal() );
	baseLyt->addLayout( driveLyt );
	setLayout( baseLyt );

	connect( this, SIGNAL( updateSignal() ), this, SLOT( update() ) );
	thread = QtConcurrent::run( this, &NBPropertiesWidget::folderProperties, pathsList );
};

void NBPropertiesWidget::folderProperties( QStringList paths ) {

	Q_UNUSED( paths );

	Q_FOREACH( QString path, pathsList ) {
		if( terminate or Settings->Special.ClosingDown )
			return;

		if ( isDir( path ) ) {
			recurseProperties( path );
		}

		else {
			files++;
			totalSize += getSize( path );
		}
	}

	emit updateSignal();
};

void NBPropertiesWidget::recurseProperties( QString path ) {

	QDirIterator it( path, QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden, QDirIterator::Subdirectories );
	while ( it.hasNext() ) {
		if( terminate or Settings->Special.ClosingDown )
			return;

		it.next();
		if( it.fileInfo().isDir() ) {
			if ( it.filePath() == path )
				continue;

			folders++;

			if( folders % 32 == 0 )
				emit updateSignal();
		}

		else {
			files++;
			totalSize += it.fileInfo().size();
		}
	}
};

void NBPropertiesWidget::update() {

	QString contentsStr;
	if( files and folders ) {
		if ( ( files == 1 ) and ( folders == 1 ) )
			contentsStr = QString( tr( "%1 file, %2 sub-folder" ) ).arg( files ).arg( folders );

		else if ( ( files > 1 ) and ( folders == 1 ) )
			contentsStr = QString( tr( "%1 files, %2 sub-folder" ) ).arg( files ).arg( folders );

		else if ( ( files == 1 ) and ( folders > 1 ) )
			contentsStr = QString( tr( "%1 files, %2 sub-folders" ) ).arg( files ).arg( folders );

		else
			contentsStr = QString( tr( "%1 files, %2 sub-folders" ) ).arg( files ).arg( folders );
	}

	else if ( files and !folders ) {
		if ( files == 1 )
			contentsStr = QString( tr( "%1 file" ) ).arg( files );

		else
			contentsStr = QString( tr( "%1 files" ) ).arg( files );
	}

	else {
		if ( not folders )
			contentsStr = "Empty";

		else if ( folders == 1 )
			contentsStr = "1 sub-folder";

		else
			contentsStr = QString( tr( "%1 sub-folders" ) ).arg( folders );
	}

	sizeTypeLabel->setText( sizeTypeStr.arg( formatSize( totalSize ) ).arg( totalSize ).arg( contentsStr ) );
};

NBPermissionsWidget::NBPermissionsWidget( QStringList paths ) : QWidget() {

	pathsList << paths;

	createGUI();
};

void NBPermissionsWidget::createGUI() {

	QFileInfo info1( pathsList.at( 0 ) );

	QLabel *usrGrpLbl = new QLabel();
	QString usrName = info1.owner();
	QString grpName = info1.group();
	if ( pathsList.count() > 1 ) {
		Q_FOREACH( QString path, pathsList ) {
			QFileInfo info( path );
			if ( info.owner() != usrName )
				usrName = "Various";

			if ( info.group() != grpName )
				grpName = "Various";
		}
	}

	usrGrpLbl->setText (
		QString(
			"<table width='100%'>"
			"	<tr>"
			"		<td width='15%'>User</td>"
			"		<td width='35%'>%1</td>"
			"		<td width='15%'>Group</td>"
			"		<td width='35%'>%2</td>"
			"	</tr>"
			"</table>"
		).arg( usrName ).arg( grpName )
	);

	uReadCheck = new QCheckBox();
	uWritCheck = new QCheckBox();
	uExecCheck = new QCheckBox();

	gReadCheck = new QCheckBox();
	gWritCheck = new QCheckBox();
	gExecCheck = new QCheckBox();

	oReadCheck = new QCheckBox();
	oWritCheck = new QCheckBox();
	oExecCheck = new QCheckBox();

	smartExecCheck = new QCheckBox( "Enable &Smart Executable" );
	smartExecCheck->setToolTip( "Set all the files which have an executable ancenstry to Executable" );

	carryCheck = new QCheckBox( "Appl&y changes to all subfolders and their contents" );

	connect( uReadCheck, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );
	connect( uWritCheck, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );
	connect( uExecCheck, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );
	connect( gReadCheck, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );
	connect( gWritCheck, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );
	connect( gExecCheck, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );
	connect( oReadCheck, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );
	connect( oWritCheck, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );
	connect( oExecCheck, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );

	connect( smartExecCheck, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );
	connect( carryCheck, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );

	readPermissions();

	QGridLayout *permsLyt = new QGridLayout();
	permsLyt->setContentsMargins( QMargins() );
	permsLyt->setSpacing( 10 );

	permsLyt->addWidget( new QLabel( tr( "Read" ) ), 0, 1 );
	permsLyt->addWidget( new QLabel( tr( "Write" ) ), 0, 2 );
	permsLyt->addWidget( new QLabel( tr( "Exec" ) ), 0, 3 );

	permsLyt->addWidget( new QLabel( tr( "Owner" ) ), 1, 0 );

	permsLyt->addWidget( uReadCheck, 1, 1 );
	permsLyt->addWidget( uWritCheck, 1, 2 );
	permsLyt->addWidget( uExecCheck, 1, 3 );

	permsLyt->addWidget( new QLabel( tr( "Group" ) ), 2, 0 );
	permsLyt->addWidget( gReadCheck, 2, 1 );
	permsLyt->addWidget( gWritCheck, 2, 2 );
	permsLyt->addWidget( gExecCheck, 2, 3 );

	permsLyt->addWidget( new QLabel( tr( "Other" ) ), 3, 0 );
	permsLyt->addWidget( oReadCheck, 3, 1 );
	permsLyt->addWidget( oWritCheck, 3, 2 );
	permsLyt->addWidget( oExecCheck, 3, 3 );

	QVBoxLayout *frameLyt = new QVBoxLayout();
	frameLyt->setSpacing( 10 );
	frameLyt->addLayout( permsLyt );
	frameLyt->addWidget( smartExecCheck );
	frameLyt->addWidget( carryCheck );

	QWidget *permsFrame = new QWidget( this );
	permsFrame->setLayout( frameLyt );

	if ( QFileInfo( pathsList.at( 0 ) ).ownerId() != QFileInfo( QDir::homePath() ).ownerId() )
		permsFrame->setDisabled( 1 );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->setContentsMargins( QMargins() );

	baseLyt->addWidget( usrGrpLbl );
	baseLyt->addWidget( permsFrame );
	baseLyt->addWidget( new QLabel( "Note: If you want to enter a directory, it must have the executable bit set." ) );
	baseLyt->addStretch();

	setLayout( baseLyt );
};

void NBPermissionsWidget::readPermissions() {

	short UR = 0, UW = 0, UE = 0, GR = 0, GW = 0, GE = 0, OR = 0, OW = 0, OE = 0;
	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = QFile::permissions( path );
		UR += ( perms & QFile::ReadOwner ? 1 : -1 );
		UW += ( perms & QFile::WriteOwner ? 1 : -1 );
		UE += ( perms & QFile::ExeOwner ? 1 : -1 );
		GR += ( perms & QFile::ReadGroup ? 1 : -1 );
		GW += ( perms & QFile::WriteGroup ? 1 : -1 );
		GE += ( perms & QFile::ExeGroup ? 1 : -1 );
		OR += ( perms & QFile::ReadOther ? 1 : -1 );
		OW += ( perms & QFile::WriteOther ? 1 : -1 );
		OE += ( perms & QFile::ExeOther ? 1 : -1 );
	}

	if ( -UR == pathsList.count() )
		uReadCheck->setCheckState( Qt::Unchecked );

	else if ( UR != pathsList.count() )
		uReadCheck->setCheckState( Qt::PartiallyChecked );

	else
		uReadCheck->setCheckState( Qt::Checked );

	if ( -UW == pathsList.count() )
		uWritCheck->setCheckState( Qt::Unchecked );

	else if ( UW != pathsList.count() )
		uWritCheck->setCheckState( Qt::PartiallyChecked );

	else
		uWritCheck->setCheckState( Qt::Checked );

	if ( -UE == pathsList.count() )
		uExecCheck->setCheckState( Qt::Unchecked );

	else if ( UE != pathsList.count() )
		uExecCheck->setCheckState( Qt::PartiallyChecked );

	else
		uExecCheck->setCheckState( Qt::Checked );

	if ( -GR == pathsList.count() )
		gReadCheck->setCheckState( Qt::Unchecked );

	else  if ( GR != pathsList.count() )
		gReadCheck->setCheckState( Qt::PartiallyChecked );

	else
		gReadCheck->setCheckState( Qt::Checked );

	if ( -GW == pathsList.count() )
		gWritCheck->setCheckState( Qt::Unchecked );

	else if ( GW != pathsList.count() )
		gWritCheck->setCheckState( Qt::PartiallyChecked );

	else
		gWritCheck->setCheckState( Qt::Checked );

	if ( -GE == pathsList.count() )
		gExecCheck->setCheckState( Qt::Unchecked );

	else if ( GE != pathsList.count() )
		gExecCheck->setCheckState( Qt::PartiallyChecked );

	else
		gExecCheck->setCheckState( Qt::Checked );

	if ( -OR == pathsList.count() )
		oReadCheck->setCheckState( Qt::Unchecked );

	else if ( OR != pathsList.count() )
		oReadCheck->setCheckState( Qt::PartiallyChecked );

	else
		oReadCheck->setCheckState( Qt::Checked );

	if ( -OW == pathsList.count() )
		oWritCheck->setCheckState( Qt::Unchecked );

	else if ( OW != pathsList.count() )
		oWritCheck->setCheckState( Qt::PartiallyChecked );

	else
		oWritCheck->setCheckState( Qt::Checked );

	if ( -OE == pathsList.count() )
		oExecCheck->setCheckState( Qt::Unchecked );

	else if ( OE != pathsList.count() )
		oExecCheck->setCheckState( Qt::PartiallyChecked );

	else
		oExecCheck->setCheckState( Qt::Checked );
};

void NBPermissionsWidget::applyPermissions() {

	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = NULL;

		if ( uReadCheck->isChecked() )
			perms |= ( ( uReadCheck->checkState() == Qt::Checked ) ? QFile::ReadOwner : ( QFile::permissions( path ) & QFile::ReadOwner ) );

		if ( uWritCheck->isChecked() )
			perms |= ( ( uWritCheck->checkState() == Qt::Checked ) ? QFile::WriteOwner : ( QFile::permissions( path ) & QFile::WriteOwner ) );

		if ( uExecCheck->isChecked() )
			perms |= ( ( uExecCheck->checkState() == Qt::Checked ) ? QFile::ExeOwner : ( QFile::permissions( path ) & QFile::ExeOwner ) );

		if ( gReadCheck->isChecked() )
			perms |= ( ( gReadCheck->checkState() == Qt::Checked ) ? QFile::ReadGroup : ( QFile::permissions( path ) & QFile::ReadGroup ) );

		if ( gWritCheck->isChecked() )
			perms |= ( ( gWritCheck->checkState() == Qt::Checked ) ? QFile::WriteGroup : ( QFile::permissions( path ) & QFile::WriteGroup ) );

		if ( gExecCheck->isChecked() )
			perms |= ( ( gExecCheck->checkState() == Qt::Checked ) ? QFile::ExeGroup : ( QFile::permissions( path ) & QFile::ExeGroup ) );

		if ( oReadCheck->isChecked() )
			perms |= ( ( oReadCheck->checkState() == Qt::Checked ) ? QFile::ReadOther : ( QFile::permissions( path ) & QFile::ReadOther ) );

		if ( oWritCheck->isChecked() )
			perms |= ( ( oWritCheck->checkState() == Qt::Checked ) ? QFile::WriteOther : ( QFile::permissions( path ) & QFile::WriteOther ) );

		if ( oExecCheck->isChecked() )
			perms |= ( ( oExecCheck->checkState() == Qt::Checked ) ? QFile::ExeOther : ( QFile::permissions( path ) & QFile::ExeOther ) );

		if ( not path.endsWith( "/" ) and isDir( path ) )
			path += "/";

		if ( isDir( path ) and carryCheck->isChecked() ) {

			/* Recursively apply these permissions to all the children in this directory */
			applyTo( path.toLocal8Bit(), perms );
		}

		else {
			QFile::setPermissions( path, perms );
		}
	}
};

void NBPermissionsWidget::applyTo( const char *node, QFile::Permissions perms ) {

	DIR* d_fh;
	struct dirent* entry;
	char nodeName[ 10240 ];

	while( ( d_fh = opendir( node ) ) == NULL ) {
		fprintf( stderr, "Couldn't open directory: %s\n", node );
		return;
	}

	strncpy( nodeName, node, 4095 );

	while( ( entry = readdir( d_fh ) ) != NULL ) {

		// Don't include . and ..
		if ( strncmp( entry->d_name, "..", 2 ) != 0 && strncmp( entry->d_name, ".", 1 ) != 0 ) {

			// If it's a directory print it's name and recurse into it
			if ( entry->d_type == DT_DIR ) {

				// Prepend the current directory and recurse
				strncat( nodeName, entry->d_name, 10239 );
				strncat( nodeName, "/", 10239 );

				// Set read permissions
				QFile::setPermissions( nodeName, perms );

				// Recurse
				applyTo( nodeName, perms );
				memset( nodeName + strlen( nodeName ) - strlen( entry->d_name ) - 1, 0, strlen( entry->d_name ) + 1 );
			}
			else {

				// Apply read permissions to this file
				QFile::setPermissions( nodeName, perms );
			}
			qApp->processEvents();
		}
	}

	closedir( d_fh );
};

NBPropertiesDialog::NBPropertiesDialog( QStringList paths, PropertiesTab tab ) : NBDialog( NBDialog::Close ) {

	pathsList << paths;

	setFixedSize( QSize( 530, 400 ) );

	propsB = new NBPropertiesBase( paths );
	propsW = new NBPropertiesWidget( paths );
	permsW = new NBPermissionsWidget( paths );

	tabs = new QTabBar();
	tabs->addTab( QIcon( ":/icons/props.png" ), QString( "P&roperties" ) );
	tabs->addTab( QIcon( ":/icons/perms.png" ), QString( "P&ermissions" ) );

	stack = new QStackedWidget();
	stack->addWidget( propsW );
	stack->addWidget( permsW );

	switchToTab( tab );

	QVBoxLayout *lyt = new QVBoxLayout();
	lyt->setContentsMargins( QMargins() );

	lyt->addWidget( tabs );
	lyt->addWidget( propsB );
	lyt->addWidget( Separator::horizontal() );
	lyt->addWidget( stack );
	setLayout( lyt );

	setDialogTitle( tr( "NewBreeze - Properties" ) );
	setDialogIcon( QIcon( ";/icons/newbreeze2.png" ) );

	setWindowModality( Qt::NonModal );

	connect( tabs, SIGNAL( currentChanged( int ) ), this, SLOT( switchToTab( int ) ) );
	connect( stack, SIGNAL( currentChanged( int ) ), this, SLOT( switchToTab( int ) ) );

	if ( tab == NBPropertiesDialog::Properties )
		connect( propsB, SIGNAL( setDirIcon() ), this, SLOT( setDirIcon() ) );
};

void NBPropertiesDialog::switchToTab( int newTab ) {

	tabs->setCurrentIndex( newTab );
	stack->setCurrentIndex( newTab );
	setFixedSize( 530, 370 );

	if ( newTab )
		disconnect( propsB, SIGNAL( setDirIcon() ), this, SLOT( setDirIcon() ) );

	else
		connect( propsB, SIGNAL( setDirIcon() ), this, SLOT( setDirIcon() ) );
};

void NBPropertiesDialog::setDirIcon() {

	QString iconName = NBFileDialog::getExistingFileName(
			QString( ":/icons/newbreeze2.png" ),
			tr( "NewBreeze - Select Icon" ),
			QDir::homePath(),
			QStringList(
				QStringList() << "Images ( *.png *.bmp *.jpg *.svg *.gif *.ppm )"
			),
			QString( "PNG (*.png)" )
	);

	if ( not iconName.isEmpty() ) {
		QFile file( pathsList.at( 0 ) + "/.directory" );
		if ( not file.open( QFile::WriteOnly ) ) {
			QMessageBox::warning( this, "Error", "Unable to set icon" );
			return;
		}
		file.write( QString( "[Desktop Entry]\nIcon=%1" ).arg( iconName ).toUtf8() );
		file.flush();
		file.close();

		NBClickLabel *label = qobject_cast<NBClickLabel*>( sender() );
		label->setPixmap( QPixmap( iconName ).scaled( 48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	}
};
