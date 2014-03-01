/*
	*
	* NBUtilityBar.cpp - NewBreeze UtilityBar Class
	*
*/

#include <NBTools.hpp>
#include <NBUtilityBar.hpp>

NBUtilityBar::NBUtilityBar() : QWidget() {

	createGUI();
	// setupConnections();
};

NBUtilityBar::~NBUtilityBar() {

};

void NBUtilityBar::createGUI() {

	setObjectName( tr( "UtilityBar") );
	setFixedHeight( 40 );

	tabs = new NBTabWidget();

	closeBtn = new NBToolButton( QString( ":/icons/delete.png" ) );
	closeBtn->setObjectName( "quitBtn" );
	closeBtn->setIconSize( QSize( 28, 28 ) );
	closeBtn->setFixedSize( QSize( 32, 32 ) );
	closeBtn->setShortcut( tr( "Ctrl+Q" ) );
	connect( closeBtn, SIGNAL( clicked() ), this, SIGNAL( closeWindow() ) );

	procWidget = new NBToolButton( QString( ":/icons/info.png" ) );
	procWidget->setFixedSize( QSize( 32, 32 ) );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setContentsMargins( 5, 0, 5, 0 );
	lyt->setAlignment( Qt::AlignVCenter );
	lyt->setSpacing( 0 );

	lyt->addWidget( NBSpacer::horizontal( 64 ) );
	lyt->addWidget( tabs );
	lyt->addStretch();
	lyt->addWidget( NBSpacer::horizontal( 16 ) );
	lyt->addWidget( procWidget );
	lyt->addWidget( NBSpacer::horizontal( 16 ) );
	lyt->addWidget( closeBtn );

	setLayout( lyt );
};

void NBUtilityBar::mouseDoubleClickEvent( QMouseEvent * ) {

	if ( isMaximized() )
		emit restoreWindow();

	else
		emit maximizeWindow();
};

void NBUtilityBar::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( mpEvent->button() == Qt::LeftButton ) {
		emit titleBarMousePress( mpEvent );
		mpEvent->accept();
	}
};

void NBUtilityBar::mouseMoveEvent( QMouseEvent *mmEvent ) {

	if ( mmEvent->buttons() & Qt::LeftButton ) {
		emit titleBarMouseMove( mmEvent );
		mmEvent->accept();
	}

	QWidget::mouseMoveEvent( mmEvent );
};

void NBUtilityBar::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );
	painter->save();

	painter->setPen( Qt::gray );
	painter->drawLine( QPoint( 0, 39 ), QPoint( width(), 39 ) );

	painter->drawPixmap( QRect( 6, 6, 28, 28 ), QIcon::fromTheme( "newbreeze" ).pixmap( 28 ) );

	painter->restore();
	painter->end();

	QWidget::paintEvent( pEvent );
};

NBTabWidget::NBTabWidget() : QWidget() {

	__currentIndex = -1;
	setFixedHeight( 30 );

	tabsLyt = new QHBoxLayout();
	tabsLyt->setContentsMargins( QMargins() );
	tabsLyt->setSpacing( 0 );

	setLayout( tabsLyt );
};

int NBTabWidget::addTab( QString path ) {

	NBTab *tab = new NBTab( NBIconProvider::icon( path ), path );
	tab->showText();
	tabsList << tab;

	connect( tab, SIGNAL( clicked() ), this, SLOT( switchToTab() ) );

	if ( __currentIndex >= 0 )
		tabsList.value( __currentIndex )->hideText();

	tabsLyt->addWidget( tab );

	__currentIndex = tabsList.count() - 1;
};

void NBTabWidget::removeTab( int tab ) {

	tabsList.removeAt( tab );

	__currentIndex--;
	tabsList.value( __currentIndex )->showText();
};

void NBTabWidget::setCurrentIndex( int tab ) {

	tabsList.value( __currentIndex )->hideText();
	__currentIndex = tab;

	tabsList.value( tab )->showText();
};

int NBTabWidget::currentIndex() {

	return __currentIndex;
};

void NBTabWidget::switchToTab() {

	NBTab *tab = qobject_cast<NBTab *>( sender() );

	int index = tabsList.indexOf( tab );
	if ( index >= 0 )
		setCurrentIndex( index );
};

void NBTabWidget::paintEvent( QPaintEvent *pEvent ) {

	QWidget::paintEvent( pEvent );

	QPainter *painter = new QPainter( this );
	painter->save();

	painter->setPen( Qt::gray );
	painter->drawRoundedRect( 0, 0, width() - 1, height() - 1, 2, 2 );

	painter->restore();
	painter->end();
};

NBTab::NBTab( QString icon, QString path ) {

	QFontMetrics fm( font() );

	__text = fm.elidedText( baseName( path ), Qt::ElideRight, 120 );
	__icon = QIcon::fromTheme( icon, QIcon( icon ) );

	bool __textVisible = false;

	setToolTip( path );
	repaint();
};

void NBTab::showText() {

	__textVisible = true;
	setFixedWidth( 150 );

	repaint();
};

void NBTab::hideText() {

	__textVisible = false;
	setFixedWidth( 30 );

	repaint();
};

void NBTab::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );
	painter->save();

	if ( not __textVisible ) {
		painter->drawPixmap( QRect( 3, 3, 24, 24 ), __icon.pixmap( 24 ) );

		painter->setPen( Qt::gray );
		painter->drawLine( QPoint( 29, 2 ), QPoint( 29, 27 ) );
	}

	else {
		painter->drawPixmap( QRect( 3, 3, 24, 24 ), __icon.pixmap( 24 ) );
		painter->drawText( QRect( 30, 0, 120, 30 ), Qt::AlignVCenter | Qt::AlignLeft, __text );

		painter->setPen( Qt::gray );
		painter->drawLine( QPoint( 149, 2 ), QPoint( 149, 27 ) );
	}

	painter->restore();
	painter->end();

	pEvent->accept();
};

void NBTab::mousePressEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {

		emit clicked();
	}

	mEvent->accept();
};
