/*
	*
	* NBMarkDownPreview.cpp - NewBreeze MarkDownPreviewWidget Class
	*
*/

#include "NBMarkDownPreviewWidget.hpp"

NBWebWatch::NBWebWatch( QString pth, QWidget *parent ) : QDialog( parent ) {

	path = QString( pth );

	createGUI();
	setWindowProperties();

	QTimer::singleShot( 0, this, SLOT( loadDocument() ) );

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
	peekWidgetBase->setFont( QFont( "sans", 10 ) );

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

void NBWebWatch::paintEvent( QPaintEvent *pEvent ) {

	QWidget::paintEvent( pEvent );
	QPainter *painter = new QPainter( this );

	painter->setPen( QPen( palette().color( QPalette::Window ).darker(), 2.0 ) );
	painter->drawRect( rect().adjusted( +1, +1, -1, -1 ) );

	painter->end();
	pEvent->accept();
};

void NBWebWatch::loadDocument() {

	Document *doc = new Document( path );
	peekWidgetBase->page()->mainFrame()->addToJavaScriptWindowObject( "doc", doc );
	peekWidgetBase->load( QUrl( "qrc:///index.html" ) );
};

void NBWebWatch::openInExternal() {

	QProcess::startDetached( "xdg-open " + path );
	close();
};

/* Name of the plugin */
QString NBMarkDownPreview::name() {

	return "MarkdownPreview";
};

/* The plugin version */
QString NBMarkDownPreview::version() {

	return "3.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> NBMarkDownPreview::actions( Interface, QStringList nodes ) {

	if ( ( nodes.count() == 1 ) and isFile( nodes.at( 0 ) ) ) {
		QAction *act = new QAction( QIcon( ":/icons/emblem-unmounted.png" ), "&Peek", this );

		NBWebWatch *mdview = new NBWebWatch( nodes.at( 0 ) );
		mdview->setWindowFlags( mdview->windowFlags() | Qt::FramelessWindowHint );
		connect( act, SIGNAL( triggered() ), mdview, SLOT( exec() ) );

		return QList<QAction*>() << act;
	}

	return QList<QAction*>();
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interfaces NBMarkDownPreview::interfaces() {

	return  NBPluginInterface::Interfaces() << NBPluginInterface::PreviewInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type NBMarkDownPreview::type( Interface ) {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts NBMarkDownPreview::contexts( Interface ) {

	return Contexts() << NBPluginInterface::File;
};

/* Mimetypes handled by the plugin */
QStringList NBMarkDownPreview::mimetypes() {

	return QStringList() << "text/markdown" << "text/x-markdown";
};

/* Invoke slots called called by triggering the actions */
void NBMarkDownPreview::actionTrigger( Interface, QString, QStringList nodes ) {

	if ( ( nodes.count() == 1 ) and isFile( nodes.at( 0 ) ) ) {
		NBWebWatch *mdview = new NBWebWatch( nodes.at( 0 ) );
		mdview->setWindowFlags( mdview->windowFlags() | Qt::FramelessWindowHint );
		mdview->exec();
	}
};

void NBMarkDownPreview::setCaller( QWidget *caller ) {

	mParent = caller;
};

Q_EXPORT_PLUGIN2( MarkDownPreview, NBMarkDownPreview );
