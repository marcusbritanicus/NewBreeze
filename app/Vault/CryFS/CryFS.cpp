/*
	*
	* CryFS.cpp - CoreApps Folder Encryption Class
	*
*/

#include "CryFS.hpp"

#include <cprime/filefunc.h>

CryFS::CryFS( QString source, QString target, QWidget *parent ) : QObject( parent ) {

	/* Encrypted location */
	mSource = QString( source );

	/* Decrypted location */
	mTarget = QString( target );

	/* Store the parent */
	mParent = parent;
};

bool CryFS::mountDir( QString password ) {

	if ( not CPrime::FileUtils::exists( mTarget ) ) {
		if ( CPrime::FileUtils::mkpath( mTarget, 0700 ) ) {
			QMessageBox::critical(
				mParent,
				"CoreApps - Mount Falied",
				QString(
					"CoreApps failed to create the target directory <tt>%1</tt>. "
					"Please check if you have sufficient permission to create folders here and try again." ).arg( mTarget )
			);
			return false;
		}
	}

	QProcess proc;
	QProcessEnvironment pe = proc.processEnvironment();
	pe.insert( "CRYFS_FRONTEND", "noninteractive" );
	proc.setProcessEnvironment( pe );
	proc.start( "cryfs", { mSource, mTarget } );

	/* Wait a second for ready read. If we get nothing, then everything is fine. */
	proc.waitForReadyRead( 1000 );
	proc.write( ( password + "\n" ).toLocal8Bit() );
	password.clear();
	proc.waitForFinished( -1 );

	QString text = proc.readAllStandardOutput() + proc.readAllStandardError() + proc.readAll();

	if ( proc.exitCode() ) {
		qDebug() << text;
		QMessageBox::critical(
			mParent,
			"CoreApps CryFS - Mount Failed",
			"CoreApps falied to mount the given CryFS volume. Please check if you have provided the correct password."
		);

		return false;
	}

	else {
		QMessageBox::information(
			mParent,
			"CoreApps - Mount Success",
			"The encrypted directory has been successfully decrypted and mounted."
		);

		return true;
	}
};

bool CryFS::unmountDir() {

	QString queryTxt =  QString(
		"<html><body><p>You have chosen to unmount and encrypt the archive which is mounted at</p>" \
		"<p><center><b><tt>%1</tt></b></center></p>" \
		"<p>To unmount, please save your work, and close all active processes in the archive and " \
		"click <b>Yes</b>.</p><p>To cancel the unmount, press <b>No</b>.</p></body></html>"
	).arg( mTarget );

	QMessageBox::StandardButtons btns = QMessageBox::No | QMessageBox::Yes;

	int reply = QMessageBox::question( mParent, "Unmount?", queryTxt, btns );

	if ( reply != QMessageBox::Yes )
		return false;

	QProcess proc;
	proc.start( "cryfs-unmount", QStringList() << mTarget );
	proc.waitForFinished( -1 );

	if ( proc.exitCode() == 0 ) {
		QMessageBox::information(
			mParent,
			"Unmount Success",
			"The decrypted directory has been successfully unmounted and encrypted."
		);

		return true;
	}

	else {
		QMessageBox::information(
			mParent,
			"Unmount Failed",
			"Could not unmount and encrypt the directory. This could be because some process may be active " \
			"in the archive. Please make sure no processes are active in the archive and then try unmounting."
		);

		return false;
	}
};

