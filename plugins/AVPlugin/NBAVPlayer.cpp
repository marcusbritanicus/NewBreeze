/*
	*
	* NBAVPlayer.cpp - Peek plugin to preview Video and Audio
	*
*/

#include "NBAVPlayer.hpp"

NBAVPlayer::NBAVPlayer( QString pth ) : QDialog() {

	/* Video path */
	mPath = QString( pth );

	/* Is video playing */
	mIsPlaying = false;

	/* Load the VLC engine */
	inst = libvlc_new( 0, NULL );

	/* Create the UI */
	createGUI();
	setWindowProperties();

	peekWidgetBase->setFocus();
};

NBAVPlayer::~NBAVPlayer() {

	/* Stop the player first */
	stop();

	/* Free the media_player */
	libvlc_media_player_release( mp );

	/* Release the instance */
	libvlc_release( inst );
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

	connect( openBtn, SIGNAL( clicked() ), this, SLOT( openInExternal() ) );

	peekWidgetBase = new QWidget();
	peekWidgetBase->setObjectName( tr( "previewBase" ) );

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

	/* Create a new item */
	m = libvlc_media_new_path( inst, mPath.toLocal8Bit().data() );

	/* Create a media player playing environment */
	mp = libvlc_media_player_new_from_media( m );

	/* Free the media */
	libvlc_media_release( m );

	/* Attach the current window to our player */
	libvlc_media_player_set_xwindow( mp, peekWidgetBase->winId() );

	/* Start playing the media */
	togglePausePlay();
};

void NBAVPlayer::togglePausePlay() {

	if ( mIsPlaying ) {
		/* Is video playing */
		mIsPlaying = false;

		/* pause the media_player */
		libvlc_media_player_pause( mp );
	}

	else {
		/* Is video playing */
		mIsPlaying = true;

		/* play the media_player */
		libvlc_media_player_play( mp );
	}
};

void NBAVPlayer::stop() {

	if ( mIsPlaying ) {
		/* Stop playing */
		libvlc_media_player_stop( mp );

		/* Is video playing */
		mIsPlaying = false;
	}
};

void NBAVPlayer::seek( int ms, bool forward ) {

	int curtime = libvlc_media_player_get_time( mp );

	libvlc_media_player_pause( mp );

	if ( forward ) {
		int newtime = curtime + ms;
		libvlc_media_player_set_time( mp, newtime );
	}

	else {
		int newtime = curtime - ms;
		if ( newtime < 0 )
			libvlc_media_player_set_time( mp, 0 );

		else
			libvlc_media_player_set_time( mp, newtime );
	}

	libvlc_media_player_play( mp );
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
			case Qt::Key_Right: {
				seek( 10000, true );
				break;
			}

			case Qt::Key_Left: {
				seek( 10000, false );
				break;
			}

			case Qt::Key_Space: {
				togglePausePlay();
				break;
			}

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

	/* Control Modifiers */
	else if ( qApp->keyboardModifiers().testFlag( Qt::ControlModifier ) ) {
		switch ( kEvent->key() ) {
			case Qt::Key_Right: {
				seek( 60000, true );
				break;
			}

			case Qt::Key_Left: {
				seek( 60000, false );
				break;
			}

			case Qt::Key_Space: {
				stop();
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

int main( int argc, char **argv ) {

	QApplication app( argc, argv );
	NBAVPlayer Gui( argv[ 1 ] );
	Gui.show();

	return 0;
};
