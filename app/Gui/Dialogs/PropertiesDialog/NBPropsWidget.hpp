/*
	*
	* NBPropsWidget.hpp - NBPropsWidget.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBFileDialog.hpp"

class NBSizeRunner;
class NBDriveInfo;

class NBPropertiesWidget: public QWidget {
    Q_OBJECT

	public:
		NBPropertiesWidget( QStringList, QWidget * );
		~NBPropertiesWidget();

	public Q_SLOTS:
		void refreshSize();

	private:
		void createGUI();
		void setWidgetProperties();

		void update();

		QLabel *nameLbl;
		NBClickLabel *iconLbl;

		QLabel *sizeLbl;
		QLabel *typeLbl;
		QLabel *contLbl;

		QLabel *aDateLbl;
		QLabel *mDateLbl;
		QLabel *cDateLbl;

		NBDriveInfo *dInfo;
		NBSizeRunner *runner;

		QStringList pathsList;

	private slots:
		void update( qint64, qint64, qint64 );
		void changeDirIcon();
};

class NBSizeRunner : public QThread {
	Q_OBJECT

	public:
		NBSizeRunner( QStringList paths ) : QThread() {

			files = 0;
			folders = 0;
			totalSize = 0;

			mPaths << paths;
			mTerminate = false;
		};

		void stop() {

			mTerminate = true;
		};

	protected:
		void run();

	private:
		QStringList mPaths;
		bool mTerminate;

		qint64 files, folders, totalSize;

		void recurseProperties( QString path );

	Q_SIGNALS:
		void currentCount( qint64, qint64, qint64 );
};

class NBDriveInfo : public QWidget {
	Q_OBJECT

	public:
		NBDriveInfo( NBDeviceInfo, QWidget* );

	private:
		QString name;
		QString disk;
		QString icon;

		QString sizeInfo;

		float percent;

	protected:
		void paintEvent( QPaintEvent * );
};
