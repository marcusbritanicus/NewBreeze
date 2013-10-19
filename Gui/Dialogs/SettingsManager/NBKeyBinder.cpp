/*
	*
	* NewBreeze.hpp - NewBreeze Header
	*
*/

#include <NBKeyBinder.hpp>
#include <NBTools.hpp>

inline QList<int> range( int start, int end ) {

	QList<int> numbers;
	for( int i = start; i < end; i++ )
		numbers << i;

	return numbers;
};

KeyButton::KeyButton( QString text ) : QPushButton( text ) {

	setStyleSheet(
		"QPushButton {"								\
		"    border: 2px solid gray;"				\
		"    border-radius: 5px;"					\
		"    font-family: Envy Code R;"				\
		"    font-weight: bold;"					\
		"}"											\

		"QPushButton:checked {"						\
		"    color: red;"							\
		/*"    background-color: #DFDFDF;"			\*/
		"}"
	);

	setFocusPolicy( Qt::NoFocus );
};

KeyLabel::KeyLabel() : QLabel() {
	setStyleSheet(
		"QLabel {"									\
		/*"    background-color: #DFDFDF;"			\*/
		"    color: red;"							\
		"    font-family: Envy Code R;"				\
		"    font-weight: bold;"					\
		"    border: 2px solid gray;"				\
		"    border-radius: 5px;"					\
		"}"
	);
};

NBKeyBinder::NBKeyBinder() : QDialog() {

	createGui();
	setupConnections();
	setWindowProperties();
};

void NBKeyBinder::createGui() {
	/*
		*
		* createGui() -> None
		*
		* Create a GUI
		*
		* @return None
		*
	*/

	QLabel *labelInfo = new QLabel(
		"To choose a key combination, click on one or more of the modifier keys, "			\
		"and press a key (Alphabets, punctuations and numbers only)"
	);
	labelInfo->setWordWrap( true );

	labelKeys = new KeyLabel();
	labelKeys->setAlignment( Qt::AlignCenter );
	labelKeys->setFixedSize( 45, 45 );

	CtrlBtn = new KeyButton( "Ctrl" );
	CtrlBtn->setCheckable( true );
	CtrlBtn->setFixedSize( 45, 45 );

	AltBtn = new KeyButton( "Alt" );
	AltBtn->setCheckable( true );
	AltBtn->setFixedSize( 45, 45 );

	ShiftBtn = new KeyButton( "Shift" );
	ShiftBtn->setCheckable( true );
	ShiftBtn->setFixedSize( 45, 45 );

	MetaBtn = new KeyButton( "Meta" );
	MetaBtn->setCheckable( true );
	MetaBtn->setFixedSize( 45, 45 );

	QGridLayout *btnLyt = new QGridLayout();
	btnLyt->addWidget( CtrlBtn, 0, 0 );
	btnLyt->addWidget( AltBtn, 0, 1 );
	btnLyt->addWidget( ShiftBtn, 0, 2 );
	btnLyt->addWidget( MetaBtn, 0, 3 );
	btnLyt->addWidget( labelKeys, 0, 4 );

	QVBoxLayout *keysLyt = new QVBoxLayout();
	keysLyt->addWidget( new QLabel( "<h3><center>Setup a Global Shortcut</center></h3>" ) );
	keysLyt->addWidget( labelInfo );
	keysLyt->addLayout( btnLyt );

	QWidget *keysW = new QWidget();
	keysW->setLayout( keysLyt );

	QLabel *progInfo = new QLabel( "Enter the complete program path and arguments if (any-> "								\
		"This program will be invoked with the given arguments on the previously set key binding-> "						\
		"Arguments need to be separated with spaces-> if (an argument contains spaces, enclose it in double quotes->"
	);
	progInfo->setWordWrap( true );

	okBtn = new QPushButton( "Ok" );
	okBtn->setFixedSize( 72, 45 );
	connect( okBtn, SIGNAL( clicked() ), this, SLOT( accept() ) );

	cancelBtn = new QPushButton( "Cancel" );
	cancelBtn->setFixedSize( 72, 45 );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );

	QHBoxLayout *btnLyt2 = new QHBoxLayout();
	btnLyt2->addStretch( 0 );
	btnLyt2->addWidget( cancelBtn );
	btnLyt2->addWidget( okBtn );
	btnLyt2->addStretch( 0 );

	QVBoxLayout *baseLayout = new QVBoxLayout();
	baseLayout->addWidget( keysW );
	baseLayout->addLayout( btnLyt2 );

	setLayout( baseLayout );
};

void NBKeyBinder::setupConnections() {
	/*
		*
		* setupConnections() -> None
		*
        *
		* Setup the Signal Connections
        *
		* @return None
		*
	*/

	connect( okBtn, SIGNAL( clicked() ), this, SLOT( finish() ) );
};

void NBKeyBinder::setWindowProperties() {
	/*
		*
		* setWindowProperties() -> None
	    *
		* Set the window properties like title, size, position, etc...
	    *
		* @return None
		*
	*/

	setWindowTitle( "KeyBindings Capturer" );

	QRect screen = QDesktopWidget().screenGeometry();
	int hpos = ( screen.width() - 450 ) / 2;
	int vpos = ( screen.height() - 180 ) / 2;
	setGeometry( hpos, vpos, 450, 180 );

	setFixedSize( 450, 180 );

	// if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		// setAttribute( Qt::WA_TranslucentBackground );
	// if ( not Settings->General.NativeTitleBar )
		// setWindowFlags( Qt::FramelessWindowHint );
};

