/*
	*
	* NBDialog.hpp - NBDialog.cpp header
	*
*/

#ifndef NBDIALOG_HPP
#define NBDIALOG_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBButtons.hpp>
#include <NBGuiWidgets.hpp>
#include <NBActionButtons.hpp>

class NBDialog : public QWidget {
	Q_OBJECT

	public:
		enum ButtonFlag {
			None     = 0x00,
			Minimize = 0x01,
			Maximize = 0x02,
			Close    = 0x04
		};

		Q_DECLARE_FLAGS( Buttons, ButtonFlag )

		NBDialog( QString btns = QString() );
		NBDialog( NBDialog::Buttons btns );

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
