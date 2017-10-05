/*
	*
	* NBPasswordDialog.cpp - New EncFS Volume Class
	*
	* This dialog can be used for the entry of a new password
	* or changing the password.
	*
*/

#include "NBPasswordDialog.hpp"

NBPasswordDialog::NBPasswordDialog( QWidget *parent ) : QDialog( parent ) {

	createGUI();
	setWindowProperties();
};

QByteArray NBPasswordDialog::password() {

	return passwd1LE->text().toLocal8Bit();
};

void NBPasswordDialog::clear() {

	passwd1LE->clear();
	passwd2LE->clear();
};

void NBPasswordDialog::createGUI() {

	QLabel *passwd1Lbl = new QLabel( "Input the &password:", this );
	passwd1LE = new NBPasswordEdit( this );
	passwd1Lbl->setBuddy( passwd1LE );

	QLabel *passwd2Lbl = new QLabel( "Input the same password, &again:", this );
	passwd2LE = new NBPasswordEdit( this );
	passwd2Lbl->setBuddy( passwd2LE );

	connect( passwd2LE, SIGNAL( passwordSet() ), this, SLOT( validatePassword() ) );

	okBtn = new QPushButton( QIcon::fromTheme( "dialog-ok-apply" ), "Okay" );
	connect( okBtn, SIGNAL( clicked() ), this, SLOT( accept() ) );
	okBtn->setDisabled( true );

	QPushButton *cancelBtn = new QPushButton( QIcon::fromTheme( "dialog-cancel" ), "&Cancel" );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addStretch();
	btnLyt->addWidget( okBtn );
	btnLyt->addWidget( cancelBtn );

	QVBoxLayout *baseLyt = new QVBoxLayout();

	baseLyt->addWidget( passwd1Lbl );
	baseLyt->addWidget( passwd1LE );
	baseLyt->addWidget( passwd2Lbl );
	baseLyt->addWidget( passwd2LE );
	baseLyt->addStretch();
	baseLyt->addLayout( btnLyt );

	setLayout( baseLyt );
};

void NBPasswordDialog::setWindowProperties() {

	setWindowTitle( "NewBreeze - Input Password" );
	setWindowIcon( QIcon( ":/icons/appearance.png" ) );

	QRect scrnSize = QDesktopWidget().screenGeometry();
	int hpos = ( int )( ( scrnSize.width() - 600 ) / 2 );
	int vpos = ( int )( ( scrnSize.height() - 200 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 600, 180 );
};

void NBPasswordDialog::validatePassword() {

	if ( passwd2LE->text() == passwd1LE->text() ) {

		okBtn->setEnabled( true );
		return;
	}

	/* When inside the while loop, we do not need NBPasswordEdit::passwordSet() signal. */
	disconnect( passwd2LE, SIGNAL( passwordSet() ), this, SLOT( validatePassword() ) );

	do {
		int reply = QMessageBox::critical(
			this,
			"NewBreeze - Password Mismatch",
			"The two passwords you have input do not match. Please input the same password in both the password input boxes.",
			QMessageBox::Cancel, QMessageBox::Ok
		);

		if ( reply == QMessageBox::Cancel ) {

			passwd1LE->setText( QString() );
			passwd2LE->setText( QString() );

			connect( passwd2LE, SIGNAL( passwordSet() ), this, SLOT( validatePassword() ) );

			return;
		}

		NBPasswordInput *pInput = new NBPasswordInput( this );
		if ( pInput->exec() == QDialog::Accepted ) {

			passwd2LE->setText( pInput->password() );
			pInput->clear();
		}
	} while ( passwd2LE->text() != passwd1LE->text() );

	okBtn->setEnabled( passwd2LE->text() == passwd1LE->text() );

	/* Now that we are outside the while loop, we renable the signal */
	connect( passwd2LE, SIGNAL( passwordSet() ), this, SLOT( validatePassword() ) );
};
