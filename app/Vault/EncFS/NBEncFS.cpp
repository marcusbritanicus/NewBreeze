/*
	*
	* NBEncFS.cpp - NewBreeze File Folder Decryption Class
	*
*/

#include "NBEncFS.hpp"

NBEncFS::NBEncFS( QString source, QString target, QWidget *parent ) : QObject( parent ) {

	mSource = QString( source );
	mTarget = QString( target );

	/* Store the parent */
	mParent = parent;
};

bool NBEncFS::mountDir( QString password ) {

	if ( not exists( mTarget ) ) {
		if ( mkpath( mTarget ) ) {
			QMessageBox::critical(
				mParent,
				"NewBreeze - Mount Falied",
				QString(
					"NewBreeze failed to create the target directory <tt>%1</tt>. "
					"Please check if you sufficient permission to create folders here and try again." ).arg( mTarget )
			);
			return false;
		}
	}

	QProcess proc;
	proc.start( "encfs", QStringList() << "-S" << mSource << mTarget );

	/* Wait a second for ready read. If we get nothing, then everything is fine. */
	proc.waitForReadyRead( 1000 );
	QString text = proc.readAllStandardOutput() + proc.readAllStandardError() + proc.readAll();

	if ( not text.count() ) {

		proc.write( password.toLocal8Bit().data() );
		proc.write( "\n" );
		password.clear();
		proc.waitForFinished();

		if ( proc.exitCode() ) {
			QMessageBox::critical(
				mParent,
				"NewBreeze EncFS - Mount Failed",
				"NewBreeze falied to mount the given encfs volume. Please check if you have provided the correct password."
			);

			return false;
		}

		else {
			QMessageBox::information(
				mParent,
				"NewBreeze - Mount Success",
				"The encrypted directory has been successfully decrypted and mounted."
			);

			return true;
		}
	}

	else {
		proc.terminate();
		QMessageBox::information(
			mParent,
			"NewBreeze EncFS - Mount Failed",
			"For some reason encfs has failed to mount the encrypted folder. Perhaps, the source folder was not an encryted volume. "
			"Check the terminal for the debug messages for the text printed out by encfs."
		);

		return false;
	}
};

bool NBEncFS::unmountDir() {

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
	proc.start( "fusermount", QStringList() << "-uz" << mTarget );
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

bool NBEncFS::createEncFS( QString passwd ) {

	QString mSrcTmp;

	if ( not mSource.endsWith( "/ ") )
		mSource += "/";

	if ( not mTarget.endsWith( "/ ") )
		mTarget += "/";

	/* If @mTarget exists, copy data to a temporary location */
	if ( exists( mTarget ) and nChildren( mTarget ) ) {
		QMessageBox *mBox = new QMessageBox( mParent );
		QString title = "NewBreeze - Create EncFS";
		QString message = QString( "Save all open files and click 'Continue' to proceed." );
		QString details = QString(
			"To create an EncFS volume, the folder %1 should be empty. I will attempt to move the data to a temporary location, "
			"then create the volume and copy the data back. Please close all the open files in <tt>%1</tt> and click "
			"'Continue' to proceed. You may click 'Abort' to cancel the creation of the encrypted volume."
		).arg( baseName( mSource ) );

		QPushButton *abortBtn = new QPushButton( QIcon::fromTheme( "dialog-cancel" ), "&Abort", mBox );
		connect( abortBtn, SIGNAL( clicked() ), mBox, SLOT( reject() ) );

		QPushButton *continueBtn = new QPushButton( QIcon::fromTheme( "dialog-ok-apply" ), "&Continue", mBox );
		connect( continueBtn, SIGNAL( clicked() ), mBox, SLOT( accept() ) );

		mBox->setWindowTitle( title );
		mBox->setText( message );
		mBox->setDetailedText( details );
		mBox->setIcon( QMessageBox::Information );

		mBox->addButton( abortBtn, QMessageBox::RejectRole );
		mBox->addButton( continueBtn, QMessageBox::AcceptRole );

		mBox->setDefaultButton( continueBtn );
		mBox->exec();

		if ( mBox->clickedButton() != continueBtn )
			return false;

		mSrcTmp = mSource + ".tmp" + QDateTime::currentDateTime().toString( "yyyyMMddThhmmssAP" );
		if ( rename( mSource.toLocal8Bit().data(), mSrcTmp.toLocal8Bit().data() ) != 0 ) {
			QMessageBox::critical(
				mParent,
				"NewBreeze - EncFS Creation Falied",
				"I have failed to create a new EncFS volume. Please check if you have read write permissions in the directory and try again."
			);

			return false;
		}
	}

	/* Create encrypted directory path */
	mkpath( mSource, 0700 );

	/* Create decrypted directory path */
	mkpath( mTarget, 0700 );

	QProcess proc;
	proc.setProcessChannelMode( QProcess::MergedChannels );
	proc.start( "encfs", QStringList() << "-S" << mSource << mTarget );

	/* Question 1: standard mode, expert mode (x) or paranoia mode (p): p */
	proc.write( "p\n" );

	/* Question 2: Input password */
	proc.write( ( passwd + "\n" ).toLocal8Bit().data() );

	/* Question 3: Reinput password */
	proc.write( ( passwd + "\n" ).toLocal8Bit().data() );

	/* Wait for the process to be finished */
	proc.waitForFinished( -1 );

	if ( proc.exitCode() ) {
		qDebug() << "encfs -S " + mSource + " " + mTarget;
		qDebug() << proc.readAll();
		QMessageBox::information(
			mParent,
			"NewBreeze - EncFS Creation Falied",
			"For some reason unknown to me the creation of the encfs volume failed. Please check the debug messages to identify the error."
		);

		/* We could not create the EncFS volume */
		return false;
	}

	QSettings sett( QDir( dirName( mSource ) ).filePath( ".directory" ), QSettings::NativeFormat );
	sett.setValue( "EncFS/" + baseName( mSource ), baseName( mTarget ) );
	QMessageBox::information(
		mParent,
		"NewBreeze - EncFS Creation Success",
		"The EncFS volume has been successfully created and mounted. To unmount the volume, right click and press '<u>U</u>nmount EncFS Volume'."
	);

	/* If source existed, and had contents: */
	if ( mSource.count() and ( mSource == mTarget ) ) {

		int ret = system( ( "mv " + mSrcTmp + "/* " + mSource ).toLocal8Bit().data() );
		if ( ret ) {
			QMessageBox::information(
				mParent,
				"NewBreeze - Data Restore Failed",
				QString( "NewBreeze has encountered error in restoring the data that existed in your target directory from the temporary location. "
				"You will find your data in the folder: %1" ).arg( mSrcTmp )
			);

			/* We created the EncFS volume properly, but data restore failed. */
			return true;
		}

		removeDir( mSrcTmp );
	}

	/* Everything went on very well */
	return true;
};

bool NBEncFS::changePass( QString oPassword, QString nPassword ) {

	QString command( "printf '%1\n%2\n' | encfsctl autopasswd %3" );
	int exitCode = system( command.arg( oPassword ).arg( nPassword ).arg( mSource ).toLocal8Bit().constData() );

	if ( not exitCode ) {
		QMessageBox::information(
			mParent,
			"NewBreeze - Success",
			"The password has been updated successfully. You may now mount the directory using the new password."
		);

		return true;
	}

	else {
		QMessageBox::information(
			mParent,
			"NewBreeze - Failed",
			"The password has not been altered. This is likely due to entering incorrect current password or "
			"failing to verify the new password."
		);

		return false;
	}
};
