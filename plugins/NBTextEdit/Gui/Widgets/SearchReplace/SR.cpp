/*
    *
    * SR.cpp - Wordy's Search-Replace Class
    *
*/

#include <SR.hpp>

SearchWidget::SearchWidget( QWidget *parent ) : QWidget( parent ) {

	setupUI();
};

void SearchWidget::setupUI() {

	// Modality
	setWindowFlags( Qt::WindowStaysOnTopHint );

	// Minimum Width
	setFixedHeight( 48 );
	setMinimumWidth( 360 );

	// Options Holder
	optsGB = new QGroupBox();
	optsGB->setVisible( false );

	// Widgets - Label and LineEdit

	findLbl = new QLabel();
	findLbl->setPixmap( QIcon::fromTheme( "edit-find" ).pixmap( 16 ) );

	findLE = new QLineEdit();
	findLE->setFocusPolicy( Qt::StrongFocus );
	connect( findLE, SIGNAL( textChanged( QString ) ), this, SLOT( updateUI() ) );
	connect( findLE, SIGNAL( textChanged( QString ) ), this, SLOT( autoFind() ) );
	connect( findLE, SIGNAL( returnPressed() ), this, SLOT( findNextClicked() ) );

	// Widgets - CheckBoxes
	useRegexCB = new QCheckBox( "&Use regular Expressions" );
	connect( useRegexCB, SIGNAL( clicked() ), this, SLOT( updateUI() ) );

	caseSensitiveCB = new QCheckBox( "C&ase Sensitive Search" );
	caseSensitiveCB->setFocusPolicy( Qt::NoFocus );
	connect( caseSensitiveCB, SIGNAL( clicked() ), this, SLOT( updateUI() ) );

	wholeWordsCB = new QCheckBox( "&Whole Words Only" );
	wholeWordsCB->setFocusPolicy( Qt::NoFocus );
	connect( wholeWordsCB, SIGNAL( clicked() ), this, SLOT( updateUI() ) );

	// Widgets - Buttons
	findPrevBtn = new QToolButton();
	findPrevBtn->setIcon( QIcon::fromTheme( "arrow-up" ) );
	findPrevBtn->setFocusPolicy( Qt::NoFocus );
	connect( findPrevBtn, SIGNAL( clicked() ), this, SLOT( findPrevClicked() ) );

	findNextBtn = new QToolButton();
	findNextBtn->setIcon( QIcon::fromTheme( "arrow-down" ) );
	findNextBtn->setFocusPolicy( Qt::NoFocus );
	connect( findNextBtn, SIGNAL( clicked() ), this, SLOT( findNextClicked() ) );

	ButtonSet2 *bSet1 = new ButtonSet2();
	bSet1->addSegment( findPrevBtn );
	bSet1->addSegment( findNextBtn );

	settingsBtn = new QToolButton();
	settingsBtn->setIcon( QIcon::fromTheme( "preferences-other" ) );
	settingsBtn->setCheckable( true );
	settingsBtn->setFocusPolicy( Qt::NoFocus );
	connect( settingsBtn, SIGNAL( toggled( bool ) ), this, SLOT( toggleOpts( bool ) ) );

	ButtonSet2 *bSet2 = new ButtonSet2();
	bSet2->addSegment( settingsBtn );

	// Layouts
	QHBoxLayout *srchLyt = new QHBoxLayout();
	srchLyt->addWidget( findLbl );
	srchLyt->addWidget( findLE );
	srchLyt->addWidget( bSet1 );
	srchLyt->addWidget( bSet2 );

	QVBoxLayout *optsLyt = new QVBoxLayout();
	optsLyt->addWidget( useRegexCB );
	optsLyt->addWidget( caseSensitiveCB );
	optsLyt->addWidget( wholeWordsCB );
	optsGB->setLayout( optsLyt );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addLayout( srchLyt );
	baseLyt->addWidget( optsGB );

	QWidget *base = new QWidget();
	base->setObjectName( "guiBase" );
	base->setLayout( baseLyt );

	QVBoxLayout *widgetLyt = new QVBoxLayout();
	widgetLyt->addWidget( base );

	setLayout( widgetLyt );
};

void SearchWidget::updateUI() {

	if ( findLE->text().isEmpty() ) {
		findNextBtn->setDisabled( true );
		findPrevBtn->setDisabled( true );
	}

	else {
		findNextBtn->setEnabled( true );
		findPrevBtn->setEnabled( true );
	}
};

void SearchWidget::toggleOpts( bool shown ) {

	if ( shown ) {
		optsGB->show();
		setFixedHeight( 135 );
	}

	else {
		optsGB->hide();
		setFixedHeight( 48 );
	}
};

void SearchWidget::autoFind() {

	emit autofind( findLE->text(), useRegexCB->isChecked(), caseSensitiveCB->isChecked(), wholeWordsCB->isChecked() );
};

void SearchWidget::findPrevClicked() {

	emit find( findLE->text(), useRegexCB->isChecked(), caseSensitiveCB->isChecked(), wholeWordsCB->isChecked(), false );
};

void SearchWidget::findNextClicked() {

	emit find( findLE->text(), useRegexCB->isChecked(), caseSensitiveCB->isChecked(), wholeWordsCB->isChecked(), true );
};

void SearchWidget::setFocus() {

	findLE->setFocus();
};

void SearchWidget::show() {

	QWidget::show();
	emit shown();
};

void SearchWidget::hide() {

	QWidget::hide();
	emit hidden();
};

void SearchWidget::keyPressEvent( QKeyEvent *kEvent ) {

	switch( kEvent->key() ) {
		case Qt::Key_Escape : {
			hide();
			break;
		}

		default: {
			QWidget::keyPressEvent( kEvent );
			break;
		}
	};

	kEvent->accept();
};

ReplaceWidget::ReplaceWidget( QWidget *parent ) : QWidget( parent ) {
};