bool CryFS::createCryFS( QString passwd ) {

	/* Create a temporary locations */
	QString mTgtTmp = mTarget + QDateTime::currentDateTime().toString( ".yyyymmdd-hhmmss" );

	/* If the source exists, move it to the above temporary location */
	if ( CPrime::FileUtils::exists( mTarget ) and not QFile::rename( mTarget, mTgtTmp ) ) {
		QMessageBox::information(
			mParent,
			"CoreApps - CryFS Creation Error",
			QString( "" )
		);

		/* We could not create the CryFS volume properly */
		return false;
	}

	/* CryFS @mSource @mTarget */
	if ( not mSource.endsWith( "/ ") )
		mSource += "/";

	if ( not mTarget.endsWith( "/ ") )
		mTarget += "/";

	/* Create encrypted directory path */
	CPrime::FileUtils::mkpath( mSource, 0700 );

	/* Create decrypted directory path */
	CPrime::FileUtils::mkpath( mTarget, 0700 );

	QProcess proc;
	QProcessEnvironment pe = proc.processEnvironment();
	pe.insert( "CRYFS_FRONTEND", "noninteractive" );
	proc.setProcessEnvironment( pe );
	proc.setProcessChannelMode( QProcess::MergedChannels );
	proc.start( "cryfs", QStringList() << mSource << mTarget );

	/* Question: Input password */
	proc.write( ( passwd + "\n" ).toLocal8Bit().data() );

	/* Wait for the process to be finished */
	proc.waitForFinished( -1 );

	/* If there was an error: print it in the debug output */
	if ( proc.exitCode() ) {
		QString error = proc.readAll();
		qDebug() << error;
		QMessageBox::information(
			mParent,
			"CoreApps - CryFS Creation Falied",
			"<p>For some reason unknown to me the creation of the CryFS volume failed. Following error message was obtained:</p>" + error
		);

		/* We could not create the CryFS volume */
		return false;
	}

	/* Successfully created the directory, we add .directory entry */
	QSettings sett( QDir( CPrime::FileUtils::dirName( mSource ) ).filePath( ".directory" ), QSettings::NativeFormat );
	sett.setValue( "CryFS/" + CPrime::FileUtils::baseName( mSource ), CPrime::FileUtils::baseName( mTarget ) );

	/* If source existed, and had contents: */
	if ( CPrime::FileUtils::exists( mTgtTmp ) ) {

		int ret = system( ( "mv " + mTgtTmp + "/* " + mTarget ).toLocal8Bit().data() );
		if ( ret ) {
			QMessageBox::information(
				mParent,
				"CoreApps - Data Restore Failed",
				QString( "CoreApps has encountered error in restoring the data that existed in your target directory from the temporary location. "
				"You will find your data in the folder: %1" ).arg( mTgtTmp )
			);

			/* We created the CryFS volume properly, but data restore failed. */
			return true;
		}

		CPrime::FileUtils::removeDir( mTgtTmp );
	}

	/* Everything went on very well */
	return true;
};

CreateCryFS::CreateCryFS( QWidget *parent ) : QDialog( parent ) {

	createGUI();

//	QRect scrnSize = QDesktopWidget().screenGeometry();
//	int hpos = ( int )( ( scrnSize.width() - 700 ) / 2 );
//	int vpos = ( int )( ( scrnSize.height() - 400 ) / 2 );
//	move( hpos, vpos );

	setFixedSize( 450, 120 );
};

QString CreateCryFS::directoryName() {

	if ( result() == QDialog::Accepted )
		return dirLE->text();

	else
		return QString();
};

void CreateCryFS::createGUI() {

	QLabel *lbl = new QLabel( "Enter the name of the &directory:" );
	dirLE = new QLineEdit( this );

	lbl->setBuddy( dirLE );

	createBtn = new QPushButton( QIcon::fromTheme( "dialog-ok" ), "&Create CryFS", this );
	createBtn->setDisabled( true );
	connect( createBtn, SIGNAL( clicked() ), this, SLOT( accept() ) );

	QPushButton *cancelBtn = new QPushButton( QIcon::fromTheme( "dialog-close" ), "&Create CryFS", this );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );

	connect( dirLE, SIGNAL( textChanged( const QString& ) ), this, SLOT( updateButton() ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addStretch();
	btnLyt->addWidget( createBtn );
	btnLyt->addWidget( cancelBtn );

	QVBoxLayout *lyt = new QVBoxLayout();
	lyt->addWidget( lbl );
	lyt->addWidget( dirLE );
	lyt->addLayout( btnLyt );

	setLayout( lyt );
};

void CreateCryFS::updateButton() {

	if ( not dirLE->text().count() )
		createBtn->setDisabled( true );

	else
		createBtn->setEnabled( true );
};
