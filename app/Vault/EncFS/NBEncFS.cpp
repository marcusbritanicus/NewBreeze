/*
	*
	* NBEncFS.cpp - NewBreeze Folder Encryption Class
	*
*/

#include "NBEncFS.hpp"

NBEncFS::NBEncFS( QString source, QString target, QWidget *parent ) : QObject( parent ) {

	/* Encrypted location */
	mSource = QString( source );

	/* Decrypted location */
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

	/* Create a temporary locations */
	QString mTgtTmp = mTarget + QDateTime::currentDateTime().toString( ".yyyymmdd-hhmmss" );

	/* If the source exists, move it to the above temporary location */
	if ( exists( mTarget ) and not QFile::rename( mTarget, mTgtTmp ) ) {
		QMessageBox::information(
			mParent,
			"NewBreeze - EncFS Creation Error",
			QString( "" )
		);

		/* We could not create the EncFS volume properly */
		return false;
	}

	/* encfs @mSource @mTarget */
	if ( not mSource.endsWith( "/ ") )
		mSource += "/";

	if ( not mTarget.endsWith( "/ ") )
		mTarget += "/";

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

	/* If there was an error: print it in the debug output */
	if ( proc.exitCode() ) {
		qDebug() << "encfs -S " + mSource + " " + mTarget;
		QString error = proc.readAll();
		qDebug() << error;
		QMessageBox::information(
			mParent,
			"NewBreeze - EncFS Creation Falied",
			"<p>For some reason unknown to me the creation of the encfs volume failed. Following error message was obtained:</p>" + error
		);

		/* We could not create the EncFS volume */
		return false;
	}

	/* Successfully created the directory, we add .directory entry */
	QSettings sett( QDir( dirName( mSource ) ).filePath( ".directory" ), QSettings::NativeFormat );
	sett.setValue( "EncFS/" + baseName( mSource ), baseName( mTarget ) );
	QMessageBox::information(
		mParent,
		"NewBreeze - EncFS Creation Success",
		"The EncFS volume has been successfully created and mounted. To unmount the volume, right click the folder and select '<u>U</u>nmount EncFS Volume'."
	);

	/* If source existed, and had contents: */
	if ( exists( mTgtTmp ) ) {

		int ret = system( ( "mv " + mTgtTmp + "/* " + mTarget ).toLocal8Bit().data() );
		if ( ret ) {
			QMessageBox::information(
				mParent,
				"NewBreeze - Data Restore Failed",
				QString( "NewBreeze has encountered error in restoring the data that existed in your target directory from the temporary location. "
				"You will find your data in the folder: %1" ).arg( mTgtTmp )
			);

			/* We created the EncFS volume properly, but data restore failed. */
			return true;
		}

		removeDir( mTgtTmp );
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

NBCreateEncFS::NBCreateEncFS( QWidget *parent ) : QDialog( parent ) {

	createGUI();

	QRect scrnSize = QDesktopWidget().screenGeometry();
	int hpos = ( int )( ( scrnSize.width() - 700 ) / 2 );
	int vpos = ( int )( ( scrnSize.height() - 400 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 450, 120 );
};

QString NBCreateEncFS::directoryName() {

	if ( result() == QDialog::Accepted )
		return dirLE->text();

	else
		return QString();
};

void NBCreateEncFS::createGUI() {

	QLabel *lbl = new QLabel( "Enter the name of the &directory:" );
	dirLE = new QLineEdit( this );

	lbl->setBuddy( dirLE );

	createBtn = new QPushButton( QIcon::fromTheme( "dialog-ok" ), "&Create EncFS", this );
	createBtn->setDisabled( true );
	connect( createBtn, SIGNAL( clicked() ), this, SLOT( accept() ) );

	QPushButton *cancelBtn = new QPushButton( QIcon::fromTheme( "dialog-close" ), "&Create EncFS", this );
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

void NBCreateEncFS::updateButton() {

	if ( not dirLE->text().count() )
		createBtn->setDisabled( true );

	else
		createBtn->setEnabled( true );
};
