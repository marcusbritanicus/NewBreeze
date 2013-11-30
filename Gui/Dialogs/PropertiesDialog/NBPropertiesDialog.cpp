/*
	*
	* NBPropertiesDialog.cpp - Properties of the selected object
	*
*/

#include <NBPropertiesDialog.hpp>

NBPropertiesDialog::NBPropertiesDialog( QStringList paths ) : NBDialog() {

	pathsList << paths;
	terminate = false;

	createGUI();
	setDialogProperties();
};

NBPropertiesDialog::~NBPropertiesDialog() {

	terminate = true;
	thread.waitForFinished();
};

void NBPropertiesDialog::createGUI() {

	files = 0;
	folders = 0;
	totalSize = 0;

	QFileInfo info( pathsList.at( 0 ) );

	// Name and Icon
	iconLabel = new NBClickLabel( QPixmap() );
	iconLabel->setFixedSize( QSize( 48, 48 ) );
	connect( iconLabel, SIGNAL( clicked() ), this, SLOT( setDirIcon() ) );
	QLabel *nameLabel = new QLabel();

	// Location
	QLabel *loc1Label = new QLabel( "Location:" );
	QLabel *loc2Label = new QLabel( dirName( pathsList.at( 0 ) ) );

	// Size
	QLabel *size1Label = new QLabel( "Size" );
	size2Label = new QLabel();

	// Type
	QLabel *type1Label = new QLabel( "Type" );
	QLabel *type2Label = new QLabel();

	// Contents
	QLabel *cont1Label = new QLabel( "Contents" );
	cont2Label = new QLabel();

	// M Time
	QLabel *time11Label = new QLabel( "Modified" );
	QLabel *time12Label = new QLabel( info.lastModified().toString( "ddd, MMM dd, yyyy hh:mm:ss AP" ) );

	// A Time
	QLabel *time21Label = new QLabel( "Accessed" );
	QLabel *time22Label = new QLabel( info.lastRead().toString( "ddd, MMM dd, yyyy hh:mm:ss AP" ) );

	if ( pathsList.count() == 1 ) {
		QString iconName = NBIconProvider::icon( pathsList.at( 0 ) );
		iconLabel->setPixmap( QIcon::fromTheme( iconName, QIcon( iconName ) ).pixmap( 48, 48 ) );
		if ( isDir( pathsList.at( 0 ) ) ) {
			iconLabel->setClickable( true );
			type2Label->setText( "inode/directory" );
			setFixedHeight( 300 );
		}
		else {
			iconLabel->setClickable( false );
			type2Label->setText( getMimeType( pathsList.at( 0 ) ) );
			setFixedHeight( 280 );
		}
		nameLabel->setText( baseName( pathsList.at( 0 ) ) );
	}

	else {
		qint64 sFiles = 0, sFolders = 0;

		foreach( QString path, pathsList ) {
			if ( isDir( path ) )
				sFolders++;

			else
				sFiles++;
		}
		QString filesText = QString( "%1 file%2" ).arg( sFiles ).arg( ( sFiles > 1 ? "s" : "" ) );
		QString foldersText = QString( "%1 folder%2" ).arg( sFolders ).arg( ( sFolders > 1 ? "s" : "" ) );

		if ( sFiles and sFolders ) {
			iconLabel->setPixmap( QIcon( ":/icons/others.png" ).pixmap( QSize( 48, 48 ) ) );
			nameLabel->setText( filesText + ", " + foldersText );
			type2Label->setText( "Files and Folders" );
		}

		else if ( sFiles and not sFolders ) {
			iconLabel->setPixmap( QIcon( ":/icons/documents.png" ).pixmap( QSize( 48, 48 ) ) );
			nameLabel->setText( filesText );
			type2Label->setText( "Files" );
		}

		else {
			iconLabel->setPixmap( QIcon( ":/icons/folders.png" ).pixmap( QSize( 48, 48 ) ) );
			nameLabel->setText( foldersText );
			type2Label->setText( "Folders" );
		}

		setFixedHeight( 260 );
	}

	QGridLayout *fileLyt = new QGridLayout();

	fileLyt->addWidget( iconLabel, 0, 0, Qt::AlignLeft );
	fileLyt->addWidget( nameLabel, 0, 1, Qt::AlignRight );

	fileLyt->addWidget( Separator::horizontal(), 1, 0, 1, 2 );

	fileLyt->addWidget( loc1Label, 2, 0, Qt::AlignLeft );
	fileLyt->addWidget( loc2Label, 2, 1, Qt::AlignRight );

	fileLyt->addWidget( type1Label, 3, 0, Qt::AlignLeft );
	fileLyt->addWidget( type2Label, 3, 1, Qt::AlignRight );

	fileLyt->addWidget( size1Label, 4, 0, Qt::AlignLeft );
	fileLyt->addWidget( size2Label, 4, 1, Qt::AlignRight );

	if ( ( pathsList.count() > 1 ) or ( isDir( pathsList.at( 0 ) ) ) ) {
		fileLyt->addWidget( cont1Label, 5, 0, Qt::AlignLeft );
		fileLyt->addWidget( cont2Label, 5, 1, Qt::AlignRight );
	}

	if ( pathsList.count() == 1 ) {
		fileLyt->addWidget( time11Label, 6, 0, Qt::AlignLeft );
		fileLyt->addWidget( time12Label, 6, 1, Qt::AlignRight );

		fileLyt->addWidget( time21Label, 7, 0, Qt::AlignLeft );
		fileLyt->addWidget( time22Label, 7, 1, Qt::AlignRight );
	}

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
	thread = QtConcurrent::run( this, &NBPropertiesDialog::folderProperties, pathsList );
};

