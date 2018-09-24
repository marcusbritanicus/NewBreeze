/*
	*
	* NBPropertiesDialog.cpp - Properties of the selected object
	*
*/

#include "NBPermsWidget.hpp"

static bool canBeExec( QString path ) {

	struct stat statbuf;
	if ( stat( path.toLocal8Bit().data(), &statbuf ) != 0 )
		return false;

	QMimeType m = mimeDb.mimeTypeForFile( path );
	if ( m.name() == "application/x-executable" )
		return true;

	else if ( m.name() == "application/x-sharedlib" )
		return true;

	else if ( m.allAncestors().contains( "application/x-executable" ) )
		return true;

	else
		return false;

	return false;
};

NBPermissionsWidget::NBPermissionsWidget( QStringList paths, QWidget *parent ) : QWidget( parent ) {

	pathsList << paths;

	for( int i = 0; i < pathsList.count(); i++ ) {
		if ( pathsList[ i ].endsWith( "/" ) )
			pathsList[ i ] = QString( pathsList[ i ] ).left( pathsList[ i ].count() - 1 );
	}

	createGUI();
	setupConnections();

	readPermissions();
};

void NBPermissionsWidget::createGUI() {

	nameLbl = new QLabel();
	nameLbl->setWordWrap( false );
	nameLbl->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

	pathLbl = new QLabel();
	pathLbl->setWordWrap( false );
	pathLbl->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

	QWidget *sep = new QWidget( this );
	sep->setFixedHeight( 1 );
	sep->setStyleSheet( "background-color: gray;" );

	uNameLbl = new QLabel();
	gNameLbl = new QLabel();

	urCB = new QCheckBox( "Read", this );
	uwCB = new QCheckBox( "Write", this );
	uxCB = new QCheckBox( "Execute", this );
	grCB = new QCheckBox( "Read", this );
	gwCB = new QCheckBox( "Write", this );
	gxCB = new QCheckBox( "Execute", this );
	orCB = new QCheckBox( "Read", this );
	owCB = new QCheckBox( "Write", this );
	oxCB = new QCheckBox( "Execute", this );

	recursiveBtn = new QPushButton( "Apply recursively", this );

	QGridLayout *gridLyt = new QGridLayout();
	gridLyt->setContentsMargins( 10, 5, 10, 5 );

	gridLyt->addWidget( new QLabel( "User:" ), 0, 0 );
	gridLyt->addWidget( uNameLbl, 0, 1 );
	gridLyt->addWidget( new QLabel( "Group:" ), 1, 0 );
	gridLyt->addWidget( gNameLbl, 1, 1 );

	gridLyt->addWidget( new QLabel( "  " ), 2, 0 );

	gridLyt->addWidget( new QLabel( "Owner" ), 3, 0 );
	gridLyt->addWidget( urCB, 3, 1 );
	gridLyt->addWidget( uwCB, 3, 2 );
	gridLyt->addWidget( uxCB, 3, 3 );

	gridLyt->addWidget( new QLabel( "Group" ), 4, 0 );
	gridLyt->addWidget( grCB, 4, 1 );
	gridLyt->addWidget( gwCB, 4, 2 );
	gridLyt->addWidget( gxCB, 4, 3 );

	gridLyt->addWidget( new QLabel( "Other" ), 5, 0 );
	gridLyt->addWidget( orCB, 5, 1 );
	gridLyt->addWidget( owCB, 5, 2 );
	gridLyt->addWidget( oxCB, 5, 3 );

	gridLyt->addWidget( new QLabel( "  " ), 6, 0 );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addStretch();
	btnLyt->addWidget( recursiveBtn );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addWidget( nameLbl );
	baseLyt->addWidget( pathLbl );
	baseLyt->addWidget( sep );
	baseLyt->addLayout( gridLyt );
	baseLyt->addLayout( btnLyt );
	baseLyt->addStretch();

	setLayout( baseLyt );
};

void NBPermissionsWidget::setupConnections() {

	connect( urCB, SIGNAL( toggled( bool ) ), SLOT( addUR() ) );
	connect( uwCB, SIGNAL( toggled( bool ) ), SLOT( addUW() ) );
	connect( uxCB, SIGNAL( toggled( bool ) ), SLOT( addUX() ) );
	connect( grCB, SIGNAL( toggled( bool ) ), SLOT( addGR() ) );
	connect( gwCB, SIGNAL( toggled( bool ) ), SLOT( addGW() ) );
	connect( gxCB, SIGNAL( toggled( bool ) ), SLOT( addGX() ) );
	connect( orCB, SIGNAL( toggled( bool ) ), SLOT( addOR() ) );
	connect( owCB, SIGNAL( toggled( bool ) ), SLOT( addOW() ) );
	connect( oxCB, SIGNAL( toggled( bool ) ), SLOT( addOX() ) );

	connect( recursiveBtn, SIGNAL( clicked() ), this, SLOT( applyRecursive() ) );
};

