/*
	*
	* NBConfirmDeleteDialog.hpp - NBConfirmDeleteDialog.cpp header
	*
*/

#ifndef NBCONFIRMDELETEDIALOG_HPP
#define NBCONFIRMDELETEDIALOG_HPP

#include <Global.hpp>
#include <NBDialog.hpp>
#include <NBGuiWidgets.hpp>
#include <NBTools.hpp>
#include <NBIconProvider.hpp>
#include <NBButtons.hpp>

class NBConfirmDeleteDialog : public NBDialog {
	Q_OBJECT

	public:
		NBConfirmDeleteDialog( QStringList, bool permanent = false );
		void setTitle( QString );
		void setButtons( QStringList );

	private:
		QLabel *iconLbl, *textLbl;
		NBButtons *segBtns;
		QPushButton *deleteBtn, *cancelBtn;
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

	signals:
		void closed();
};

#endif
