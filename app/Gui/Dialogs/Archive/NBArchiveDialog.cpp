/*
	*
	* NBArchiveDialog.cpp - NBArchive Dialog for NewBreeze
	*
*/

#pragma once

#include "NBArchiveDialog.hpp"

NBArchive* NBArchiveDialog::newArchive( QWidget *parent ) {

	NBArchiveDialog *dlg = new NBArchiveDialog( parent );
	if ( dlg->exec() == QDialog::Accepted )
		return dlg->archive();

	else
		return NULL;
};

NBArchiveDialog::NBArchiveDialog( QWidget *parent ) : NBDialog( parent ) {

	createGUI();
};

void NBArchiveDialog::createGUI() {

	setDialogTitle( "NewBreeze - Create an Archive" );
	setDialogIcon( QIcon( ":/icons/newbreeze.png" ) );

	/* Name Lyt */
	QLineEdit *nameLE = new QLineEdit( this );
	nameLE->setPlaceholderText( "Type the archive name here" );
	connect( nameLE, SIGNAL( textChanged( QString ) ), this, SLOT( updateFileName( QString ) ) );

	QLabel *nameLbl = new QLabel( "Archive &Name:" );
	nameLbl->setBuddy( nameLE );

	QHBoxLayout *nLyt = new QHBoxLayout();
	nLyt->addWidget( nameLbl );
	nLyt->addWidget( nameLE );

	/* Location Lyt */
	locationLE = new QLineEdit( this );
	locationLE->setPlaceholderText( "Click browse to select where the archive will be stored" );
	locationLE->setDisabled( true );

	QLabel *locationLbl = new QLabel( "&Location:", this );
	locationLbl->setBuddy( locationLE );

	QToolButton *locationTB = new QToolButton( this );
	locationTB->setIcon( QIcon::fromTheme( "folder" ) );
	connect( locationTB, SIGNAL( clicked() ), this, SLOT( updateDirName() ) );

	QHBoxLayout *lLyt = new QHBoxLayout();
	lLyt->addWidget( locationLbl );
	lLyt->addWidget( locationLE );
	lLyt->addWidget( locationTB );

	/* Format Layout */
	QComboBox *formatsCB = new QComboBox( this );
	formatsCB->addItems( QStringList() << ".tar.xz" << ".tar.gz" << ".tar.bz2" << ".7z" << ".zip" );
	formatsCB->addItems( QStringList() << ".tar" << ".iso" << ".cpio" << ".shar" << ".tar.Z" << ".ar" << ".xar" );
	connect( formatsCB, SIGNAL( currentIndexChanged( QString ) ), this, SLOT( updateFormat( QString ) ) );

	QLabel *formatsLbl = new QLabel( "&Format:", this );
	formatsLbl->setBuddy( formatsCB );

	/* WHAT IS ADVACNED? WHY DID I ADD THIS? */
	QCheckBox *advanced = new QCheckBox( "Ad&vanced", this );
	advanced->setDisabled( true );
	advanced->setStyleSheet( "color:gray;");
	advanced->hide();

	QHBoxLayout *fLyt = new QHBoxLayout();
	fLyt->addWidget( advanced );
	fLyt->addStretch();
	fLyt->addWidget( formatsLbl );
	fLyt->addWidget( formatsCB );

	/* Buttons */
	NBSegmentButton *segBtn = new NBSegmentButton( this );
	segBtn->setCount( 2 );

	segBtn->setSegmentIcon( 0, QIcon( ":/icons/archive.png" ) );
	segBtn->setSegmentText( 0, "Create &Archive" );
	segBtn->segment( 0 )->setObjectName( "okBtn" );
	connect( segBtn->segment( 0 ), SIGNAL( clicked() ), this, SLOT( accept() ) );

	segBtn->setSegmentIcon( 1, QIcon( ":/icons/delete.png" ) );
	segBtn->setSegmentText( 1, "&Cancel" );
	segBtn->segment( 1 )->setObjectName( "cancelBtn" );
	connect( segBtn->segment( 1 ), SIGNAL( clicked() ), this, SLOT( reject() ) );

	QHBoxLayout *bLyt = new QHBoxLayout();
	bLyt->addStretch();
	bLyt->addWidget( segBtn );

	/* Final Layout */
	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addLayout( nLyt );
	baseLyt->addLayout( lLyt );
	baseLyt->addLayout( fLyt );
	baseLyt->addWidget( Separator::horizontal() );
	baseLyt->addLayout( bLyt );

	setLayout( baseLyt );
};

NBArchive* NBArchiveDialog::archive() {

	if ( filename.isEmpty() )
		return NULL;

	QString archiveName = filename;
	if ( not filename.endsWith( format ) )
		archiveName += format;

	return new NBArchive( QDir( location ).filePath( archiveName ) );
};

void NBArchiveDialog::updateFileName( QString fn ) {

	filename = QString( fn );
};

void NBArchiveDialog::updateDirName() {

	QString loc = NBDirectoryDialog::getDirectoryName( this, "NewBreeze - Choose Directory", QDir::currentPath() );
	if ( not loc.isEmpty() ) {
		locationLE->setText( loc );
		location = loc;
	}
};

void NBArchiveDialog::updateFormat( QString fmt ) {

	format = QString( fmt );
};