void NBPermissionsWidget::readPermissions() {

	QString uname = QFileInfo( pathsList.at( 0 ) ).owner();
	QString gname = QFileInfo( pathsList.at( 0 ) ).group();

	short UR = 0, UW = 0, UE = 0, GR = 0, GW = 0, GE = 0, OR = 0, OW = 0, OE = 0;
	int folders = 0, files = 0;
	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = QFile::permissions( path );

		UR += ( ( perms & QFile::ReadOwner ) ? 1 : -1 );
		UW += ( ( perms & QFile::WriteOwner ) ? 1 : -1 );
		UE += ( ( perms & QFile::ExeOwner ) ? 1 : -1 );
		GR += ( ( perms & QFile::ReadGroup ) ? 1 : -1 );
		GW += ( ( perms & QFile::WriteGroup ) ? 1 : -1 );
		GE += ( ( perms & QFile::ExeGroup ) ? 1 : -1 );
		OR += ( ( perms & QFile::ReadOther ) ? 1 : -1 );
		OW += ( ( perms & QFile::WriteOther ) ? 1 : -1 );
		OE += ( ( perms & QFile::ExeOther ) ? 1 : -1 );

		if ( path.endsWith( "/" ) )
			path.chop( 1 );

		QFileInfo info( path );

		uname = ( ( uname != info.owner() ) ? "Various" : info.owner() );
		gname = ( ( gname != info.group() ) ? "Various" : info.group() );

		if ( info.isDir() )
			folders++;

		else
			files++;
	}

	uNameLbl->setText( uname );
	gNameLbl->setText( gname );

	if ( pathsList.count() == 1 )
		nameLbl->setText( "<b>" + QFileInfo( pathsList.at( 0 ) ).fileName() + "</b>" );

	else
		nameLbl->setText( QString( "<b>%1 folder(s) and %2 file(s) in %3<b>" ).arg( folders ).arg( files ).arg( QFileInfo( pathsList.at( 0 ) ).path() ) );

	pathLbl->setText( dirName( pathsList.at( 0 ) ) );

	if ( -UR == pathsList.count() )
		urCB->setCheckState( Qt::Unchecked );

	else if ( UR != pathsList.count() )
		urCB->setCheckState( Qt::PartiallyChecked );

	else
		urCB->setCheckState( Qt::Checked );

	if ( -UW == pathsList.count() )
		uwCB->setCheckState( Qt::Unchecked );

	else if ( UW != pathsList.count() )
		uwCB->setCheckState( Qt::PartiallyChecked );

	else
		uwCB->setCheckState( Qt::Checked );

	if ( -UE == pathsList.count() )
		uxCB->setCheckState( Qt::Unchecked );

	else if ( UE != pathsList.count() )
		uxCB->setCheckState( Qt::PartiallyChecked );

	else
		uxCB->setCheckState( Qt::Checked );

	if ( -GR == pathsList.count() )
		grCB->setCheckState( Qt::Unchecked );

	else  if ( GR != pathsList.count() )
		grCB->setCheckState( Qt::PartiallyChecked );

	else
		grCB->setCheckState( Qt::Checked );

	if ( -GW == pathsList.count() )
		gwCB->setCheckState( Qt::Unchecked );

	else if ( GW != pathsList.count() )
		gwCB->setCheckState( Qt::PartiallyChecked );

	else
		gwCB->setCheckState( Qt::Checked );

	if ( -GE == pathsList.count() )
		gxCB->setCheckState( Qt::Unchecked );

	else if ( GE != pathsList.count() )
		gxCB->setCheckState( Qt::PartiallyChecked );

	else
		gxCB->setCheckState( Qt::Checked );

	if ( -OR == pathsList.count() )
		orCB->setCheckState( Qt::Unchecked );

	else if ( OR != pathsList.count() )
		orCB->setCheckState( Qt::PartiallyChecked );

	else
		orCB->setCheckState( Qt::Checked );

	if ( -OW == pathsList.count() )
		owCB->setCheckState( Qt::Unchecked );

	else if ( OW != pathsList.count() )
		owCB->setCheckState( Qt::PartiallyChecked );

	else
		owCB->setCheckState( Qt::Checked );

	if ( -OE == pathsList.count() )
		oxCB->setCheckState( Qt::Unchecked );

	else if ( OE != pathsList.count() )
		oxCB->setCheckState( Qt::PartiallyChecked );

	else
		oxCB->setCheckState( Qt::Checked );
};

void NBPermissionsWidget::addUR() {

	bool checked = urCB->checkState();
	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = QFileInfo( path ).permissions();
		perms = ( checked ? perms | QFile::ReadUser : perms ^ QFile::ReadUser );
		perms = ( checked ? perms | QFile::ReadOwner : perms ^ QFile::ReadOwner );

		QFile( path ).setPermissions( perms );
	}
};

void NBPermissionsWidget::addUW() {

	bool checked = uwCB->checkState();
	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = QFileInfo( path ).permissions();
		perms = ( checked ? perms | QFile::WriteUser : perms ^ QFile::WriteUser );
		perms = ( checked ? perms | QFile::WriteOwner : perms ^ QFile::WriteOwner );

		QFile( path ).setPermissions( perms );
	}
};

