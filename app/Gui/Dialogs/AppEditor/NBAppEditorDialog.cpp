/*
	*
	* NBAppEditorDialog.cpp - AppEditor class for NewBreeze
	*
*/

#include "NBAppEditorDialog.hpp"

NBMimeTypeSelector::NBMimeTypeSelector( QWidget *parent ) : QTreeWidget( parent ) {

	populateTree();
	setMinimumSize( 450, 300 );
};

QStringList NBMimeTypeSelector::selectedItems() {

	return checked;
};

void NBMimeTypeSelector::setChecked( QString mimeName, bool chkd ) {

	int sep = mimeName.indexOf( "/" );
	QString cls = mimeName.left( sep );
	QString elm = mimeName.right( mimeName.count() - sep - 1 );

	QList<QTreeWidgetItem*> items = findItems( elm, Qt::MatchContains | Qt::MatchRecursive, 0 );
	Q_FOREACH( QTreeWidgetItem *itm, items ) {
		if ( not itm->parent() )
			continue;

		if ( itm->parent()->text( 0 ) == cls ) {
			itm->setCheckState( 0, chkd ? Qt::Checked : Qt::Unchecked );
			return;
		}
	}
};

bool NBMimeTypeSelector::isChecked( QString mimeName ) {

	int sep = mimeName.indexOf( "/" );
	QString cls = mimeName.left( sep );
	QString elm = mimeName.right( mimeName.count() - sep - 1 );

	QList<QTreeWidgetItem*> items = findItems( elm, Qt::MatchContains | Qt::MatchRecursive, 0 );
	Q_FOREACH( QTreeWidgetItem *itm, items ) {
		if ( not itm->parent() )
			continue;

		if ( itm->parent()->text( 0 ) == cls )
			return ( itm->checkState( 0 ) == Qt::Checked ? true : false );
	}

	return false;
};

void NBMimeTypeSelector::populateTree() {

	setHeaderHidden( true );
	mdict.clear();

	// Create our database
	Q_FOREACH( QMimeType mtype, mimeDb.allMimeTypes() ) {
		QStringList l = mtype.name().split( "/" );
		if ( not mdict.contains( l[ 0 ] ) )
			mdict[ l[ 0 ] ] = QStringList();

		mdict[ l[ 0 ] ] << l[ 1 ];
	}

	// Populate our tree
	Q_FOREACH( QString key, mdict.keys() ) {
		QTreeWidgetItem *twip = new QTreeWidgetItem( this, QStringList() << key );
		twip->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
		twip->setCheckState( 0, Qt::Unchecked );
		addTopLevelItem( twip );
		Q_FOREACH( QString elm, mdict[ key ] ) {
			QTreeWidgetItem *twi = new QTreeWidgetItem( twip, QStringList() << elm );
			twi->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
			twi->setCheckState( 0, Qt::Unchecked );
			twip->addChild( twi );
		}

		twip->sortChildren( 0, Qt::AscendingOrder );
	}

	expandAll();

	connect( this, SIGNAL( itemChanged( QTreeWidgetItem*, int ) ), this, SLOT( changeSelection( QTreeWidgetItem*, int ) ) );
};

void NBMimeTypeSelector::changeSelection( QTreeWidgetItem *itm, int col ) {

	if ( not itm->parent() ) {
		for( int c = 0; c < itm->childCount(); c++ )
			itm->child( c )->setCheckState( col, itm->checkState( col ) );
	}

	else {
		QString txt = itm->parent()->text( col ) + "/" + itm->text( col );
		if ( itm->checkState( col ) == Qt::Unchecked )
			checked.removeAll( txt );

		else
			checked << txt;
	}
};

NBAppIconButton::NBAppIconButton( QWidget *parent ) : QToolButton( parent ) {

	mIconFile = QString();
	connect( this, SIGNAL( clicked() ), this, SLOT( selectIcon() ) );
};

void NBAppIconButton::setIcon( QString icn ) {

	mIconFile = icn;
	if ( icn.startsWith( "/" ) )
		QToolButton::setIcon( QIcon( icn ) );

	else
		QToolButton::setIcon( QIcon::fromTheme( icn ) );
};

QString NBAppIconButton::iconPath() {

	return mIconFile;
};

void NBAppIconButton::selectIcon() {

	QString iconFile = QFileDialog::getOpenFileName(
		this,
		"NewBreeze - Select Icon",
		"/usr/share/icons/breeze/apps/48",
		"Icon files (*.png *.jpg *.xpm *.svg *.svgz);;All files (*)"
	);

	if ( iconFile.count() ) {
		mIconFile = iconFile;
		QToolButton::setIcon( QIcon( iconFile ) );
	}
};

NBDesktopFileEditor::NBDesktopFileEditor( QWidget *parent, NewBreeze::DesktopFile desktopFile ) : QDialog( parent ) {

	mApp = desktopFile;

	createGUI();
	updateEntries();
};

