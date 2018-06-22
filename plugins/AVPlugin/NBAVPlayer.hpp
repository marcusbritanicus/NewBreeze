/*
	*
	* NBAVPlayer.hpp - Peek plugin to preview Video and Audio
	*
*/

#pragma once

#include <QtGui>
#include <QtCore>

#include <unistd.h>
#include <vlc/vlc.h>

#if QT_VERSION > 0x050000
	#include <QtWidgets>
#endif

class NBAVPlayer : public QDialog {
	Q_OBJECT

	public :
		NBAVPlayer( QString path = QDir::homePath() );
		~NBAVPlayer();

	private :
		void createGUI();
		void setWindowProperties();

		void loadMedia();

		void togglePausePlay();
		void stop();

		void seek( int ms, bool forward );

		QVBoxLayout *avLyt;
		QLabel *lbl;

		QString mPath;
		QWidget *peekWidgetBase;

		libvlc_instance_t * inst;
		libvlc_media_player_t *mp;
		libvlc_media_t *m;

		bool mIsPlaying;

	public Q_SLOTS:
		int exec();

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};
