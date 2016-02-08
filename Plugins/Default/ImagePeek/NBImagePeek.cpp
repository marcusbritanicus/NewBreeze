/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Classes
	*
*/

#include <NBImagePeek.hpp>

NBImagePeek::NBImagePeek( QString pth ) : QDialog() {

	path = QString( pth );

	createGUI();
	setWindowProperties();

	peekWidgetBase->setFocus();
};

void NBImagePeek::createGUI() {

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

	peekWidgetBase = new QLabel();
	peekWidgetBase->setAlignment( Qt::AlignCenter );
	peekWidgetBase->setObjectName( tr( "previewBase" ) );

	loadImage();

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

void NBImagePeek::setWindowProperties() {

	setFixedSize( 720, 540 );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 540 ) / 2 );

	setGeometry( hpos, vpos, 720, 540 );
};

void NBImagePeek::keyPressEvent( QKeyEvent *keyEvent ) {

	if ( keyEvent->key() == Qt::Key_Escape )
		close();

	else
		QWidget::keyPressEvent( keyEvent );
};

void NBImagePeek::changeEvent( QEvent *event ) {

	if ( ( event->type() == QEvent::ActivationChange ) and ( !isActiveWindow() ) ) {
		hide();
		event->accept();
	}

	else {
		QWidget::changeEvent( event );
		event->accept();
	}
};

void NBImagePeek::loadImage() {

	// MNG/GIF File
	if ( path.endsWith( "gif" ) or path.endsWith( "mng" ) ) {
		QMovie *movie = new QMovie( path );
		peekWidgetBase->setMovie( movie );
		movie->start();
		return;
	}

	QImage image = QImage( path );
	QSize isize = QSize();

	if ( !image.isNull() ) {
		if ( ( image.width() < 700 ) and ( image.height() < 500 ) ) {
			isize = image.size();
		}
		else {
			isize = QSize( image.size() );
			isize.scale( QSize( 700, 500 ), Qt::KeepAspectRatio );
		}
	}
	else {
		peekWidgetBase->setText( "<h1>Error loading image</h1>" );
		return;
	}

	peekWidgetBase->setPixmap( QPixmap::fromImage( image ).scaled( isize, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
};

void NBImagePeek::paintEvent( QPaintEvent *pEvent ) {

	QWidget::paintEvent( pEvent );
	QPainter *painter = new QPainter( this );

	painter->setPen( QPen( palette().color( QPalette::Window ).darker(), 2.0 ) );
	painter->drawRect( rect().adjusted( +1, +1, -1, -1 ) );

	painter->end();
	pEvent->accept();
};

void NBImagePeek::openInExternal() {

	QProcess::startDetached( "xdg-open " + path );
	close();
};
