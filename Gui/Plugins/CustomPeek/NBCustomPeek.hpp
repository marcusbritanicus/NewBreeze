/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#ifndef NBCUSTOMPEEK_HPP
#define NBCUSTOMPEEK_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>
#include <NBIconProvider.hpp>

class NBCustomPeek : public QWidget {
	Q_OBJECT

	public :
		NBCustomPeek( QString path = QDir::homePath() );
		QString path;
		QWidget *peekWidgetBase;
		QLabel *iconLbl, *nameLbl, *pathLbl, *mimeLbl, *sizeLbl, *timeLbl, *ugidLbl;

	private :
		void createGUI();
		void setWindowProperties();
		void getFileProps();

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );
};

#endif
