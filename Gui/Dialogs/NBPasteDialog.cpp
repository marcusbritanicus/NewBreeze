/*
	*
	* NBPasteDialog.cpp - NewBreeze PasteDialog Class
	*
*/

#include <NBPasteDialog.hpp>

NBPasteDialog::NBPasteDialog() : NBDialog( QString( "n" ) ) {

	wasCanceled = false;
	isPaused = false;

	totalSize = 0;
	copiedSize = 0;

	createGUI();
	createAndSetupActions();
	setWindowProperties();
}

QStringList NBPasteDialog::sources() {

	return srcList;
};

void NBPasteDialog::setSources( QStringList sourceList ) {

	srcList.clear();
	srcList << sourceList;
};

QString NBPasteDialog::target() {

	return tgtDir;
};

void NBPasteDialog::setTarget( QString target ) {

	tgtDir = QString( target );
};

NBIOMode::Mode NBPasteDialog::ioMode() {

	return mode;
};

void NBPasteDialog::setIOMode( NBIOMode::Mode iomode ) {

	mode = iomode;
	if ( mode == NBIOMode::Copy )
		nameLbl->setText( "Moving files... ( Computing sizes )" );

	else
		nameLbl->setText( "Copying files... ( Computing sizes )" );
};

qreal NBPasteDialog::progress() {

	return totalPB->value();
};

QStringList NBPasteDialog::errorNodes() {

	return errors;
};

void NBPasteDialog::createGUI() {

	progressCheckTimer = new QTimer( this );

	QVBoxLayout *mainLyt = new QVBoxLayout();
	mainLyt->setContentsMargins( QMargins() );
	QHBoxLayout *btnLyt = new QHBoxLayout();

	nameLbl = new QLabel();
	fromToLbl = new QLabel();

	totalPB = new QProgressBar();
	totalPB->setRange( 0, 100 );
	filePB = new QProgressBar();
	filePB->setRange( 0, 100 );

	pauseBtn = new QPushButton( QIcon::fromTheme( "media-playback-pause" ), "&Pause" );
	pauseBtn->setFocusPolicy( Qt::ClickFocus );

	cancelBtn = new QPushButton( QIcon( ":/icons/cancel.png" ), "&Cancel" );
	cancelBtn->setObjectName( "cancelBtn" );
	cancelBtn->setFocusPolicy( Qt::ClickFocus );

	btnLyt->addWidget( pauseBtn );
	btnLyt->addStretch( 0 );
	btnLyt->addWidget( cancelBtn );

	mainLyt->addWidget( nameLbl );
	mainLyt->addWidget( new QLabel( "Total progress:" ) );
	mainLyt->addWidget( totalPB );
	mainLyt->addWidget( fromToLbl );
	mainLyt->addWidget( filePB );
	mainLyt->addLayout( btnLyt );

	setLayout( mainLyt );
}

void NBPasteDialog::createAndSetupActions() {

	connect( progressCheckTimer, SIGNAL( timeout() ), this, SLOT( setProgress() ) );

	connect( pauseBtn, SIGNAL( clicked() ), this, SLOT( togglePauseResume() ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( setCanceled() ) );
}

void NBPasteDialog::setWindowProperties() {

	if ( mode == NBIOMode::Copy )
		setDialogTitle( "NewBreeze - Copying files" );

	else
		setDialogTitle( "NewBreeze - Moving files" );

	setDialogIcon( QIcon( ":/icons/newbreeze.png" ) );
	setWindowModality( Qt::NonModal);

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 450 ) / 2 );
	int vpos = ( int )( ( dw.height() - 240 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 450, 240 );
}

void NBPasteDialog::startWork() {

	qApp->processEvents();

	preCopy();

	if ( mode == NBIOMode::Copy )
		nameLbl->setText( tr( "Copying files (%1)" ).arg( formatSize( totalSize ) ) );

	else
		nameLbl->setText( tr( "Moving files (%1)" ).arg( formatSize( totalSize ) ) );

	qApp->processEvents();
	performIO();
}

void NBPasteDialog::togglePauseResume() {

	if ( isPaused ) {
		isPaused = false;
		pauseBtn->setText( "Pause" );
		pauseBtn->setIcon( QIcon::fromTheme( "media-playback-pause" ) );
	}

	else {
		isPaused = true;
		pauseBtn->setText( "Resume" );
		pauseBtn->setIcon( QIcon::fromTheme( "media-playback-start" ) );
	}
};

void NBPasteDialog::setCanceled() {

	nameLbl->setText( "Cancelling...." );
	fromToLbl->setText( "" );

	totalPB->reset();
	filePB->reset();

	update();
	qApp->processEvents();

	wasCanceled = true;
};

void NBPasteDialog::handleFinished( int exitCode, QProcess::ExitStatus exitStatus ) {

	if ( exitStatus == QProcess::NormalExit ) {
		if ( exitCode ) {
			QString title = QString( "Access Error" );
			QString text = tr( "<html><body><p>An error was encountered while copy the file<br>" \
				"<b><tt>%1</tt></b><br><center>to</center><br><b><tt>%1</tt></b><br>" \
				"This may be because of unsufficient permissions. Please check that " \
				"the folder has write permissions and the disk is not full.</p>" \
				"</body></html>" ).arg( srcFile).arg( tgtFile );

			NBMessageDialog::error( title, text, QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Ok );
			return;
		}
	}
};

