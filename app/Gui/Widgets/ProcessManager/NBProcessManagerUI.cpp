/*
	*
	* NBProcessManagerUI.cpp - The NBProcessManager UI class for NewBreeze
	*
*/

#include "NBProcessManagerUI.hpp"

int NBProcessManagerUI::procCount = 0;
NBProcessManagerUI* NBProcessManagerUI::procMgrUI = NULL;

NBProcessManagerUI* NBProcessManagerUI::instance() {

	if ( procMgrUI != NULL )
		return procMgrUI;

	procMgrUI = new NBProcessManagerUI();

	return procMgrUI;
};

NBProcessManagerUI::NBProcessManagerUI() : NBDialog() {

	pMgr = NBProcessManager::instance();
	connect( pMgr, SIGNAL( processAdded( NBProcess::Progress*, NBAbstractProcess* ) ), this, SLOT( addProcess( NBProcess::Progress*, NBAbstractProcess*) ) );

	createGUI();
	setWindowProperties();
};

void NBProcessManagerUI::createGUI() {

	QScrollArea *scroller = new QScrollArea();
	scroller->setWidgetResizable( true );

	QWidget *baseWidget = new QWidget( this );
	baseLyt = new QVBoxLayout();
	baseLyt->setContentsMargins( QMargins( 5, 5, 5, 5 ) );

	if ( pMgr->processCount() ) {
		for ( quint64 i = pMgr->processCount() - 1; i >= 0; i-- ) {

			NBProcessWidget *procWidget = new NBProcessWidget( pMgr->progress( i ), pMgr->process( i ), this );
			baseLyt->addWidget( procWidget );
		}
	}

	baseLyt->addStretch( 0 );

	baseWidget->setLayout( baseLyt );
	scroller->setWidget( baseWidget );

	QHBoxLayout *scrollLyt = new QHBoxLayout();
	scrollLyt->setContentsMargins( QMargins() );
	scrollLyt->addWidget( scroller );

	setLayout( scrollLyt );
};

void NBProcessManagerUI::setWindowProperties() {

	setWindowModality( Qt::NonModal );

	setDialogTitle( "NewBreeze Process Manager" );
	setDialogIcon( QIcon( ":/icons/newbreeze.png" ) );

	setMinimumSize( QSize( 800, 500 ) );
};

void NBProcessManagerUI::show() {

	if ( not procCount ) {
		NBMessageDialog::information(
			NULL,
			"NB Process Manager",
			"There are no processes running at the time."
		);

		return;
	}

	NBDialog::show();
};

void NBProcessManagerUI::addProcess( NBProcess::Progress* progress, NBAbstractProcess *process ) {

	procCount++;
	NBProcessWidget *procWidget = new NBProcessWidget( progress, process, this );
	baseLyt->insertWidget( 0, procWidget );
};

NBProcessManagerMini::NBProcessManagerMini( QWidget *parent ) : QToolButton( parent ) {

	/* Local instances for the ProcessManagers */
	pMgr = NBProcessManager::instance();
	procMgrUI = NBProcessManagerUI::instance();

	/* Progress fraction */
	fraction = 0;
	/* Number of active jobs */
	jobs = NBProcessManager::instance()->activeProcessCount();

	/* Size */
	setFixedSize( QSize( 32, 32 ) );

	/* Bold font */
	setFont( QFont( font().family(), 10, QFont::Bold ) );

	/* We update only once a second */
	if ( jobs )
		timer.start( 1000, this );

	connect( pMgr, SIGNAL( activeProcessCountChanged( quint64 ) ), this, SLOT( updateTimer() ) );
	connect( this, SIGNAL( clicked() ), this, SLOT( toggleProcessManager() ) );
};

void NBProcessManagerMini::showProcessManager() {

	if ( not procMgrUI->isVisible() )
		procMgrUI->show();

	procMgrUI->raise();
	procMgrUI->activateWindow();

	repaint();
};

void NBProcessManagerMini::toggleProcessManager() {

	if ( not procMgrUI->isVisible() )
		showProcessManager();

	else
		procMgrUI->hide();

	repaint();
};

void NBProcessManagerMini::updateTimer() {

	if ( not timer.isActive() )
		timer.start( 1000, this );
};

void NBProcessManagerMini::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == timer.timerId() ) {
		jobs = pMgr->activeProcessCount();

		if ( not jobs ) {
			timer.stop();
			repaint();

			return;
		}

		fraction = 0;

		Q_FOREACH( NBProcess::Progress *prog, pMgr->activeProgresses() )
			fraction += 1.0 * prog->totalBytesCopied / prog->totalBytes;

		fraction /= jobs;

		repaint();
	}

	else {

		QToolButton::timerEvent( tEvent );
	}
};

void NBProcessManagerMini::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );
	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );

	painter->setPen( Qt::NoPen );

	/* A background color indicating 'checked' status */
	if ( procMgrUI->isVisible() ) {

		painter->setBrush( QColor( 0, 0, 0, 90 ) );
		painter->drawRoundedRect( 4, 4, 24, 24, 5, 5 );
	}

	if ( jobs ) {
		int red, green;

		// Change from Red to Yellow: When fraction = 0, green = 0; fraction = 0.4
		if ( fraction <= 0.4 ) {
			red = ( int )( 255 );
			green = ( int )( fraction * 638 );
		}

		// Remain Yellow
		else if ( fraction <= 0.6 ) {
			red = 255;
			green = 255;
		}

		// Change from Yellow to Green: When fraction = 0.6, red = 255; fraction = 1, red = 0;
		else {
			red = ( int )( ( 1 - fraction ) * 638 );
			green = ( int )( 255 );
		}

		/* Draw the total progress indicator */
		painter->setPen( Qt::NoPen );
		painter->setBrush( QColor( red, green, 0, 56 ) );
		painter->drawRoundedRect( 4, 4, 24 * fraction, 24, 5, 5 );

		/* Draw the border */
		painter->setBrush( Qt::NoBrush );
		painter->setPen( Qt::gray );
		painter->drawRoundedRect( 4, 4, 24, 24, 5, 5 );

		/* Draw text indicating total active jobs */
		painter->setPen( Qt::black );
		painter->drawText( 0, 0, 32, 32, Qt::AlignCenter, QString( "%1" ).arg( jobs ) );
	}

	else {
		/* Draw the pixmap */
		painter->setBrush( Qt::NoBrush );
		painter->setPen( Qt::gray );
		painter->drawRoundedRect( 4, 4, 24, 24, 5, 5 );

		painter->setPen( Qt::black );
		painter->setFont( QFont( font().family(), 10, QFont::Bold ) );
		painter->drawText( 0, 0, 32, 32, Qt::AlignCenter, QString::fromUtf8( "\u2139" ) );
	}

	painter->end();
	pEvent->accept();
};
