/*
	*
	* NBAVPlayer.hpp - Peek plugin to preview Video and Audio
	*
*/

#pragma once

#include <QtGui>
#include <QtCore>

#if QT_VERSION > 0x050000
	#include <QtWidgets>
#endif

#include <mpv/client.h>

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

		QWidget *mpv_container;
		mpv_handle *mpv;

	public Q_SLOTS:
		int exec();
		void play( QString path );
		void handleMpvEvents();

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );

	Q_SIGNALS:
		void mpv_events();
};
