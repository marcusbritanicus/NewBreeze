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

	tabs = new NBTabs();

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

NBTabs::NBTabs() : QTabBar() {

	QTabBar::addTab( QIcon::fromTheme( "folder" ), QString( "Documents" ) );
	QTabBar::addTab( QIcon::fromTheme( "folder" ), QString( "Downloads" ) );
	setFixedHeight( 30 );
	setIconSize( QSize( 24, 24 ) );

	setStyleSheet(
		QString(
			"QTabWidget::tab-bar {"
			"	alignment: center;"
			"}"
			"QTabBar::tab {"
			"	background: none;"
			"	border: none;"
			"	min-width: 8ex;"
			"	padding: 2px;"
			"	padding-left: 5px;"
			"}"
			"QTabBar::tab:selected, QTabBar::tab:hover {"
			"	background: rgba( 255, 255, 255, 20 );"
			"}"
			"QTabBar::tab:selected {"
			"	border-color: #9B9B9B;"
			"}"
			"QTabBar::tab:first {"
			"	border-top-left-radius: 2px;"
			"	border-bottom-left-radius: 2px;"
			"}"
			"QTabBar::tab:last {"
			"	border-top-right-radius: 2px;"
			"	border-bottom-right-radius: 2px;"
			"}"
		)
	);
};

void NBTabs::paintEvent( QPaintEvent *pEvent ) {

	QTabBar::paintEvent( pEvent );

	QPainter *painter = new QPainter( this );
	painter->save();

	painter->setPen( Qt::gray );
	painter->drawRoundedRect( 0, 0, width() - 1, height() - 1, 2, 2 );

	painter->restore();
	painter->end();
};
