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
	setWindowProperties();
};

NBPropertiesDialog::~NBPropertiesDialog() {

	terminate = true;
	thread.waitForFinished();
};

void NBPropertiesDialog::createGUI() {

	files = 0;
	folders = 0;
	totalSize = 0;

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins( QMargins() );

	QPushButton *okBtn = new QPushButton( QIcon( ":/icons/ok.png" ), "&Ok" );
	okBtn->setObjectName( tr( "okBtn" ) );

	connect( okBtn, SIGNAL( clicked() ), this, SLOT( accept() ) );

	// Show hide widget
	fileSHWidget = getGeneralInfoWidget();
	permSHWidget = getPermissionsWidget();
	drivSHWidget = getDeviceInfoWidget();

	connect( fileSHWidget, SIGNAL( visibilityChanged() ), this, SLOT( resizeDialog() ) );
	connect( permSHWidget, SIGNAL( visibilityChanged() ), this, SLOT( resizeDialog() ) );
	connect( drivSHWidget, SIGNAL( visibilityChanged() ), this, SLOT( resizeDialog() ) );

	// Main layout
	layout->addWidget( fileSHWidget );
	layout->addWidget( permSHWidget );
	layout->addWidget( drivSHWidget );
	layout->addWidget( okBtn );

	setLayout( layout );

	setFixedWidth( 530 );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 530 ) / 2 );
	int vpos = ( int )( ( dw.height() - 540 ) / 2 );
	setGeometry( hpos, vpos, 530, 540 );

	connect( this, SIGNAL( updateSignal() ), this, SLOT( update() ) );

	this->setAttribute( Qt::WA_DeleteOnClose, 1 );
	thread = QtConcurrent::run( this, &NBPropertiesDialog::folderProperties, pathsList );

	okBtn->setFocus();
};

NBShowHideWidget* NBPropertiesDialog::getGeneralInfoWidget() {

	QFileInfo info( pathsList.at( 0 ) );

	// Name and Icon
	iconLabel = new NBClickLabel( QPixmap() );
	iconLabel->setFixedSize( QSize( 48, 48 ) );
	connect( iconLabel, SIGNAL( clicked() ), this, SLOT( setDirIcon() ) );
	QLabel *nameLabel = new QLabel();

	// Location
	QLabel *loc1Label = new QLabel( "Location:" );
	QLabel *loc2Label = new QLabel( QFileInfo( pathsList.at( 0 ) ).absolutePath() );

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
		QString iconName = NBIconProvider::icon( info.absoluteFilePath() );
		iconLabel->setPixmap( QIcon::fromTheme( iconName, QIcon( iconName ) ).pixmap( 48, 48 ) );
		if ( info.isDir() ) {
			iconLabel->setClickable( true );
			type2Label->setText( "inode/directory" );
		}
		else {
			iconLabel->setClickable( false );
			type2Label->setText( getMimeTypeAlt( pathsList.at( 0 ) ) );
		}
		nameLabel->setText( info.fileName() );
	}

	else {
		qint64 sFiles = 0, sFolders = 0;

		foreach( QString path, pathsList ) {
			if ( QFileInfo( path ).isDir() )
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
			iconLabel->setPixmap( QIcon( ":/icons/files.png" ).pixmap( QSize( 48, 48 ) ) );
			nameLabel->setText( filesText );
			type2Label->setText( "Files" );
		}

		else {
			iconLabel->setPixmap( QIcon( ":/icons/folders.png" ).pixmap( QSize( 48, 48 ) ) );
			nameLabel->setText( foldersText );
			type2Label->setText( "Folders" );
		}
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

	if ( ( pathsList.count() > 1 ) or ( QFileInfo( pathsList.at( 0 ) ).isDir() ) ) {
		fileLyt->addWidget( cont1Label, 5, 0, Qt::AlignLeft );
		fileLyt->addWidget( cont2Label, 5, 1, Qt::AlignRight );
	}

	if ( pathsList.count() == 1 ) {
		fileLyt->addWidget( time11Label, 6, 0, Qt::AlignLeft );
		fileLyt->addWidget( time12Label, 6, 1, Qt::AlignRight );

		fileLyt->addWidget( time21Label, 7, 0, Qt::AlignLeft );
		fileLyt->addWidget( time22Label, 7, 1, Qt::AlignRight );
	}

	QWidget *fileFrame = new QWidget( this );
	fileFrame->setObjectName( "infoFrame" );
	fileFrame->setLayout( fileLyt );
	fileFrame->setFixedHeight( 180 );

	NBShowHideWidget *fileSHWidget = new NBShowHideWidget( "General Info", fileFrame );

	return fileSHWidget;
};

