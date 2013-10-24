/*
	*
	* NBPropertiesDialog.hpp - NBPropertiesDialog.cpp header
	*
*/

#ifndef NBPROPERTIES_HPP
#define NBPROPERTIES_HPP

#include <Global.hpp>
#include <NBDialog.hpp>
#include <NBGuiWidgets.hpp>
#include <NBIconProvider.hpp>
#include <NBDeviceInfo.hpp>
#include <NBTools.hpp>
#include <NBFileDialog.hpp>

class NBPropertiesDialog: public NBDialog {
    Q_OBJECT

	public:
		NBPropertiesDialog( QStringList );
		~NBPropertiesDialog();

	private:
		void createGUI();
		void setDialogProperties();

		bool terminate;

	public slots:
		void update();

	signals:
		void updateSignal();

	private:
		void folderProperties( QStringList paths );
		void recurseProperties( QString path );

		NBClickLabel *iconLabel;

		QLabel *size2Label;
		QLabel *cont2Label;

		QStringList pathsList;

		QFuture<void> thread;

		qint64 files;
		qint64 folders;
		qint64 totalSize;

	private slots:
		void setDirIcon();
};

class NBPermissionsDialog: public NBDialog {
    Q_OBJECT

	public:
		NBPermissionsDialog( QStringList );

	private:
		void createGUI();
		void setDialogProperties();
		void readPermissions();

		NBClickLabel *iconLabel;

		QStringList pathsList;

		QCheckBox *uReadCheck, *uWritCheck, *uExecCheck;
		QCheckBox *gReadCheck, *gWritCheck, *gExecCheck;
		QCheckBox *oReadCheck, *oWritCheck, *oExecCheck;

		QCheckBox *smartExecCheck;
		QCheckBox *carryCheck;

		QPushButton *applyBtn, *cancelBtn;

	private slots:
		void applyPermissions();
		void applyTo( const char*, QFile::Permissions );
};

void smartExecutable( QString );

#endif
