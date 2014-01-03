/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Classes
	*
*/

#include <NBOdfOgle.hpp>

NBOdfOgle::NBOdfOgle( QString pth ) : QWidget() {

	path = QString( pth );

	createGUI();
	setWindowProperties();

	peekWidgetBase->setFocus();
};

void NBOdfOgle::createGUI() {

	QHBoxLayout *lblBtnLyt = new QHBoxLayout();
	QVBoxLayout *widgetLyt = new QVBoxLayout();
	QVBoxLayout *baseLyt = new QVBoxLayout();

	QLabel *lbl = new QLabel( "<tt><b>" + path + "</b></tt>" );

	QToolButton *openBtn = new QToolButton();
	openBtn->setIcon( QIcon( ":/icons/maximize.png" ) );
	openBtn->setAutoRaise( true );
	openBtn->setFocusPolicy( Qt::NoFocus );

	QWidget *baseWidget = new QWidget();
	baseWidget->setObjectName( tr( "guiBase" ) );

	connect( openBtn, SIGNAL( clicked() ), this, SLOT( openInExternal() ) );

	peekWidgetBase = new QTextBrowser();

	// Some basic settings
	peekWidgetBase->setTabStopWidth( 9 * 4 );
	peekWidgetBase->setFont( QFont( "Courier 10 Pitch", 9 ) );
	peekWidgetBase->setAlignment( Qt::AlignLeft );
	peekWidgetBase->setAcceptRichText( false );
	peekWidgetBase->setReadOnly( true );

	// Remove Horizontal Scrollbar, put a Vertical one
	peekWidgetBase->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	peekWidgetBase->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

	// Enable word wrap
	peekWidgetBase->setWordWrapMode( QTextOption::WrapAtWordBoundaryOrAnywhere );
	peekWidgetBase->setObjectName( tr( "previewBase" ) );

	peekWidgetBase->setDocument( loadFile() );

	lblBtnLyt->addWidget( lbl );
	lblBtnLyt->addStretch( 0 );
	lblBtnLyt->addWidget( openBtn );

	widgetLyt->addLayout( lblBtnLyt );
	widgetLyt->addWidget( peekWidgetBase );

	baseWidget->setLayout( widgetLyt );
	baseLyt->addWidget( baseWidget );
	baseLyt->setContentsMargins( 0, 0, 0, 0 );

	setLayout( baseLyt );
	peekWidgetBase->setFocus();
};

void NBOdfOgle::setWindowProperties() {

	setFixedSize( 720, 540 );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );
	setWindowFlags( Qt::FramelessWindowHint );

	setStyleSheet( getStyleSheet( "NBPreview", Settings->General.Style ) );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 540 ) / 2 );

	setGeometry( hpos, vpos, 720, 540 );
};

void NBOdfOgle::keyPressEvent( QKeyEvent *keyEvent ) {

	if ( keyEvent->key() == Qt::Key_Escape )
		close();

	else
		QWidget::keyPressEvent( keyEvent );
};

void NBOdfOgle::changeEvent( QEvent *event ) {

	if ( ( event->type() == QEvent::ActivationChange ) and ( !isActiveWindow() ) ) {
		hide();
		event->accept();
	}

	else {
		QWidget::changeEvent( event );
		event->accept();
	}
};

QTextDocument* NBOdfOgle::loadFile() {

	OOO::Converter *odtConverter = new OOO::Converter();

	return odtConverter->convert( path );
};

void NBOdfOgle::openInExternal() {

	QProcess::startDetached( "xdg-open " + path );
	close();
};
