/*
	*
	* NBProgressDialog.hpp - NBProgressDialog.cpp header
	*
*/

#ifndef NBPROGRESSDIALOG_HPP
#define NBPROGRESSDIALOG_HPP

#include <Global.hpp>
#include <NBDialog.hpp>

class NBProgressDialog : public NBDialog {
	Q_OBJECT

	public:
		NBProgressDialog();

		void setIOMode( char );
		void setSource( QString );
		void setTarget( QString );

	private:
		QLabel *sourceLbl;
		QLabel *targetLbl;
		QLabel *curfileLbl;
		QLabel *speedLbl;

		QProgressBar *totalBar;
		QProgressBar *cfileBar;

		QPushButton *closeBtn;
		QPushButton *cancelBtn;

		bool loner;

	public slots:
		void update( QString, float, float );
		void cancelIO();
		void hide();

	Q_SIGNALS:
		void closed();
};

#endif
