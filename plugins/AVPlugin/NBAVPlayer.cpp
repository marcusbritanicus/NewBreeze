/*
	*
	* NBAVPlayer.cpp - Peek plugin to preview Video and Audio
	*
*/

#include "NBAVPlayer.hpp"

static void wakeup( void *ctx ) {

	NBAVPlayer *player = ( NBAVPlayer * )ctx;
	emit player->mpv_events();
}

NBAVPlayer::NBAVPlayer( QString pth ) : QDialog() {

	/* Video path */
	mPath = QString( pth );

	/* Create the UI */
	createGUI();
	setWindowProperties();

	peekWidgetBase->setFocus();
};

NBAVPlayer::~NBAVPlayer() {

	mpv_terminate_destroy( mpv );
	mpv = NULL;
};

void NBAVPlayer::createGUI() {

	QHBoxLayout *lblBtnLyt = new QHBoxLayout();
	QVBoxLayout *widgetLyt = new QVBoxLayout();
	QVBoxLayout *baseLyt = new QVBoxLayout();

	lbl = new QLabel( "<tt><b>" + mPath + "</b></tt>" );

	QToolButton *openBtn = new QToolButton();
	openBtn->setIcon( QIcon( ":/icons/maximize.png" ) );
	openBtn->setAutoRaise( true );
	openBtn->setFocusPolicy( Qt::NoFocus );

	QWidget *baseWidget = new QWidget();
	baseWidget->setObjectName( tr( "guiBase" ) );

	connect( openBtn, &QToolButton::clicked, this, &NBAVPlayer::openInExternal );

	peekWidgetBase = new QWidget();
	peekWidgetBase->setObjectName( tr( "previewBase" ) );

	setlocale( LC_NUMERIC, "C" );

	mpv = mpv_create();
	if ( !mpv ) {
		throw std::runtime_error( "failed to create mpv handle" );
	}

	mpv_container->setAttribute( Qt::WA_DontCreateNativeAncestors );
	mpv_container->setAttribute( Qt::WA_NativeWindow );

	int64_t wid = peekWidgetBase->winId();
	mpv_set_option( mpv, "wid", MPV_FORMAT_INT64, &wid );

	mpv_set_option_string( mpv, "input-default-bindings", "yes" );
	mpv_set_option_string( mpv, "input-vo-keyboard", "yes" );
	mpv_set_option_string( mpv, "osc", "yes" );

	mpv_observe_property( mpv, 0, "time-pos", MPV_FORMAT_DOUBLE );
	mpv_observe_property( mpv, 0, "track-list", MPV_FORMAT_NODE );
	mpv_observe_property( mpv, 0, "chapter-list", MPV_FORMAT_NODE );

	connect( this, SIGNAL( mpv_events() ), this, SLOT( handleMpvEvents() ), Qt::QueuedConnection );
	mpv_set_wakeup_callback( mpv, wakeup, this );

	if ( mpv_initialize( mpv ) < 0 )
		throw std::runtime_error( "mpv failed to initialize" );

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

void NBAVPlayer::setWindowProperties() {

	setFixedSize( 720, 540 );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 540 ) / 2 );

	setGeometry( hpos, vpos, 720, 540 );

	setWindowFlags( windowFlags() | Qt::FramelessWindowHint );
};

void NBAVPlayer::loadMedia() {

	const char *args[] = { "loadfile", mPath.toUtf8().data(), NULL };
	mpv_command_async( mpv, 0, args );
};

void NBAVPlayer::stop() {

};

void NBAVPlayer::handleMpvEvents() {

};

void NBAVPlayer::play( QString ) {

};

int NBAVPlayer::exec() {

	/* Load the video */
	loadMedia();

	/* Show the dialog */
	return QDialog::exec();
};

void NBAVPlayer::openInExternal() {

	QProcess::startDetached( "xdg-open " + mPath );
	close();
};

void NBAVPlayer::keyPressEvent( QKeyEvent *kEvent ) {

	/* No Modifiers */
	if ( qApp->keyboardModifiers().testFlag( Qt::NoModifier ) ) {
		switch ( kEvent->key() ) {
			case Qt::Key_Escape: {
				stop();
				close();
				break;
			}

			default: {
				QWidget::keyPressEvent( kEvent );
			}
		}
	}
};

void NBAVPlayer::changeEvent( QEvent *event ) {

	if ( ( event->type() == QEvent::ActivationChange ) and ( !isActiveWindow() ) ) {
		stop();
		close();
		event->accept();
	}

	else {
		QDialog::changeEvent( event );
		event->accept();
	}
};

void NBAVPlayer::paintEvent( QPaintEvent *pEvent ) {

	QWidget::paintEvent( pEvent );
	QPainter *painter = new QPainter( this );

	painter->setPen( QPen( palette().color( QPalette::Window ).darker(), 2.0 ) );
	painter->drawRect( rect().adjusted( +1, +1, -1, -1 ) );

	painter->end();
	pEvent->accept();
};