void NBDesktopFileEditor::createGUI() {

	nameLE = new QLineEdit( this );
	descrLE = new QLineEdit( this );
	cmtLE = new QLineEdit( this );
	execLE = new QLineEdit( this );

	iconBtn = new NBAppIconButton( this );
	iconBtn->setFixedSize( 64, 64 );
	iconBtn->setIconSize( QSize( 48, 48 ) );

	wrkPthLE = new QLineEdit( this );

	hiddenCB = new QCheckBox( "&Don't show in menu" );
	termCB = new QCheckBox( "&Launch in terminal" );

	QHBoxLayout *cbLyt = new QHBoxLayout();
	cbLyt->setContentsMargins( QMargins() );

	cbLyt->addWidget( hiddenCB );
	cbLyt->addStretch();
	cbLyt->addWidget( termCB );
	cbLyt->addStretch();

	mimesTW = new NBMimeTypeSelector( this );

	saveBtn = new QPushButton( QIcon::fromTheme( "document-save" ), "&Save" );
	connect( saveBtn, SIGNAL( clicked() ), this, SLOT( accept() ) );

	cancelBtn = new QPushButton( QIcon::fromTheme( "dialog-cancel" ), "&Cancel" );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->setContentsMargins( QMargins() );

	btnLyt->addStretch();
	btnLyt->addWidget( saveBtn );
	btnLyt->addWidget( cancelBtn );

	QGridLayout *lyt = new QGridLayout();
	lyt->addWidget( new QLabel( "Name:" ), 0, 0, Qt::AlignRight );
	lyt->addWidget( nameLE, 0, 1 );

	lyt->addWidget( new QLabel( "Description:" ), 1, 0, Qt::AlignRight );
	lyt->addWidget( descrLE, 1, 1 );

	lyt->addWidget( new QLabel( "Comments:" ), 2, 0, Qt::AlignRight );
	lyt->addWidget( cmtLE, 2, 1, 1, 2 );

	lyt->addWidget( new QLabel( "Executable:" ), 3, 0, Qt::AlignRight );
	lyt->addWidget( execLE, 3, 1, 1, 2 );

	lyt->addWidget( iconBtn, 0, 2, 2, 1 );

	lyt->addWidget( new QLabel( "  " ), 4, 1 );

	lyt->addWidget( new QLabel( "WorkPath:" ), 5, 0, Qt::AlignRight );
	lyt->addWidget( wrkPthLE, 5, 1, 1, 2 );

	lyt->addLayout( cbLyt, 6, 0, 1, 3 );

	lyt->addWidget( new QLabel( "MimeTypes:" ), 7, 0 );
	lyt->addWidget( mimesTW, 8, 0, 1, 3 );

	lyt->addLayout( btnLyt, 9, 0, 1, 3 );

	setLayout( lyt );
};

void NBDesktopFileEditor::updateEntries() {

	// nameLE->setText( mApp.value( NewBreeze::DesktopFile::Name ).toString() );
	// descrLE->setText( mApp.value( NewBreeze::DesktopFile::Description ).toString() );
	// cmtLE->setText( mApp.value( NewBreeze::DesktopFile::Comment ).toString() );
	// execLE->setText( mApp.value( NewBreeze::DesktopFile::Exec ).toString() );

	// iconBtn->setIcon( mApp.value( NewBreeze::DesktopFile::Icon ).toString() );

	// wrkPthLE->setText( mApp.value( NewBreeze::DesktopFile::WorkPath ).toString() );
	// hiddenCB->setChecked( mApp.value( NewBreeze::DesktopFile::NoDisplay ).toBool() );
	// termCB->setChecked( mApp.value( NewBreeze::DesktopFile::TerminalMode ).toBool() );

	// Q_FOREACH( QString mime, mApp.value( NewBreeze::DesktopFile::MimeTypes ).toStringList() )
		// mimesTW->setChecked( mime, true );
};

void NBDesktopFileEditor::accept() {

	// mApp.setValue( NewBreeze::DesktopFile::Name, nameLE->text() );
	// mApp.setValue( NewBreeze::DesktopFile::Description, descrLE->text() );
	// mApp.setValue( NewBreeze::DesktopFile::Comment, cmtLE->text() );
	// mApp.setValue( NewBreeze::DesktopFile::Exec, execLE->text() );

	// mApp.setValue( NewBreeze::DesktopFile::Icon, iconBtn->iconPath() );

	// mApp.setValue( NewBreeze::DesktopFile::WorkPath, wrkPthLE->text() );
	// mApp.setValue( NewBreeze::DesktopFile::NoDisplay, hiddenCB->isChecked() );
	// mApp.setValue( NewBreeze::DesktopFile::TerminalMode, termCB->isChecked() );

	// mApp.setValue( NewBreeze::DesktopFile::MimeTypes, mimesTW->selectedItems() );

	// mApp.save();

	QDialog::accept();
};
