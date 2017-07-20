/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Classes
	*
*/

#include "NBPdfPeep.hpp"

NBPdfPeep::NBPdfPeep( QString pth ) : QDialog() {

	path = QString( pth );

	createGUI();
	setWindowProperties();

	peekWidgetBase->setFocus();
};

void NBPdfPeep::createGUI() {

	QHBoxLayout *lblBtnLyt = new QHBoxLayout();
	QVBoxLayout *widgetLyt = new QVBoxLayout();
	QVBoxLayout *baseLyt = new QVBoxLayout();

	lbl = new QLabel( "<tt><b>" + QFileInfo( path ).fileName() + "</b></tt>" );

	QToolButton *openBtn = new QToolButton();
	openBtn->setIcon( QIcon( ":/icons/maximize.png" ) );
	openBtn->setAutoRaise( true );
	openBtn->setFocusPolicy( Qt::NoFocus );

	QWidget *baseWidget = new QWidget();
	baseWidget->setObjectName( tr( "guiBase" ) );

	connect( openBtn, SIGNAL( clicked() ), this, SLOT( openInExternal() ) );

	peekWidgetBase = new QScrollArea();
	peekWidgetBase->setAlignment( Qt::AlignCenter );
	peekWidgetBase->setObjectName( tr( "previewBase" ) );

	QWidget *pdfBase = new QWidget();
	pdfLyt = new QVBoxLayout();
	pdfLyt->setAlignment( Qt::AlignHCenter );
	pdfLyt->setContentsMargins( QMargins() );
	pdfBase->setLayout( pdfLyt );

	peekWidgetBase->setWidget( pdfBase );
	peekWidgetBase->setWidgetResizable( true );

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

int NBPdfPeep::exec() {

	QTimer::singleShot( 0, this, SLOT( loadPdf() ) );
	return QDialog::exec();
};

void NBPdfPeep::openInExternal() {

	QProcess::startDetached( "xdg-open " + path );
	close();
};

void NBPdfPeep::loadPdf() {

	int pageWidth = 700 - peekWidgetBase->verticalScrollBar()->width() - 4;
	Poppler::Document *doc = Poppler::Document::load( path );

	qDebug() << "ABC";

	if ( not doc ) {
		QLabel *page = new QLabel( this );
		page->setAlignment( Qt::AlignCenter );
		page->setWordWrap( true );
		page->setText(
			"Unable to process the pdf document. The pdf file may be damaged or you may not have sufficient permissions to view it."
		);
		pdfLyt->addWidget( page );
	}

	else if ( doc->isEncrypted() or doc->isLocked() ) {
		QLabel *page = new QLabel( this );
		page->setAlignment( Qt::AlignCenter );
		page->setWordWrap( true );
		page->setText(
			"Unable to process the password protected pdf document. We will support password protected pdfs very soon."
		);
		pdfLyt->addWidget( page );
	}

	else {
		doc->setRenderHint( Poppler::Document::Antialiasing, true );
		doc->setRenderHint( Poppler::Document::TextAntialiasing, true );

		for ( int i = 0; i < doc->numPages(); i++ ) {

			QLabel *pdfPage = new QLabel();
			QImage image = doc->page( i )->renderToImage( 96, 96 );
			pdfPage->setPixmap( QPixmap::fromImage( image ).scaledToWidth( pageWidth, Qt::SmoothTransformation ) );
			pdfLyt->addWidget( pdfPage );

			qApp->processEvents();
			lbl->setText( "<tt><b>" + QFileInfo( path ).fileName() + QString( "</b></tt> (Loading... %1/%2)" ).arg( i + 1 ).arg( doc->numPages() ) );
		}

		lbl->setText( "<tt><b>" + QFileInfo( path ).fileName() + "</tt></b>" );
	}
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