QString NBKeyBinder::keyBinding() {

	if ( labelKeys->text().isEmpty() )
		return QString();

	QString binding;
	binding += ( CtrlBtn->isChecked() ? "Ctrl+" : "" );
	binding += ( AltBtn->isChecked() ? "Alt+" : "" );
	binding += ( ShiftBtn->isChecked() ? "Shift+" : "" );
	binding += ( MetaBtn->isChecked() ? "Meta+" : "" );
	binding += labelKeys->text();

	return binding;
};

void NBKeyBinder::keyPressEvent( QKeyEvent *kEvent ) {
	/*
		*
		* keyPressEvent( QKeyEvent ) -> None
	    *
		* Re-imlement the QDialog::keyPressEvent
	    *
		* @return None
		*
	*/


	QList<int> allowed = QList<int>() << range( 42, 58 ) << range( 65, 94 ) << 16777232 << 16777233 << 39 << 59 << 61 << 96;

	if ( kEvent->key() == Qt::Key_Control ) {
		if ( CtrlBtn->isChecked() )
			CtrlBtn->setChecked( false );

		else
			CtrlBtn->setChecked( true );
	}

	else if ( kEvent->key() == Qt::Key_Alt ) {
		if ( AltBtn->isChecked() )
			AltBtn->setChecked( false );

		else
			AltBtn->setChecked( true );
	}

	else if ( kEvent->key() == Qt::Key_Shift ) {
		if ( ShiftBtn->isChecked() )
			ShiftBtn->setChecked( false );

		else
			ShiftBtn->setChecked( true );
	}

	else if ( kEvent->key() == Qt::Key_Meta ) {
		if ( MetaBtn->isChecked() )
			MetaBtn->setChecked( false );

		else
			MetaBtn->setChecked( true );
	}

	else if ( allowed.contains( kEvent->key() ) ) {
		labelKeys->setText( kEvent->text().toUpper() );
		okBtn->setEnabled( true );
	}

	else if ( range( 16777264, 16777299 ).contains( kEvent->key() ) ) {
		switch( kEvent->key() ) {
			case 16777264: labelKeys->setText( "F1" ); break;
			case 16777265: labelKeys->setText( "F2" ); break;
			case 16777266: labelKeys->setText( "F3" ); break;
			case 16777267: labelKeys->setText( "F4" ); break;
			case 16777268: labelKeys->setText( "F5" ); break;
			case 16777269: labelKeys->setText( "F6" ); break;
			case 16777270: labelKeys->setText( "F7" ); break;
			case 16777271: labelKeys->setText( "F8" ); break;
			case 16777272: labelKeys->setText( "F9" ); break;
			case 16777273: labelKeys->setText( "F10" ); break;
			case 16777274: labelKeys->setText( "F11" ); break;
			case 16777275: labelKeys->setText( "F12" ); break;
			case 16777276: labelKeys->setText( "F13" ); break;
			case 16777277: labelKeys->setText( "F14" ); break;
			case 16777278: labelKeys->setText( "F15" ); break;
			case 16777279: labelKeys->setText( "F16" ); break;
			case 16777280: labelKeys->setText( "F17" ); break;
			case 16777281: labelKeys->setText( "F18" ); break;
			case 16777282: labelKeys->setText( "F19" ); break;
			case 16777283: labelKeys->setText( "F20" ); break;
			case 16777284: labelKeys->setText( "F21" ); break;
			case 16777285: labelKeys->setText( "F22" ); break;
			case 16777286: labelKeys->setText( "F23" ); break;
			case 16777287: labelKeys->setText( "F24" ); break;
			case 16777288: labelKeys->setText( "F25" ); break;
			case 16777289: labelKeys->setText( "F26" ); break;
			case 16777290: labelKeys->setText( "F27" ); break;
			case 16777291: labelKeys->setText( "F28" ); break;
			case 16777292: labelKeys->setText( "F29" ); break;
			case 16777293: labelKeys->setText( "F30" ); break;
			case 16777294: labelKeys->setText( "F31" ); break;
			case 16777295: labelKeys->setText( "F32" ); break;
			case 16777296: labelKeys->setText( "F33" ); break;
			case 16777297: labelKeys->setText( "F34" ); break;
			case 16777298: labelKeys->setText( "F35" ); break;
		}
		okBtn->setEnabled( true );
	}

	else if ( kEvent->key() == Qt::Key_Space ) {
		labelKeys->setText( "Space" );
		okBtn->setEnabled( true );
	}

	else if ( kEvent->key() == Qt::Key_Backspace ) {
		labelKeys->clear();
		okBtn->setDisabled( true );
	}

	else {
		QDialog::keyPressEvent( kEvent );
		labelKeys->clear();
		okBtn->setDisabled( true );
	}

	kEvent->accept();
};

void NBKeyBinder::finish() {
	/*
		*
		* finish() -> None
		*
		* Save the keybinding and the program
		*
		* @return None
		*
	*/

	accept();
	close();
};
