/*
	*
	* NBNewEncFS.cpp - New EncFS Volume Class
	*
*/

#include <NBNewEncFS.hpp>

NBNewEncFS::NBNewEncFS( QString encPath, QString decPath, QWidget *parent ) : QDialog( parent ) {

	/* Init the resources */
	Q_INIT_RESOURCE( Crypt );

	mOkay = false;

	/* Preferred/Default paths */
	encDirPath = encPath;
	decDirPath = decPath;

	createGUI();
	setWindowProperties();
};

QString NBNewEncFS::encryptedPath() {

	return encPathLE->text();
};

QString NBNewEncFS::decryptedPath() {

	return decPathLE->text();
};

QString NBNewEncFS::password() {

	return passwd1LE->text();
};

void NBNewEncFS::createGUI() {

	QLabel *encPathLbl = new QLabel( "Input the &encrypted directory path:", this );
	encPathLE = new QLineEdit( encDirPath, this );
	encPathLbl->setBuddy( encPathLE );
	encDirPath.clear();

	QLabel *decPathLbl = new QLabel( "Input the &decrypted directory path:", this );
	decPathLE = new QLineEdit( decDirPath, this );
	decPathLbl->setBuddy( decPathLE );
	decDirPath.clear();

	QLabel *passwd1Lbl = new QLabel( "Input the &password:", this );
	passwd1LE = new NBPasswordEdit( this );
	passwd1Lbl->setBuddy( passwd1LE );

	QLabel *passwd2Lbl = new QLabel( "Input the same password, &again:", this );
	passwd2LE = new NBPasswordEdit( this );
	passwd2Lbl->setBuddy( passwd2LE );

	connect( passwd2LE, SIGNAL( textChanged( const QString& ) ), this, SLOT( validatePassword( const QString& ) ) );

	QPushButton *okBtn = new QPushButton( QIcon::fromTheme( "dialog-ok-apply" ), "C&reate" );
	connect( okBtn, SIGNAL( clicked() ), this, SLOT( accept() ) );

	QPushButton *cancelBtn = new QPushButton( QIcon::fromTheme( "dialog-cancel" ), "&Cancel" );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addStretch();
	btnLyt->addWidget( okBtn );
	btnLyt->addWidget( cancelBtn );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addWidget( encPathLbl );
	baseLyt->addWidget( encPathLE );
	baseLyt->addWidget( decPathLbl );
	baseLyt->addWidget( decPathLE );
	baseLyt->addWidget( passwd1Lbl );
	baseLyt->addWidget( passwd1LE );
	baseLyt->addWidget( passwd2Lbl );
	baseLyt->addWidget( passwd2LE );
	baseLyt->addStretch();
	baseLyt->addLayout( btnLyt );

	setLayout( baseLyt );
};

void NBNewEncFS::setWindowProperties() {

	setWindowTitle( "NewBreeze - Input Password" );
	setWindowIcon( QIcon( ":/icons/appearance.png" ) );

	QRect scrnSize = QDesktopWidget().screenGeometry();
	int hpos = ( int )( ( scrnSize.width() - 600 ) / 2 );
	int vpos = ( int )( ( scrnSize.height() - 300 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 600, 300 );
};

void NBNewEncFS::validatePassword( const QString& passwd ) {

	while( passwd != passwd1LE->text() ) {
		int reply = QMessageBox::critical(
			this,
			"NewBreeze - Password Mismatch",
			"The two passwords you have input do not match. Please input the same password in both the password input boxes.",
			QMessageBox::Cancel, QMessageBox::Ok
		);

		if ( reply == QMessageBox::Cancel ) {

			passwd1LE->setText( QString() );
			passwd2LE->setText( QString() );

			return;
		}

		PasswordInput *pInput = new PasswordInput( this );
		if ( pInput->exec() == QDialog::Accepted ) {
			passwd2LE->setText( pInput->password() );
			pInput->clear();
		}
	}
};
