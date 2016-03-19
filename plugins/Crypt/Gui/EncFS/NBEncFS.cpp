/*
	*
	* NBEncFS.cpp - NewBreeze File Folder Decryption Class
	*
*/

#include <NBEncFS.hpp>

NBEncFS::NBEncFS( QString source, QString target, QWidget *parent ) : QObject( parent ) {

	mSource = QString( source );
	mTarget = QString( target );

	/* Store the parent */
	mParent = parent;
};

void NBEncFS::mountDir() {

	if ( not exists( mTarget ) ) {
		if ( mkpath( mTarget ) ) {
			QMessageBox::critical(
				mParent,
				"NewBreeze - Mount Falied",
				QString(
					"NewBreeze failed to create the target directory <tt>%1</tt>. "
					"Please check if you sufficient permission to create folders here and try again." ).arg( mTarget )
			);
			return;
		}
	}

	QProcess proc;
	proc.start( "encfs", QStringList() << "-S" << mSource << mTarget );

	/* Wait a second for ready read. If we get nothing, then everything is fine. */
	proc.waitForReadyRead( 1000 );
	QString text = proc.readAllStandardOutput() + proc.readAllStandardError() + proc.readAll();

	if ( not text.count() ) {

		/* Get our Password Keypad */
		PasswordInput *pKeyPad = new PasswordInput( mParent );
		if ( pKeyPad->exec() == QDialog::Accepted ) {
			proc.terminate();
			return;
		}

		QString password = pKeyPad->password();
		pKeyPad->clear();

		proc.write( password.toLocal8Bit().data() );
		proc.write( "\n" );
		password.clear();
		proc.waitForFinished();

		if ( proc.exitCode() )
			QMessageBox::critical(
				mParent,
				"NewBreeze EncFS - Mount Failed",
				"NewBreeze falied to mount the given encfs volume. Please check if you have provided the correct password."
			);

		else
			QMessageBox::information(
				mParent,
				"NewBreeze - Mount Success",
				"The encrypted directory has been successfully decrypted and mounted."
			);
	}

	else {
		proc.terminate();
		qDebug() << text;
		QMessageBox::information(
			mParent,
			"NewBreeze EncFS - Mount Failed",
			"For some reason encfs has failed to mount the encrypted folder. Perhaps, the source folder was not an encryted volume. "
			"Check the terminal for the debug messages for the text printed out by encfs."
		);
	}
};

void NBEncFS::unmountDir() {

	QString queryTxt =  QString(
		"<html><body><p>You have chosen to unmount and encrypt the archive which is mounted at</p>" \
		"<p><center><b><tt>%1</tt></b></center></p>" \
		"<p>To unmount, please save your work, and close all active processes in the archive and " \
		"click <b>Yes</b>.</p><p>To cancel the unmount, press <b>No</b>.</p></body></html>"
	).arg( mTarget );

	QMessageBox::StandardButtons btns = QMessageBox::No | QMessageBox::Yes;

	int reply = QMessageBox::question( mParent, "Unmount?", queryTxt, btns );

	if ( reply != QMessageBox::Yes )
		return;

	QProcess proc;
	proc.start( "fusermount", QStringList() << "-u" << mTarget );
	proc.waitForFinished( -1 );

	if ( proc.exitCode() == 0 ) {
		QMessageBox::information(
			mParent,
			"Unmount Success",
			"The decrypted directory has been successfully unmounted and encrypted."
		);
	}

	else {
		qDebug() << proc.readAllStandardOutput() + proc.readAllStandardError();
		QMessageBox::information(
			mParent,
			"Unmount Failed",
			"Could not unmount and encrypt the directory. This could be because some process may be active " \
			"in the archive. Please make sure no processes are active in the archive and then try unmounting."
		);
	}
};

void NBEncFS::createEncFS() {

	QString mSrcTmp;

	/* If we have a source folder, create an encFS for it. */
	if ( mSource.count() ) {
		QMessageBox *mBox = new QMessageBox( mParent );
		QString title = "NewBreeze - EncFS Creation";
		QString message = QString( "Save all open files and clicke 'Continue' to proceed." );
		QString details = QString(
			"To create an EncFS volume this folder should be empty. I will attempt to move the data to a temporary location, "
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
			return;

		mSrcTmp = mSource + ".tmp" + QDateTime::currentDateTime().toString( "yyyyMMddThhmmssAP" );
		if ( rename( mSource.toLocal8Bit().data(), mSrcTmp.toLocal8Bit().data() ) != 0 ) {
			QMessageBox::critical(
				mParent,
				"NewBreeze - EncFS Creation Falied",
				"I have failed to create a new EncFS volume. Please check if you have read write permissions in the directory and try again."
			);

			return;
		}
	}


	QString pDir, encBase, decBase;

	if ( mSource.count() ) {
		pDir = dirName( mSource );
		encBase = "." + baseName( mSource);
		decBase = baseName( mSource );
	}

	NBNewEncFS *newEncFS = new NBNewEncFS( pDir + encBase, pDir + encBase, mParent );
	if ( newEncFS->exec() == QDialog::Accepted ) {

		QString encPath = newEncFS->encryptedPath();
		QString decPath = newEncFS->decryptedPath();
		QString passwd = newEncFS->password();

		if ( not encPath.endsWith( "/ ") )
			encPath += "/";

		if ( not decPath.endsWith( "/ ") )
			decPath += "/";

		/* Create encrypted directory path */
		mkpath( encPath );

		/* Create decrypted directory path */
		mkpath( decPath );

		QProcess proc;
		proc.setProcessChannelMode( QProcess::MergedChannels );
		proc.start( "encfs", QStringList() << "-S" << encPath << decPath );

		/* Question 1: standard mode, expert mode (x) or paranoia mode (p): p */
		proc.write( "p\n" );

		/* Question 2: Input password */
		proc.write( ( passwd + "\n" ).toLocal8Bit().data() );

		/* Question 3: Reinput password */
		proc.write( ( passwd + "\n" ).toLocal8Bit().data() );

		/* Wait for the process to be finished */
		proc.waitForFinished( -1 );

		if ( proc.exitCode() ) {
			qDebug() << "encfs -S " + encPath + " " + decPath;
			qDebug() << proc.readAll();
			QMessageBox::information(
				mParent,
				"NewBreeze - EncFS Creation Falied",
				"For some reason unknown to me the creation of the encfs volume failed. Please check the debug messages to identify the error."
			);

			return;
		}

		QSettings sett( QDir( dirName( encPath ) ).filePath( ".directory" ), QSettings::NativeFormat );
		sett.setValue( "EncFS/" + baseName( encPath ), baseName( decPath ) );
		QMessageBox::information(
			mParent,
			"NewBreeze - EncFS Creation Success",
			"The EncFS volume has been successfully created and mounted. To unmount the volume, right click and press '<u>U</u>nmount EncFS Volume'."
		);

		/* If source existed, and had contents: */
		if ( mSource.count() and ( mSource == decPath ) ) {

			int ret = system( ( "mv " + mSrcTmp + "/* " + mSource ).toLocal8Bit().data() );
			if ( ret ) {
				QMessageBox::information(
					mParent,
					"NewBreeze - Data Copy Failed",
					"NewBreeze has encountered error in copying the data that existed in your target directory from the"
				);
			}
		}
	}
};
