/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#ifndef NBIMAGEPEEK_HPP
#define NBIMAGEPEEK_HPP

#include <Global.hpp>

class NBImagePeek : public QDialog {
	Q_OBJECT

	public :
		NBImagePeek( QString path = QDir::homePath() );
		QString path;
		QLabel *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();
		void loadImage();

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

#endif
