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

class NBPasteDialog : public NBDialog {
	Q_OBJECT

	public:
		NBPasteDialog();
		void setSources( QStringList );
		void setTarget( QString );
		void setIOMode( NBIOMode::Mode iomode = NBIOMode::Copy );

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
		qint64 totalSize, copiedSize;
		bool wasCanceled;
		bool isPaused;

		QStringList errors;

	public slots:
		void show();

	private slots:
		void preCopy();
		void performIO();
		void createDirTree();
		void copyFile();

		void togglePauseResume();
		void setCanceled();

		void handleErrors();
		void handleFinished( int, QProcess::ExitStatus );

		void setProgress();

	Q_SIGNALS:
		void IOComplete();
};

#endif