void NBPasteDialog::preCopy() {

	foreach( QString url, srcList ) {
		if ( QFileInfo( url ).isDir() ) {

			QDirIterator it( url, QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
			while ( it.hasNext() ) {
				totalSize += getSize( it.next() );
				qApp->processEvents();
			}
		}
		else {
			totalSize += getSize( url );
		}
	}
	qApp->processEvents();
};

void NBPasteDialog::performIO() {

	foreach( srcFile, srcList ) {
		if ( wasCanceled ) {
			emit IOComplete();
			return;
		}

		// If source is a dir, create the dir tree, then iterate through the dir and copy the files
		if ( QFileInfo( srcFile ).isDir() ) {
			srcDir = QString( srcFile );
			createDirTree();

			QDirIterator it( srcDir, QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
			while ( it.hasNext() ) {
				if ( wasCanceled ) {
					emit IOComplete();
					return;
				}

				srcFile = it.next();
				tgtFile = QDir( tgtDir ).filePath( QString( srcFile ).remove( QFileInfo( srcDir ).path() ).remove( 0, 1 ) );

				if ( srcFile == tgtFile ) {
					QString tgtDir = QFileInfo( srcFile ).absolutePath();
					QString tgtBase = QFileInfo( srcFile ).baseName();
					QString tgtSfx = mimeDb.suffixForFileName( srcFile );
					tgtSfx = tgtSfx.isEmpty() ? QFileInfo( srcFile ).completeSuffix() : tgtSfx;

					tgtFile = QDir( tgtDir ).filePath( tgtBase + " - copy." + tgtSfx );

					copyFile();
				}
				else
					copyFile();
			}

			if ( mode == NBIOMode::Move )
				// qDebug() << QString( "rm -rf '%1'" ).arg( srcDir.c_str();
				system( qPrintable( QString( "rm -rf '%1'" ).arg( srcDir ) ) );
		}

		// Otherwise it is a file ( hopefully ) set the tgtFile and begin copy
		else if ( QFileInfo( srcFile ).isFile() ) {
			if ( wasCanceled ) {
				emit IOComplete();
				return;
			}

			tgtFile = QDir( tgtDir ).filePath( QFileInfo( srcFile ).fileName() );
			if ( srcFile == tgtFile ) {
				QString tgtDir = QFileInfo( srcFile ).absolutePath();
				QString tgtBase = QFileInfo( srcFile ).baseName();
				QString tgtSfx = QFileInfo( srcFile ).completeSuffix().isEmpty() ? "" : "." + QFileInfo( srcFile ).completeSuffix();

				tgtFile = QDir( tgtDir ).filePath( tgtBase + " - copy" + tgtSfx );

				copyFile();
			}
			else
				copyFile();
		}
	}

	close();
	progressCheckTimer->stop();

	emit IOComplete();
};

void NBPasteDialog::createDirTree() {

	qDebug() << tgtDir;
	QDir( tgtDir ).mkpath( QFileInfo( srcDir ).fileName() );

	QDirIterator it( srcDir, QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot, QDirIterator::Subdirectories );
	while ( it.hasNext() )
		QDir( tgtDir ).mkpath( QString( it.next() ).remove( QFileInfo( srcDir ).path() ).remove( 0, 1 ) );
};

void NBPasteDialog::copyFile() {

	filePB->setValue( 0 );
	fromToLbl->setText( tr( "Copying %1..." ).arg( srcFile ) );

	progressCheckTimer->start( 500 );

	QFile ifile( srcFile );
	QFile ofile( tgtFile );

	ifile.open( QFile::ReadOnly );
	ofile.open( QFile::WriteOnly );

	if ( !ifile.isOpen() ) {
		errors << srcFile;
		return;
	}

	if ( !ofile.isOpen() ) {
		errors << tgtFile;
		return;
	}

	while( !ifile.atEnd() ) {
		if ( wasCanceled ) {
			ifile.close();
			ofile.close();

			progressCheckTimer->stop();
			close();
			return;
		}

		while ( isPaused ) {
			usleep( 100 );
			qApp->processEvents();
		}

		char block[ 4096 ];
		qint64 inBytes = ifile.read( block, sizeof( block ) );
		ofile.write( block, inBytes );
		qApp->processEvents();
	}

	ifile.close();
	ofile.close();

	ofile.setPermissions( ifile.permissions() );

	copiedSize += QFileInfo( srcFile ).size();
	totalPB->setValue( ( int )( ( 100 * copiedSize ) / totalSize ) );

	if ( mode == NBIOMode::Move )
		// qDebug() << QString( tr( "rm %1" ).arg( srcFile ) );
		ifile.remove();
};

void NBPasteDialog::setProgress() {

	int percentTotal = 100 * ( copiedSize + QFileInfo( tgtFile ).size() ) / totalSize;
	int percentFile = ( 100 * QFileInfo( tgtFile ).size() ) / QFileInfo( srcFile ).size();

	totalPB->setValue( percentTotal );
	filePB->setValue( percentFile );
	qApp->processEvents();
};
