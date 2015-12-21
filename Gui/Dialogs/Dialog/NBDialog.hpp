/*
	*
	* NBDialog.hpp - NBDialog.cpp header
	*
*/

#pragma once
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

		NBDialog( QWidget *parent = 0 );
		NBDialog( QString btns = QString(), QWidget *parent = 0 );
		NBDialog( NBDialog::Buttons btns, QWidget *parent = 0 );

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
		void mouseDoubleClickEvent( QMouseEvent * );
		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );

		void paintEvent( QPaintEvent *pEvent );

	signals:
		void closed();
};

#endif
