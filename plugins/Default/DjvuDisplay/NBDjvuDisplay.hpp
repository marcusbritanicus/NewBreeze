/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#ifndef NBDJVUDISPLAY_HPP
#define NBDJVUDISPLAY_HPP

#include <Global.hpp>
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

	private slots :
		void openInExternal();
		void loadDjvu();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

#endif
