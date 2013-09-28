/*
	*
	* NBProgressDialog.cpp - IO Progress Dialog for NewBreeze
	*
*/

#include <NBProgressDialog.hpp>

NBProgressDialog::NBProgressDialog() : NBDialog() {

	loner = false;

	sourceLbl = new QLabel();
	targetLbl = new QLabel();
	curfileLbl = new QLabel();
	speedLbl = new QLabel();

	totalBar = new QProgressBar();
	totalBar->setRange( 0, 100 );

	cfileBar = new QProgressBar();
	cfileBar->setRange( 0, 100 );

	closeBtn = new QPushButton();
	closeBtn->setText( "&Close" );
	closeBtn->setObjectName( "cancelBtn" );
	connect( closeBtn, SIGNAL( clicked() ), this, SLOT( hide() ) );

	cancelBtn = new QPushButton();
	cancelBtn->setText( "&Cancel" );
	cancelBtn->setObjectName( "abortBtn" );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( cancelIO() ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addWidget( cancelBtn );
	btnLyt->addStretch( 0 );
	btnLyt->addWidget( closeBtn );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addWidget( sourceLbl );
	baseLyt->addWidget( targetLbl );
	baseLyt->addWidget( curfileLbl );
	baseLyt->addWidget( speedLbl );
	baseLyt->addWidget( totalBar );
	baseLyt->addWidget( cfileBar );
	baseLyt->addLayout( btnLyt );

	setLayout( baseLyt );

	closeBtn->setFocus();
};

void NBProgressDialog::setIOMode( char mode ) {

	switch( mode ) {

		case 'c' :
			setDialogTitle( "Copying files" );
			break;

		case 'm' :
			setDialogTitle( "Moving files" );
			break;
	}
};

void NBProgressDialog::setSource( QString source ) {

	sourceLbl->setText( QString( "Source: %1" ).arg( source ) );
};

void NBProgressDialog::setTarget( QString target ) {

	targetLbl->setText( QString( "Target: %1" ).arg( target ) );
};

void NBProgressDialog::update( QString name, float cfileCopied, float totalCopied ) {

	curfileLbl->setText( name );
	cfileBar->setValue( cfileCopied );
	totalBar->setValue( totalCopied );
};

void NBProgressDialog::cancelIO() {

};

void NBProgressDialog::hide() {

	if ( not loner ) {
		emit closed();
		NBDialog::hide();
	}
};
