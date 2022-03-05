/*
	*
	* NBPdfPeep.hpp - NewBreeze PdfPeep Classes
	*
*/

#include "NBPdfPeep.hpp"

NBPdfPeep::NBPdfPeep( QString pth ) : QDialog() {

	mPath = QString( pth );

	createGUI();
	setWindowProperties();

	peekWidgetBase->setFocus();
};

void NBPdfPeep::createGUI() {

	QHBoxLayout *lblBtnLyt = new QHBoxLayout();
	QVBoxLayout *widgetLyt = new QVBoxLayout();
	QVBoxLayout *baseLyt = new QVBoxLayout();

	lbl = new QLabel( "<tt><b>" + baseName( mPath ) + "</b></tt>" );

	QToolButton *openBtn = new QToolButton();
	openBtn->setIcon( QIcon( ":/icons/maximize.png" ) );
	openBtn->setAutoRaise( true );
	openBtn->setFocusPolicy( Qt::NoFocus );

	QWidget *baseWidget = new QWidget();
	baseWidget->setObjectName( tr( "guiBase" ) );

	connect( openBtn, SIGNAL( clicked() ), this, SLOT( openInExternal() ) );

	/* PdfView */
	peekWidgetBase = new QDocumentView( this );
	peekWidgetBase->setObjectName( tr( "previewBase" ) );
    peekWidgetBase->setPageLayout( QDocumentView::SinglePage );
    peekWidgetBase->setLayoutContinuous( true );
    peekWidgetBase->setZoomMode( QDocumentView::FitToWidth );
	peekWidgetBase->load( mPath );

	QShortcut *close = new QShortcut( QKeySequence( Qt::Key_Escape ), this, SLOT( close() ), SLOT( close() ), Qt::WindowShortcut );

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

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 540 ) / 2 );

	setGeometry( hpos, vpos, 720, 540 );

	/* We don't want to bother deleting this widget, let it get automatically done. */
	setAttribute( Qt::WA_DeleteOnClose );
};

void NBPdfPeep::openInExternal() {

	QProcess::startDetached( "xdg-open " + mPath );
	close();
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

void NBPdfPeep::paintEvent( QPaintEvent *pEvent ) {

	QWidget::paintEvent( pEvent );
	QPainter *painter = new QPainter( this );

	painter->setPen( QPen( palette().color( QPalette::Window ).darker(), 2.0 ) );
	painter->drawRect( rect().adjusted( +1, +1, -1, -1 ) );

	painter->end();
	pEvent->accept();
};
