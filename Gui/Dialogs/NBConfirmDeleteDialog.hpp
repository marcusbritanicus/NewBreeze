/*
	*
	* NBConfirmDeleteDialog.hpp - NBConfirmDeleteDialog.cpp header
	*
*/

#ifndef NBCONFIRMDELETEDIALOG_HPP
#define NBCONFIRMDELETEDIALOG_HPP

#include <Global.hpp>
#include <NBDialog.hpp>

class NBConfirmDeleteDialog : public NBDialog {
	Q_OBJECT

	public:
		NBConfirmDeleteDialog( QStringList, bool permanent = false );
		void setTitle( QString );
		void setButtons( QStringList );

	private:
		QLabel *iconLbl, *textLbl;
		QPushButton *deleteBtn, *cancelBtn, *detailsBtn;
		QTableWidget *table;
		QStringList deletePaths;

		void setupGUI( bool );
		void setupTable();
		void addEntry( QString );

		bool deleteFiles;

	protected:
		void keyPressEvent( QKeyEvent *keyEvent );

	public slots:
		void close();
		int exec();

	private slots:
		void deleteOk();
		void deleteCancel();
		void handleDetailsBtnClicked();

	signals:
		void closed();
};

#endif
