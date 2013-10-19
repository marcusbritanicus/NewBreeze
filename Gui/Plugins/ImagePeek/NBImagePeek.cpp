/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Classes
	*
*/

#include <NBImagePeek.hpp>
#include <NBTools.hpp>

NBImagePeek::NBImagePeek( QWidget *parent, QString pth ) : QWidget( parent ) {

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
	peekWidgetBase->setFixedSize( 700, 500 );
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
	setWindowFlags( Qt::Popup );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	setStyleSheet( getStyleSheet( "NBPreview", Settings->General.Style ) );

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

void NBImagePeek::loadImage() {

	if ( getMimeType( path ).contains( "gif" ) or getMimeType( path ).contains( "mng" ) ) {
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

void NBImagePeek::openInExternal() {

	QProcess::startDetached( "xdg-open " + path );
	close();
};
