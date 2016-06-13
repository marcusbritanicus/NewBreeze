/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include <libdjvu/ddjvuapi.h>

class NBDjvuDisplay : public QDialog {
	Q_OBJECT

	public :
		NBDjvuDisplay( QString path = QDir::homePath() );
		QString path;
		QScrollArea *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

		QVBoxLayout *djvuLyt;

	public slots:
		int exec();

	private slots :
		void openInExternal();
		void loadDjvu();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};
