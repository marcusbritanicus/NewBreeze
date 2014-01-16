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
#if QT_VERSION >= 0x050000
	#include <QtConcurrent>
#endif

class NBPropertiesBase: public QWidget {
	Q_OBJECT

	public:
		NBPropertiesBase( QStringList );

	private:
		NBClickLabel *iconLabel;
		QLabel *infoLabel;
		QStringList pathsList;

	Q_SIGNALS:
		void setDirIcon();
};

class NBPropertiesWidget: public QWidget {
    Q_OBJECT

	public:
		NBPropertiesWidget( QStringList );
		~NBPropertiesWidget();

	private:
		void createGUI();
		void setWidgetProperties();

		bool terminate;

	public slots:
		void update();

	private:
		void folderProperties( QStringList paths );
		void recurseProperties( QString path );

		QString sizeTypeStr;
		QLabel *sizeTypeLabel;

		QLabel *timeLabel;

		QStringList pathsList;

		QFuture<void> thread;

		qint64 files = 0;
		qint64 folders = 0;
		qint64 totalSize = 0;

		QDateTime ctimeMin, mtimeMin, atimeMin;
		QDateTime ctimeMax, mtimeMax, atimeMax;

	Q_SIGNALS:
		void updateSignal();
};

class NBPermissionsWidget: public QWidget {
    Q_OBJECT

	public:
		NBPermissionsWidget( QStringList );

	private:
		void createGUI();
		void setWidgetProperties();
		void readPermissions();

		NBClickLabel *iconLabel;

		QStringList pathsList;

		QCheckBox *uReadCheck, *uWritCheck, *uExecCheck;
		QCheckBox *gReadCheck, *gWritCheck, *gExecCheck;
		QCheckBox *oReadCheck, *oWritCheck, *oExecCheck;

		QCheckBox *smartExecCheck;
		QCheckBox *carryCheck;

	private slots:
		void applyPermissions();
		void applyTo( const char*, QFile::Permissions );
};

void smartExecutable( QString );

/*
	*
	* We want a independent, non modal dialog showing properties,
	*
*/
class NBPropertiesDialog: public NBDialog {
	Q_OBJECT


	public:
		Q_ENUMS( PropertiesTab )

		enum PropertiesTab {
			Properties  = 0x00,
			Permissions = 0x01,
			OpenWith    = 0x02
		};

		NBPropertiesDialog( QStringList paths, PropertiesTab tab = Properties );

	private:
		QStackedWidget *stack;
		QTabBar *tabs;

		NBPropertiesBase *propsB;
		NBPropertiesWidget *propsW;
		NBPermissionsWidget *permsW;

		QStringList pathsList;

	private slots:
		void switchToTab( int  );
		void setDirIcon();
};

#endif
