/*
	*
	* NBMessageDialog.hpp - NBMessageDialog.cpp header
	*
*/

#pragma once
#ifndef NBMESSAGEDIALOG_HPP
#define NBMESSAGEDIALOG_HPP

#include <Global.hpp>
#include <NBSegmentControl.hpp>
#include <NBDialog.hpp>

class NBMessageDialog : public NBDialog {
	Q_OBJECT

	public:
		enum StandardButton {
			Ok                 = 0x00000400,
			Yes                = 0x00004000,
			YesToAll           = 0x00008000,
			No                 = 0x00010000,
			NoToAll            = 0x00020000,
			Abort              = 0x00040000,
			Retry              = 0x00080000,
			Ignore             = 0x00100000,
			Cancel             = 0x00400000,
		};

		NBMessageDialog( QString icon = "", QString title = "", QString text = "",
				QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *info = 0, QWidget *parent = 0 );
		void setIcon( QString );
		void setTitle( QString );
		void setText( QString );
		void setButtons( QList<StandardButton> );

		static int information( QWidget*, QString, QString, QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *infoWidget = 0 );
		static int question( QWidget*, QString, QString, QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *infoWidget = 0 );
		static int warning( QWidget*, QString, QString, QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *infoWidget = 0 );
		static int error( QWidget*, QString, QString, QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *infoWidget = 0 );
		static int critical( QWidget*, QString, QString, QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *infoWidget = 0 );

		int retValue;

	private:
		QLabel *iconLbl, *textLbl;
		NBSegmentControl *segBtns;
		NBSegmentControl *detailsBtn;
		QWidget *infoWidget;

		QByteArray origGeometry;

	protected:
		void keyPressEvent( QKeyEvent *keyEvent );

	public slots:
		void close();

	private slots:
		void toggleInfoWidget();
		void buttonClickHandler( int btn );

	signals:
		void closed();
};

#endif