void NBPermissionsWidget::addUX() {

	bool checked = uxCB->checkState();
	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = QFileInfo( path ).permissions();
		perms = ( checked ? perms | QFile::ExeUser : perms ^ QFile::ExeUser );
		perms = ( checked ? perms | QFile::ExeOwner : perms ^ QFile::ExeOwner );

		/* Smart Executable */
		if ( isDir( path ) and urCB->isChecked() and not checked )
			perms = perms | QFile::ExeUser | QFile::ExeOwner;

		QFile( path ).setPermissions( perms );
	}
};

void NBPermissionsWidget::addGR() {

	bool checked = grCB->checkState();
	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = QFileInfo( path ).permissions();
		perms = ( checked ? perms | QFile::ReadGroup : perms ^ QFile::ReadGroup );

		QFile( path ).setPermissions( perms );
	}
};

void NBPermissionsWidget::addGW() {

	bool checked = gwCB->checkState();
	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = QFileInfo( path ).permissions();
		perms = ( checked ? perms | QFile::WriteGroup : perms ^ QFile::WriteGroup );

		QFile( path ).setPermissions( perms );
	}
};

void NBPermissionsWidget::addGX() {

	bool checked = gxCB->checkState();
	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = QFileInfo( path ).permissions();
		perms = ( checked ? perms | QFile::ExeGroup : perms ^ QFile::ExeGroup );

		/* Smart Executable */
		if ( isDir( path ) and grCB->isChecked() and not checked )
			perms = perms | QFile::ExeGroup;

		QFile( path ).setPermissions( perms );
	}
};

void NBPermissionsWidget::addOR() {

	bool checked = orCB->checkState();
	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = QFileInfo( path ).permissions();
		perms = ( checked ? perms | QFile::ReadOther : perms ^ QFile::ReadOther );

		QFile( path ).setPermissions( perms );
	}
};

void NBPermissionsWidget::addOW() {

	bool checked = owCB->checkState();
	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = QFileInfo( path ).permissions();
		perms = ( checked ? perms | QFile::WriteOther : perms ^ QFile::WriteOther );

		QFile( path ).setPermissions( perms );
	}
};

void NBPermissionsWidget::addOX() {

	bool checked = oxCB->checkState();
	Q_FOREACH( QString path, pathsList ) {
		QFile::Permissions perms = QFileInfo( path ).permissions();
		perms = ( checked ? perms | QFile::ExeOther : perms ^ QFile::ExeOther );

		/* Smart Executable */
		if ( isDir( path ) and orCB->isChecked() and not checked )
			perms = perms | QFile::ExeOther;

		QFile( path ).setPermissions( perms );
	}
};

void NBPermissionsWidget::applyRecursive() {

	// Directory/Executable Permissions
	QFile::Permissions dxPerms = 0;

	if ( urCB->checkState() ) dxPerms |= QFile::ReadOwner | QFile::ReadUser;
	if ( uwCB->checkState() ) dxPerms |= QFile::WriteOwner | QFile::WriteUser;
	if ( uxCB->checkState() ) dxPerms |= QFile::ExeOwner | QFile::ExeUser;

	if ( grCB->checkState() ) dxPerms |= QFile::ReadGroup;
	if ( gwCB->checkState() ) dxPerms |= QFile::WriteGroup;
	if ( gxCB->checkState() ) dxPerms |= QFile::ExeGroup;

	if ( orCB->checkState() ) dxPerms |= QFile::ReadOther;
	if ( owCB->checkState() ) dxPerms |= QFile::WriteOther;
	if ( oxCB->checkState() ) dxPerms |= QFile::ExeOther;

	// File Permissions: No execute permissions
	QFile::Permissions fPerms = 0;
	if ( urCB->checkState() ) fPerms |= QFile::ReadOwner;
	if ( uwCB->checkState() ) fPerms |= QFile::WriteOwner;

	if ( grCB->checkState() ) fPerms |= QFile::ReadGroup;
	if ( gwCB->checkState() ) fPerms |= QFile::WriteGroup;

	if ( orCB->checkState() ) fPerms |= QFile::ReadOther;
	if ( owCB->checkState() ) fPerms |= QFile::WriteOther;

	Q_FOREACH( QString path, pathsList ) {
		QDirIterator it( path, QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden, QDirIterator::Subdirectories );
		while ( it.hasNext() ) {
			it.next();

			// Directory
			if ( it.fileInfo().isDir() ) {
				if ( it.filePath() == path )
					continue;

				QFile( it.filePath() ).setPermissions( dxPerms );
			}

			// Files
			else if ( it.fileInfo().isFile() ) {
				// Python, Shell Scripts, etc
				if ( canBeExec( it.filePath() ) )
					QFile( it.filePath() ).setPermissions( dxPerms );

				/* Normal */
				else
					QFile( it.filePath() ).setPermissions( fPerms );
			}

			else {
				// Do nothing
			}
		}
	}
};
