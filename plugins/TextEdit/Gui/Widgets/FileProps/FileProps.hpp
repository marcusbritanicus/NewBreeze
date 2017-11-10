/*
	*
	* FileProps.hpp - TextEditor's File Properties Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "QsciEditor.hpp"

class wFileProps: public QDialog {
    Q_OBJECT

	public:
		wFileProps( QsciEditor *parent );

	private :
		void createGUI();
		void setWindowProperties();

		QLabel *iconLbl, *nameLbl, *typeLbl, *sizeLbl;
		QLabel *locLbl, *time1Lbl, *time2Lbl, *time3Lbl;
		QLabel *linesLbl, *wordsLbl, *charsLbl;
		QString fileName;
		QString lines, words, chars;
		bool noFileProps;
};
