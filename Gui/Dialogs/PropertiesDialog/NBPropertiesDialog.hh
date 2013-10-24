/*
	*
	* NBPropertiesDialog.hpp - NBPropertiesDialog.cpp header
	*
*/

#ifndef NBPROPERTIES_HPP
#define NBPROPERTIES_HPP

#include <Global.hpp>
#include <NBDialog.hpp>
#include <NBShowHideWidget.hpp>
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
		void setWindowProperties();

		void readPermissions();
		bool terminate;

	public slots:
		void accept();
		void update();

		void combosChanged();
		void checkboxesChanged();
		void numericChanged(QString);

		void resizeDialog();

	signals:
		void updateSignal();

	private:
		void folderProperties( QStringList paths );
		void recurseProperties( QString path );

		NBShowHideWidget* getGeneralInfoWidget();
		NBShowHideWidget* getPermissionsWidget();
		NBShowHideWidget* getDeviceInfoWidget();

		NBShowHideWidget *fileSHWidget;
		NBShowHideWidget *permSHWidget;
		NBShowHideWidget *drivSHWidget;

		NBClickLabel *iconLabel;

		QLabel *size2Label;
		QLabel *cont2Label;

		QLineEdit *permissionsNumeric;

		QCheckBox *ownerRead;
		QCheckBox *ownerWrite;
		QCheckBox *ownerExec;

		QCheckBox *groupRead;
		QCheckBox *groupWrite;
		QCheckBox *groupExec;

		QCheckBox *otherRead;
		QCheckBox *otherWrite;
		QCheckBox *otherExec;

		QComboBox *ownerCombo;
		QComboBox *groupCombo;
		QComboBox *otherCombo;

		QCheckBox *execCheck;

		QStringList pathsList;
		QString permString;
		bool iconChanged;

		QFuture<void> thread;

		qint64 files;
		qint64 folders;
		qint64 totalSize;

	private slots:
		void setDirIcon();
};

#endif