void NBPropertiesDialog::folderProperties( QStringList paths ) {

	Q_UNUSED( paths );

	foreach( QString path, pathsList ) {
		if( terminate )
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

void NBPropertiesDialog::recurseProperties( QString path ) {

	QDirIterator it( path, QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden, QDirIterator::Subdirectories );
	while ( it.hasNext() ) {
		if( terminate )
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

void NBPropertiesDialog::setDialogProperties() {

	setDialogTitle( tr( "Properties" ) );
	setDialogIcon( QIcon( ":/icons/newbreeze.png" ) );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	if ( not Settings->General.NativeTitleBar )
		setWindowFlags( Qt::FramelessWindowHint | Qt::Popup );

	else
		setWindowFlags( Qt::Popup );

	setFixedWidth( 530 );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 530 ) / 2 );
	int vpos = ( int )( ( dw.height() - 300 ) / 2 );
	move( hpos, vpos );

	this->setAttribute( Qt::WA_DeleteOnClose, 1 );
};

void NBPropertiesDialog::update() {

	size2Label->setText( formatSize( totalSize ) );

	if( files and folders ) {
		if ( ( files == 1 ) and ( folders == 1 ) )
			cont2Label->setText( QString( tr( "%1 file, %2 sub-folder" ) ).arg( files ).arg( folders ) );

		else if ( ( files > 1 ) and ( folders == 1 ) )
			cont2Label->setText( QString( tr( "%1 files, %2 sub-folder" ) ).arg( files ).arg( folders ) );

		else if ( ( files == 1 ) and ( folders > 1 ) )
			cont2Label->setText( QString( tr( "%1 files, %2 sub-folders" ) ).arg( files ).arg( folders ) );

		else
			cont2Label->setText( QString( tr( "%1 files, %2 sub-folders" ) ).arg( files ).arg( folders ) );
	}

	else if ( files and !folders ) {
		if ( files == 1 )
			cont2Label->setText( QString( tr( "%1 file" ) ).arg( files ) );

		else
			cont2Label->setText( QString( tr( "%1 files" ) ).arg( files ) );
	}

	else {
		if ( not folders )
			cont2Label->setText( "Empty" );

		else if ( folders == 1 )
			cont2Label->setText( "1 sub-folder" );

		else
			cont2Label->setText( QString( tr( "%1 sub-folders" ) ).arg( folders ) );
	}
};

void NBPropertiesDialog::setDirIcon() {

	QString iconName = NBFileDialog::getExistingFileName(
			QString( ":/icons/newbreeze.png" ),
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

NBPermissionsDialog::NBPermissionsDialog( QStringList paths ) : NBDialog() {

	pathsList << paths;

	createGUI();
	setDialogProperties();
};

void NBPermissionsDialog::createGUI() {

	// Name and Icon
	iconLabel = new NBClickLabel( QPixmap() );
	iconLabel->setFixedSize( QSize( 48, 48 ) );
	QLabel *nameLabel = new QLabel();
	nameLabel->setFixedHeight( 48 );

	if ( pathsList.count() == 1 ) {
		QString iconName = NBIconProvider::icon( pathsList.at( 0 ) );
		iconLabel->setPixmap( QIcon::fromTheme( iconName, QIcon( iconName ) ).pixmap( 48, 48 ) );
		nameLabel->setText( baseName( pathsList.at( 0 ) ) );
	}

	else {
		qint64 sFiles = 0, sFolders = 0;

		foreach( QString path, pathsList ) {
			if ( isDir( path ) )
				sFolders++;

			else
				sFiles++;
		}
		QString filesText = QString( "%1 file%2" ).arg( sFiles ).arg( ( sFiles > 1 ? "s" : "" ) );
		QString foldersText = QString( "%1 folder%2" ).arg( sFolders ).arg( ( sFolders > 1 ? "s" : "" ) );

		if ( sFiles and sFolders ) {
			iconLabel->setPixmap( QIcon( ":/icons/others.png" ).pixmap( QSize( 48, 48 ) ) );
			nameLabel->setText( filesText + ", " + foldersText );
		}

		else if ( sFiles and not sFolders ) {
			iconLabel->setPixmap( QIcon( ":/icons/files.png" ).pixmap( QSize( 48, 48 ) ) );
			nameLabel->setText( filesText );
		}

		else {
			iconLabel->setPixmap( QIcon( ":/icons/folders.png" ).pixmap( QSize( 48, 48 ) ) );
			nameLabel->setText( foldersText );
		}
	}

	QHBoxLayout *iconLyt = new QHBoxLayout();
	iconLyt->addWidget( iconLabel );
	iconLyt->addStretch( 0 );
	iconLyt->addWidget( nameLabel );

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
	smartExecCheck->setToolTip( "Set all ELF files and shell scripts and files in 'bin' folders to executable" );

	carryCheck = new QCheckBox( "Appl&y changes to all subfolders and their contents" );

	applyBtn = new QPushButton( "&Apply" );
	applyBtn->setObjectName( tr( "okBtn" ) );
	connect( applyBtn, SIGNAL( clicked() ), this, SLOT( applyPermissions() ) );

	cancelBtn = new QPushButton( "&Cancel" );
	cancelBtn->setObjectName( tr( "cancelBtn" ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	readPermissions();

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addStretch();
	btnLyt->addWidget( applyBtn );
	btnLyt->addWidget( cancelBtn );
	btnLyt->addStretch();

	QGridLayout *permsLyt = new QGridLayout();
	permsLyt->setContentsMargins( QMargins() );

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

	permsLyt->addWidget( smartExecCheck, 4, 0, 4, -1 );
	permsLyt->addWidget( carryCheck, 5, 0, 5, -1 );

	QWidget *permsFrame = new QWidget( this );
	permsFrame->setLayout( permsLyt );

	if ( QFileInfo( pathsList.at( 0 ) ).ownerId() != QFileInfo( QDir::homePath() ).ownerId() )
		permsFrame->setDisabled( 1 );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->addLayout( iconLyt );
	baseLyt->addWidget( Separator::horizontal() );
	baseLyt->addWidget( permsFrame );
	baseLyt->addWidget( Separator::horizontal() );
	baseLyt->addLayout( btnLyt );

	setLayout( baseLyt );
};

void NBPermissionsDialog::readPermissions() {

	short UR = 0, UW = 0, UE = 0, GR = 0, GW = 0, GE = 0, OR = 0, OW = 0, OE = 0;
	foreach( QString path, pathsList ) {
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

void NBPermissionsDialog::setDialogProperties() {

	setDialogTitle( tr( "Permissions" ) );
	setDialogIcon( QIcon( ":/icons/newbreeze.png" ) );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	setFixedSize( 530, 300 );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 530 ) / 2 );
	int vpos = ( int )( ( dw.height() - 300 ) / 2 );
	move( hpos, vpos );

	this->setAttribute( Qt::WA_DeleteOnClose, 1 );
};

void NBPermissionsDialog::applyPermissions() {

	QFile::Permissions perms = NULL;

	if ( uReadCheck->isChecked() ) perms |= QFile::ReadOwner;
	if ( uWritCheck->isChecked() ) perms |= QFile::WriteOwner;
	if ( uExecCheck->isChecked() ) perms |= QFile::ExeOwner;

	if ( gReadCheck->isChecked() ) perms |= QFile::ReadGroup;
	if ( gWritCheck->isChecked() ) perms |= QFile::WriteGroup;
	if ( gExecCheck->isChecked() ) perms |= QFile::ExeGroup;

	if ( oReadCheck->isChecked() ) perms |= QFile::ReadOther;
	if ( oWritCheck->isChecked() ) perms |= QFile::WriteOther;
	if ( oExecCheck->isChecked() ) perms |= QFile::ExeOther;

	foreach( QString path, pathsList ) {
		if ( not path.endsWith( "/" ) and isDir( path ) )
			path += "/";

		if ( carryCheck->isChecked() )
			applyTo( path.toLocal8Bit(), perms );

		else
			QFile::setPermissions( path, perms );
	}

	close();
};

void NBPermissionsDialog::applyTo( const char *node, QFile::Permissions perms ) {

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
