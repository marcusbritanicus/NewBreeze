/*
	*
	* NewBreeze.hpp - NewBreeze Header
	*
*/

#ifndef NBKEYBINDER_HPP
#define NBKEYBINDER_HPP

#include <Global.hpp>

class KeyButton : public QPushButton {
	Q_OBJECT

	public:
		KeyButton( QString text );
};

class KeyLabel : public QLabel {
	Q_OBJECT

	public:
		KeyLabel();
};

class NBKeyBinder : public QDialog {
	Q_OBJECT

	public:
		NBKeyBinder();
		QString keyBinding();

		KeyLabel *labelKeys;
		KeyButton *CtrlBtn, *AltBtn, *ShiftBtn, *MetaBtn;
		QPushButton *cancelBtn, *okBtn;

	private:
		void createGui();
		void setupConnections();
		void setWindowProperties();

	protected :
		void keyPressEvent(  QKeyEvent * );

	private slots:
		void finish();
};

#endif
