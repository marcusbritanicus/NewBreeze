/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Classes
	*
*/

#include <NBPdfPeep.hpp>

NBPdfPeep::NBPdfPeep( QString pth ) : QWidget() {

	path = QString( pth );

	createGUI();
	setWindowProperties();

	peekWidgetBase->setFocus();
};

void NBPdfPeep::createGUI() {

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

	peekWidgetBase = new QScrollArea();
	peekWidgetBase->setFixedSize( 700, 500 );
	peekWidgetBase->setAlignment( Qt::AlignCenter );
	peekWidgetBase->setObjectName( tr( "previewBase" ) );

	QWidget *pdfBase = new QWidget();
	pdfLyt = new QVBoxLayout();
	pdfLyt->setContentsMargins( QMargins() );
	pdfBase->setLayout( pdfLyt );

	peekWidgetBase->setWidget( pdfBase );
	peekWidgetBase->setWidgetResizable( true );

	QTimer::singleShot( 100, this, SLOT( loadPdf() ) );

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

void NBPdfPeep::setWindowProperties() {

	setFixedSize( 720, 540 );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );
	setWindowFlags( Qt::FramelessWindowHint );

	setStyleSheet( getStyleSheet( "NBPreview", Settings->General.Style ) );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 540 ) / 2 );

	setGeometry( hpos, vpos, 720, 540 );

	/* We don't want to bother deleting this widget, let it get automatically done. */
	setAttribute( Qt::WA_DeleteOnClose );
};

void NBPdfPeep::keyPressEvent( QKeyEvent *keyEvent ) {

	if ( keyEvent->key() == Qt::Key_Escape )
		close();

	else
		QWidget::keyPressEvent( keyEvent );
};

void NBPdfPeep::changeEvent( QEvent *event ) {

	if ( ( event->type() == QEvent::ActivationChange ) and ( !isActiveWindow() ) ) {
		close();
		event->accept();
	}

	else {
		QWidget::changeEvent( event );
		event->accept();
	}
};

void NBPdfPeep::loadPdf() {

	int pageWidth = 700 - peekWidgetBase->verticalScrollBar()->width() - 4;
	Poppler::Document *doc = Poppler::Document::load( path );

	for ( int i = 0; i < doc->numPages(); i++ ) {
		if ( not isVisible() )
			return;

		QLabel *pdfPage = new QLabel();
		QImage image = doc->page( i )->renderToImage( 96, 96 );
		pdfPage->setPixmap( QPixmap::fromImage( image ).scaledToWidth( pageWidth, Qt::SmoothTransformation ) );
		pdfLyt->addWidget( pdfPage );

		qApp->processEvents();
	}

	delete doc;
};

void NBPdfPeep::openInExternal() {

	QProcess::startDetached( "xdg-open " + path );
	close();
};
