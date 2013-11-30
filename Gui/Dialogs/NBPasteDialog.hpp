/*
	*
	* NBPasteDialog.hpp - NewBreeze PasteDialog Class Header
	*
*/

#ifndef NBPASTEDIALOG_HPP
#define NBPASTEDIALOG_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>
#include <NBDialog.hpp>
#include <NBMessageDialog.hpp>
#include <NBIconProvider.hpp>
#include <NBDeviceInfo.hpp>

class NBPasteDialog : public NBDialog {
	Q_OBJECT

	public:
		NBPasteDialog();

		QStringList sources();
		void setSources( QStringList );

		QString target();
		void setTarget( QString );

		NBIOMode::Mode ioMode();
		void setIOMode( NBIOMode::Mode iomode = NBIOMode::Copy );

		qreal progress();
		QStringList errorNodes();

	private:
		void createGUI();
		void createAndSetupActions();
		void setWindowProperties();

		QLabel *nameLbl, *fromToLbl;
		QProgressBar *totalPB, *filePB;
		QPushButton *pauseBtn, *cancelBtn;

		QString srcFile, tgtFile, tgtDir, srcDir;
		NBIOMode::Mode mode;

		QTimer *progressCheckTimer;

		QStringList srcList;
		quint64 totalSize, copiedSize;
		bool wasCanceled;
		bool isPaused;

		QStringList errors;

	public slots:
		void startWork();

	private slots:
		void preCopy();
		void performIO();
		void createDirTree();
		void copyFile();

		void togglePauseResume();
		void setCanceled();

		void handleFinished( int, QProcess::ExitStatus );

		void setProgress();

	Q_SIGNALS:
		void IOComplete();
};

#endif
