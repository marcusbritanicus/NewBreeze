/*
	*
	* NBDialog.hpp - NBDialog.cpp header
	*
*/

#ifndef NBDIALOG_HPP
#define NBDIALOG_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>
#include <NBActionButtons.hpp>

class NBDialog : public QWidget {
	Q_OBJECT

	public:
		NBDialog( QString btns = QString() );

		void setLayout( QLayout* );
		void setDialogTitle( QString );
		void setDialogIcon( QIcon );

		bool result();

	private:
		void setupGUI();
		void setupActions();
		void setupDialogProperties();

		NBActionButtons *actBtns;
		QLabel *titleLbl;
		QString buttons;

		QFrame *BodyFrame;
		QPoint dragPosition;

		bool __newClosed;
		bool __accepted;

	public slots:
		void done( int );
		void close();
		void show();
		void exec();

		void accept();
		void reject();

	private slots:
		void maxBtnClicked();

	protected:
		void keyPressEvent( QKeyEvent *keyEvent );

		void mouseDoubleClickEvent( QMouseEvent * );
		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );
		virtual void closeEvent( QCloseEvent *cEvent );

	signals:
		void closed();
};

#endif