NBShowHideWidget* NBPropertiesDialog::getPermissionsWidget() {

	QGridLayout *permsLyt = new QGridLayout();

	QStringList permsList;
	if ( pathsList.count() > 1 )
		permsList << "Varying (No change)" << "Forbidden" << "Read Only" << "Read and Write";

	else
		permsList << "Forbidden" << "Read Only" << "Read and Write";

	ownerCombo = new QComboBox();
	groupCombo = new QComboBox();
	otherCombo = new QComboBox();

	execCheck = new QCheckBox( "&Executable" );

	ownerCombo->addItems( permsList );
	groupCombo->addItems( permsList );
	otherCombo->addItems( permsList );

	readPermissions();

	connect( ownerCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( combosChanged() ) );
	connect( groupCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( combosChanged() ) );
	connect( otherCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( combosChanged() ) );
	connect( execCheck, SIGNAL( stateChanged( int ) ), this, SLOT( combosChanged() ) );

	permsLyt->addWidget( new QLabel( tr( "Owner" ) ), 0, 0 );
	permsLyt->addWidget( new QLabel( tr( "Group" ) ), 1, 0 );
	permsLyt->addWidget( new QLabel( tr( "Other" ) ), 2, 0 );

	permsLyt->addWidget( ownerCombo, 0, 1 );
	permsLyt->addWidget( groupCombo, 1, 1 );
	permsLyt->addWidget( otherCombo, 2, 1 );
	permsLyt->addWidget( execCheck, 3, 1 );

	QWidget *permsFrame = new QWidget( this );
	permsFrame->setObjectName( "infoFrame" );
	permsFrame->setLayout( permsLyt );
	permsFrame->setFixedHeight( 120 );

	if ( QFileInfo( pathsList.at( 0 ) ).ownerId() != QFileInfo( QDir::homePath() ).ownerId() )
		permsFrame->setDisabled( 1 );

	NBShowHideWidget *permSHWidget = new NBShowHideWidget( "Permissions", permsFrame );

	return permSHWidget;
};

NBShowHideWidget* NBPropertiesDialog::getDeviceInfoWidget() {

	QWidget *driveFrame = new QWidget( this );
	driveFrame->setObjectName( "infoFrame" );

	driveFrame->setLayout( driveLyt );
	driveFrame->setFixedHeight( 90 );

	NBShowHideWidget *drivSHWidget = new NBShowHideWidget( "Drive Info", driveFrame );

	return drivSHWidget;
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

void NBPropertiesDialog::readPermissions() {

	if ( pathsList.count() > 1 ) {
		ownerCombo->setCurrentIndex( -1 );
		groupCombo->setCurrentIndex( -1 );
		otherCombo->setCurrentIndex( -1 );

		execCheck->setChecked( false );
	}

	else {
		QFileInfo info( pathsList.at( 0 ) );
		if ( info.isDir() )
			execCheck->setDisabled( true );

		else
			execCheck->setChecked( info.isExecutable() );

		// File/Folder Permission
		if ( info.permission( QFile::ReadOwner | QFile::WriteOwner ) ) {
			ownerCombo->setCurrentIndex( 2 );
			if ( info.isDir() and not info.permission( QFile::ExeOwner ) )
				ownerCombo->setCurrentIndex( 0 );
		}

		else if ( info.permission( QFile::ReadOwner ) ) {
			ownerCombo->setCurrentIndex( 1 );
			if ( info.isDir() and not info.permission( QFile::ExeOwner ) )
				ownerCombo->setCurrentIndex( 0 );
		}

		else {
			ownerCombo->setCurrentIndex( 0 );
		}

		if ( info.permission( QFile::ReadGroup | QFile::WriteGroup ) ) {
			groupCombo->setCurrentIndex( 2 );
			if ( info.isDir() and not info.permission( QFile::ExeGroup ) )
				groupCombo->setCurrentIndex( 0 );
		}

		else if ( info.permission( QFile::ReadGroup  ) ) {
			groupCombo->setCurrentIndex( 1 );
			if ( info.isDir() and not info.permission( QFile::ExeGroup ) )
				groupCombo->setCurrentIndex( 0 );
		}

		else {
			groupCombo->setCurrentIndex( 0 );
		}

		if ( info.permission( QFile::ReadOther | QFile::WriteOther | QFile::ExeOther ) ) {
			otherCombo->setCurrentIndex( 2 );
			if ( info.isDir() and not info.permission( QFile::ExeOther ) )
				otherCombo->setCurrentIndex( 0 );
		}

		else if ( info.permission( QFile::ReadOther ) ) {
			otherCombo->setCurrentIndex( 1 );
			if ( info.isDir() and not info.permission( QFile::ExeOther ) )
				otherCombo->setCurrentIndex( 0 );
		}

		else {
			otherCombo->setCurrentIndex( 0 );
		}
	}
};

void NBPropertiesDialog::setWindowProperties() {

	setDialogTitle( tr( "Properties" ) );
	setDialogIcon( QIcon( ":/icons/newbreeze2.png" ) );

	if ( Settings->General.Style == "Transparent" )
		setAttribute( Qt::WA_TranslucentBackground );
	setWindowFlags( Qt::FramelessWindowHint );

	setFixedWidth( 540 );
	setFixedHeight( 570 );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 540 ) / 2 );
	int vpos = ( int )( ( dw.height() - 570 ) / 2 );
	move( hpos, vpos );
};

