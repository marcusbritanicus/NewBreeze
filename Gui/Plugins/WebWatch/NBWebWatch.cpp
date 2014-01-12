/*
	*
	* NBWebWatch.hpp - NewBreeze HtmlPreviewWidget Classes
	*
*/

#include <NBWebWatch.hpp>

NBWebWatch::NBWebWatch( QString pth ) : QWidget() {

	path = QString( pth );

	createGUI();
	setWindowProperties();

	peekWidgetBase->setFocus();
};

void NBWebWatch::createGUI() {

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

	peekWidgetBase = new QWebView();
	peekWidgetBase->setRenderHints( QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing );
	peekWidgetBase->settings()->setUserStyleSheetUrl( QUrl::fromLocalFile( ":/StyleSheets/Template/CustomWebkit.qss" ) );

	peekWidgetBase->load( QUrl::fromLocalFile( path ) );

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

void NBWebWatch::setWindowProperties() {

	setFixedSize( 1024, 640 );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );
	setWindowFlags( Qt::FramelessWindowHint );

	setStyleSheet( getStyleSheet( "NBPreview", Settings->General.Style ) );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 1024 ) / 2 );
	int vpos = (int)( ( dw.height() - 640 ) / 2 );

	setGeometry( hpos, vpos, 1024, 640 );
};

void NBWebWatch::keyPressEvent( QKeyEvent *keyEvent ) {

	if ( keyEvent->key() == Qt::Key_Escape )
		close();

	else
		QWidget::keyPressEvent( keyEvent );
};

void NBWebWatch::changeEvent( QEvent *event ) {

	if ( ( event->type() == QEvent::ActivationChange ) and ( !isActiveWindow() ) ) {
		hide();
		event->accept();
	}

	else {
		QWidget::changeEvent( event );
		event->accept();
	}
};

void NBWebWatch::openInExternal() {

	QProcess::startDetached( "xdg-open " + path );
	close();
};
