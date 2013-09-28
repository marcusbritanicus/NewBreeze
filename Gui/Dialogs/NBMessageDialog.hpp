/*
	*
	* NBMessageDialog.hpp - NBMessageDialog.cpp header
	*
*/

#ifndef NBMESSAGEDIALOG_HPP
#define NBMESSAGEDIALOG_HPP

#include <Global.hpp>
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
				QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *info = 0 );
		void setIcon( QString );
		void setTitle( QString );
		void setText( QString );
		void setButtons( QStringList );

		static void aboutNewBreeze();
		static void aboutQt();
		static int information( QString, QString, QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *infoWidget = 0 );
		static int question( QString, QString, QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *infoWidget = 0 );
		static int warning( QString, QString, QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *infoWidget = 0 );
		static int error( QString, QString, QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *infoWidget = 0 );
		static int critical( QString, QString, QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok, QWidget *infoWidget = 0 );

		int retValue;

	private:
		QLabel *iconLbl, *textLbl;
		QPushButton *detailsBtn;
		QWidget *infoWidget;

		QByteArray origGeometry;

	protected:
		void keyPressEvent( QKeyEvent *keyEvent );

	public slots:
		void close();
		void show();
		void exec();

	private slots:
		void toggleInfoWidget();
		void buttonClickHandler();

	signals:
		void closed();
};

#endif
