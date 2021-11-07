/*
	*
	* NBPasswordInput.cpp - NBPasswordInput class
	*
*/

#include "NBPasswordInput.hpp"

NBPasswordInput::NBPasswordInput( QWidget *parent ) : QDialog( parent ) {

	mOkay = false;

	createGUI();
	setWindowProperties();
};

QString NBPasswordInput::password() {

	return mPassword;
};

void NBPasswordInput::clear() {

	pKeyPad->clear();
	passwdLE->clear();
	mPassword.clear();
};

void NBPasswordInput::createGUI() {

	pKeyPad = new PasswordKeypad( this );
	connect( pKeyPad, SIGNAL( inputComplete() ), this, SLOT( setPassword() ) );
	connect( pKeyPad, SIGNAL( inputComplete() ), this, SLOT( accept() ) );

	passwdLE = new QLineEdit();

	passwdLE->setAlignment( Qt::AlignCenter );
	passwdLE->setEchoMode( QLineEdit::Password );
	passwdLE->setStyleSheet( "lineedit-password-character: 8729;" );
	passwdLE->setFixedHeight( 32 );

	connect( passwdLE, SIGNAL( returnPressed() ), this, SLOT( setPassword() ) );
	connect( passwdLE, SIGNAL( returnPressed() ), this, SLOT( accept() ) );

	QVBoxLayout *base = new QVBoxLayout();
	base->addWidget( new QLabel( "Draw your password pattern below:", this ) );
	base->addWidget( pKeyPad );
	base->addWidget( new QLabel( "Or type in a password:", this ) );
	base->addWidget( passwdLE );

	setLayout( base );
};

void NBPasswordInput::setWindowProperties() {

	setWindowTitle( "NewBreeze - Input Password" );
	setWindowIcon( QIcon( ":/icons/newbreeze.png" ) );

	QRect scrnSize = qApp->primaryScreen()->geometry();
	int hpos = ( int )( ( scrnSize.width() - 300 ) / 2 );
	int vpos = ( int )( ( scrnSize.height() - 400 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 300, 400 );
};

void NBPasswordInput::setPassword() {

	if ( pKeyPad == qobject_cast<PasswordKeypad*>( sender() ) ) {
		mPassword = pKeyPad->password();
		pKeyPad->clear();
	}

	else if ( ( passwdLE == qobject_cast<QLineEdit*>( sender() ) ) ) {
		mPassword = passwdLE->text();
		passwdLE->clear();
	}
};