void NBPropertiesDialog::resizeDialog() {

	int size = 180;
	if ( fileSHWidget->isWidgetVisible() ) {
		size += 180;
	}
	if ( permSHWidget->isWidgetVisible() ) {
		size += 120;
	}
	if ( drivSHWidget->isWidgetVisible() ) {
		size += 90;
	}

	setFixedHeight( size );
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

void NBPropertiesDialog::accept() {

	terminate = true;
	thread.waitForFinished();

	this->done( 1 );
};

void NBPropertiesDialog::checkboxesChanged() {

	int u = ownerRead->isChecked() * 4 + ownerWrite->isChecked() * 2 + ownerExec->isChecked();
	int g = groupRead->isChecked() * 4 + groupWrite->isChecked() * 2 + groupExec->isChecked();
	int o = otherRead->isChecked() * 4 + otherWrite->isChecked() * 2 + otherExec->isChecked();

	permissionsNumeric->setText( QString( "%1%2%3" ).arg( u ).arg( g ).arg( o ) );
};

void NBPropertiesDialog::combosChanged() {

	foreach( QString path, pathsList ) {
		int u = 0, g = 0, o = 0;

		QFileInfo info( path );

		if ( ownerCombo->currentIndex() == 0 ) {
			u = 0;
		}

		else if ( ownerCombo->currentIndex() == 1 ) {
			u = 4;
			if ( info.isDir() )
				u++;
		}

		else if ( ownerCombo->currentIndex() == 2 ) {
			u = 6;
			if ( info.isDir() )
				u++;
		}

		if ( groupCombo->currentIndex() == 0 ) {
			g = 0;
		}

		else if ( groupCombo->currentIndex() == 1 ) {
			g = 4;
			if ( info.isDir() )
				g++;
		}

		else if ( groupCombo->currentIndex() == 2 ) {
			g = 6;
			if ( info.isDir() )
				g++;
		}

		if ( otherCombo->currentIndex() == 0 ) {
			o = 0;
		}

		else if ( otherCombo->currentIndex() == 1 ) {
			o = 4;
			if ( info.isDir() )
				o++;
		}

		else if ( otherCombo->currentIndex() == 2 ) {
			o = 6;
			if ( info.isDir() )
				o++;
		}

		if ( info.isFile() and execCheck->isChecked() ) {
			u++;
			g++;
			o++;
		}

		QString permString = QString( "%1%2%3" ).arg( u ).arg( g ).arg( o );
		chmod( path.toLocal8Bit(), permString.toInt( 0, 8 ) );
	}
};

void NBPropertiesDialog::numericChanged( QString text ) {

	if( text.count() != 3 ) return;

	int owner = QString( text.at( 0 ) ).toInt();
	ownerRead->setChecked( owner / 4 );
	ownerWrite->setChecked( ( owner - owner / 4 * 4- owner % 2 ) );
	ownerExec->setChecked( owner % 2 );

	int group = QString( text.at( 1 ) ).toInt();
	groupRead->setChecked( group / 4 );
	groupWrite->setChecked( ( group - group / 4 * 4 - group % 2 ) );
	groupExec->setChecked( group % 2 );

	int other = QString( text.at( 2 ) ).toInt();
	otherRead->setChecked( other / 4 );
	otherWrite->setChecked( ( other - other / 4 * 4 - other % 2 ) );
	otherExec->setChecked( other % 2 );

	chmod( pathsList.at( 0 ).toLocal8Bit(), permissionsNumeric->text().toInt( 0, 8 ) );
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
