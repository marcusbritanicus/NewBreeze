/*
	*
	* NBVfsView.cpp - Mounted VirtualFS View class for NewBreeze
	*
*/

#include "NBVfsView.hpp"

inline QString getDevType( NBDeviceInfo info ) {

	QString dev = info.device();
	QString vfsType = info.fileSystemType();

	QStringList cdTypes = QStringList() << "cdfs" << "iso9660" << "udf";
	QString devType = QString( "unknown" );

	if ( cdTypes.contains( vfsType ) )
		return ":/icons/optical.png";

	if ( vfsType.contains( "encfs" ) )
		return ":/icons/encfs.png";

	if ( vfsType.contains( "archivemount" ) )
		return ":/icons/archive.png";

	QDir disks = QDir( "/dev/disk/by-path" );
	disks.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System );
	foreach( QString disk, disks.entryList() ) {
		QFileInfo info( disks.filePath( disk ) );
		if ( info.symLinkTarget() == dev ) {
			if ( info.absoluteFilePath().contains( "usb" ) )
				return QString( ":/icons/usb.png" );

			else {
				if ( vfsType.toLower().contains( "ntfs" ) )
					return QString( ":/icons/hdd-win.png" );

				else if ( vfsType.toLower().contains( "fuseblk" ) )
					return QString( ":/icons/hdd-win.png" );

				else if ( vfsType.toLower().contains( "ext" ) )
					return QString( ":/icons/hdd-linux.png" );

				else if ( vfsType.toLower().contains( "jfs" ) )
					return QString( ":/icons/hdd-linux.png" );

				else if ( vfsType.toLower().contains( "reiser" ) )
					return QString( ":/icons/hdd-linux.png" );

				else if ( vfsType.toLower().contains( "zfs" ) )
					return QString( ":/icons/hdd-linux.png" );

				else if ( vfsType.toLower().contains( "xfs" ) )
					return QString( ":/icons/hdd-linux.png" );

				else if ( vfsType.toLower().contains( "btrfs" ) )
					return QString( ":/icons/hdd-linux.png" );

				else
					return QString( ":/icons/hdd.png" );
			}
		}
	}

	if ( devType == "unknown" ) {
		/*
			*
			* Lets try /sys/block approach
			*
			* Take /dev/<dev> part of the /dev/<dev> and check if 'usb' ia part of
			* target of /sys/block/<dev>. Else check the starting of <dev> and determine the type
			*
		*/
		QString sysfsPath = QString( "/sys/block/%1" ).arg( baseName( dev ) );
		if ( readLink( sysfsPath ).contains( "usb" ) )
			return QString( ":/icons/usb.png" );

		else {
			if ( baseName( dev ).startsWith( "sd" ) )
			// We have a generic mass storage device
				return QString( ":/icons/hdd.png" );

			else if ( baseName( dev ).startsWith( "sr" ) )
				return QString( ":/icons/optical.png" );

			else if ( baseName( dev ).startsWith( "se" ) or baseName( dev ).startsWith( "ses" ) )
				return QString( ":/icons/enclosure.png" );

			else if ( baseName( dev ).startsWith( "st" ) )
				return QString( ":/icons/tape.png" );

			else
				return devType;
		}
	}

	return devType;
};

NBVfsIcon::NBVfsIcon( QWidget *parent ) : QWidget( parent ) {

	/* Default Pixmap */
	mPixmap = QPixmap( ":/icons/encfs.png" );

	/* Flash settings */
	alpha = 0;
	mAlphaDelta = 30;
	color = palette().color( QPalette::Window ).darker();
	color.setAlpha( alpha );

	currentStep = 0;
	flashSteps = 10;
	mFlashDuration = 200;

	flash = false;
	flashesCompleted = 0;
	maxFlashes = 2;

	timer.setInterval( mFlashDuration / flashSteps );
	timer.setSingleShot( false );

	connect( &timer, SIGNAL( timeout() ), this, SLOT( repaint() ) );

	/* To track mouse movements */
	setMouseTracking( true );

	/* Set the fixed size of 48 px */
	setFixedSize( 48, 48 );

	/* Drag and Drop */
	setAcceptDrops( true );
	dndEntry = false;

	/* DeviceView */
	devView = new NBVfsMenu( this );
	devView->setObjectName( "NBVfsMenu" );
	connect( devView, SIGNAL( triggered( QAction* ) ), this, SLOT( clickDrive( QAction* ) ) );

	/* ToolTip */
	setToolTip( "Virtual FS" );
};

/* Override the QLabel pixmap property handlers */
QPixmap NBVfsIcon::pixmap() {

	return mPixmap;
};

void NBVfsIcon::setPixmap( QPixmap newPixmap ) {

	mPixmap = newPixmap;

	repaint();
};

/* Color property handlers */
QColor NBVfsIcon::flashColor() {

	return color;
};

void NBVfsIcon::setFlashColor( QColor newColor ) {

	color = newColor;
	color.setAlpha( alpha );

	repaint();
};

/* Alpha property handlers */
qint64 NBVfsIcon::alphaDelta() {

	return mAlphaDelta;
};

void NBVfsIcon::setAlphaDelta( qint64 newAlphaDelta ) {

	if ( ( newAlphaDelta * 10 ) > 255 )
		mAlphaDelta = 25;

	else
		mAlphaDelta = newAlphaDelta;
};

/* Flash duration property handlers */
int NBVfsIcon::flashDuration() {

	return mFlashDuration;
};

void NBVfsIcon::setFlashDuration( int newFlashDuration ) {

	mFlashDuration = ( newFlashDuration >= 10 ? newFlashDuration : 10 );
	timer.setInterval( mFlashDuration / flashSteps );
};

/* Number of Flashes property handlers */
int NBVfsIcon::numberOfFlashes() {

	return maxFlashes;
};

void NBVfsIcon::setNumberOfFlashes( int newNumOfFlashes ) {

	maxFlashes = newNumOfFlashes;
};

/* flashFrames property handlers - Number of frames per flash */
int NBVfsIcon::flashFrames() {

	return flashSteps;
};

void NBVfsIcon::setFlashFrames( int newFlashSteps ) {

	/* Persistence of vision is 62.5 (~63) ms */

	flashSteps = ( newFlashSteps > mFlashDuration ? mFlashDuration / 63 : newFlashSteps );
	timer.setInterval( mFlashDuration / flashSteps );
};

/* Overriding of paint event for showing flashes */
void NBVfsIcon::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );
	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );

	if ( flash ) {
		if ( flashesCompleted == maxFlashes ) {
			timer.stop();
			flash = false;
			currentStep = 0;
			alpha = 0;
			flashesCompleted = 0;
		}

		else if ( currentStep == flashSteps ) {
			flashesCompleted += 1;
			currentStep = 0;
		}

		else if ( currentStep < flashSteps / 2 ) {
			currentStep += 1;
			alpha += mAlphaDelta;
			color.setAlpha( alpha );
		}

		else if ( currentStep >= ( flashSteps / 2 ) ) {
			currentStep += 1;
			alpha -= mAlphaDelta;
			color.setAlpha( alpha );
		}
	}

	painter->save();

	painter->setPen( Qt::transparent );
	painter->setBrush( color );
	painter->drawRect( 0, 0, width(), height() );

	if ( not mPixmap.isNull() ) {
		painter->setBrush( Qt::transparent );
		painter->drawPixmap( 8, 8, 32, 32, mPixmap );
	}

	painter->restore();
	painter->end();

	pEvent->accept();
};

/* Overriding QLabel::mousePressEvent to emit clicked signal */
void NBVfsIcon::mousePressEvent( QMouseEvent *mEvent ) {

	if ( devView->isVisible() )
		devView->close();

	mEvent->accept();
};

/* Overriding QLabel::mouseMoveEvent to expand the view */
void NBVfsIcon::mouseMoveEvent( QMouseEvent *mEvent ) {

	mEvent->accept();
};

void NBVfsIcon::enterEvent( QEvent *eEvent ) {

	if ( dndEntry ) {
		eEvent->accept();
		return;
	}

	/* Start the delay timer */
	delayTimer.stop();
	delayTimer.start( 250, this );

	eEvent->accept();
};

void NBVfsIcon::leaveEvent( QEvent *eEvent ) {

	dndEntry = false;
	eEvent->accept();
};

void NBVfsIcon::dragEnterEvent( QDragEnterEvent *deEvent ) {

	dndEntry = true;

	/* Start the delay timer */
	delayTimer.stop();
	delayTimer.start( 250, this );

	deEvent->accept();
};

void NBVfsIcon::dragLeaveEvent( QDragLeaveEvent *dlEvent ) {

	dndEntry = false;
	dlEvent->accept();
};

void NBVfsIcon::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == delayTimer.timerId() ) {

		delayTimer.stop();
		if ( not devView->isVisible() and QRect( 0, 0, 48, 48 ).contains( mapFromGlobal( QCursor::pos() ) ) ) {

			showDevices();
		}
	}

	else if ( tEvent->timerId() == closeTimer.timerId() ) {

		closeTimer.stop();
		if ( devView->isVisible() )
			devView->close();
	}

	else
		QWidget::timerEvent( tEvent );

	tEvent->accept();
};

/* Slot to access the flashing */
void NBVfsIcon::flashLabel() {

	color = palette().color( QPalette::Window ).darker();
	color.setAlpha( alpha );

	if ( timer.isActive() )
		timer.stop();

	timer.start();
	flash = true;
};

/* Slot to access the flashing with a given color */
void NBVfsIcon::flashLabel( QColor newColor ) {

	setFlashColor( newColor );

	if ( timer.isActive() )
		timer.stop();

	timer.start();
	flash = true;
};

void NBVfsIcon::showDevices() {

	devView->clear();

	// Spacer Label
	QLabel *lbl = new QLabel( "<h4>&nbsp;&nbsp;&nbsp;&nbsp;VFS</h4>" );
	lbl->setFixedHeight( 48 );
	lbl->setMinimumWidth( 150 );

	QWidgetAction *lblAct = new QWidgetAction( devView );
	lblAct->setIcon( QIcon() );
	lblAct->setDefaultWidget( lbl );
	lblAct->setDisabled( true );

	devView->addAction( lblAct );

	Q_FOREACH( NBDeviceInfo info, NBDeviceManager::allVirtualMounts() ) {

		QWidgetAction *wa = new QWidgetAction( devView );
		wa->setData( info.mountPoint() );

		NBVfsAction *actWdgt = new NBVfsAction( info, devView );
		wa->setDefaultWidget( actWdgt );

		devView->addAction( wa );
	};

	closeTimer.start( 10000, this );
	devView->exec( mapToGlobal( QPoint( 49, 0 ) ) );
};

void NBVfsIcon::clickDrive( QAction *act ) {

	QWidgetAction *wAct = qobject_cast<QWidgetAction*>( act );
	NBVfsAction *devAct = qobject_cast<NBVfsAction*>( wAct->defaultWidget() );

	if ( devAct )
		emit driveClicked( devAct->mountPoint() );
};

NBVfsAction::NBVfsAction( NBDeviceInfo info, QWidget *parent ) : QWidget( parent ) {

	/* Mouse Tracking */
	setMouseTracking( true );

	/* Selection background */
	select = false;

	/* Fixed Item Heights */
	setFixedHeight( 32 );

	/* Width computation */
	QFontMetrics fm( font() );
	setMinimumWidth( qMax( 32 + fm.width( info.displayName() ) + 10, 150 ) );

	/*Data for Display  */
	mDeviceLabel = info.displayName();
	icon = QIcon( getDevType( info ) );
	mMountPoint = info.mountPoint();
	if ( not mMountPoint.endsWith( "/" ) )
		mMountPoint += "/";

	/* Disk Usage */
	percentUsed = 1.0 * info.bytesUsed() / info.bytesTotal();

	/* Enable drag and drop */
	setAcceptDrops( true );

	/* Set tooltip */
	setToolTip( mMountPoint );
};

QString NBVfsAction::mountPoint() {

	return mMountPoint;
};

void NBVfsAction::highlight( bool high ) {

	select = high;
	repaint();
}

void NBVfsAction::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );

	painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );

	// Selection Background
	if ( select or hasFocus() ) {
		painter.save();
		QPalette pltt = palette();
		painter.setPen( Qt::NoPen );
		painter.setBrush( pltt.color( QPalette::Highlight ) );
		painter.drawRect( rect() );
		painter.restore();
	}

	// Label
	painter.save();
	painter.drawText( QRect( 32, 0, width() - 32 - 10, 20 ), Qt::AlignLeft | Qt::AlignVCenter, mDeviceLabel );
	painter.restore();

	// Icon
	painter.save();

	painter.setPen( Qt::NoPen );
	painter.drawPixmap( QRect( 4, 4, 24, 24 ), icon.pixmap( 24 ) );

	painter.restore();

	// Used/Free Size
	painter.save();

	int red = 0, green = 0;
	// Lots of free space
	if ( percentUsed <= 0.4 ) {
		green = ( int )( 255 );
		red = ( int )( percentUsed * 638 );
	}

	// Around 50% free space remains
	else if ( percentUsed <= 0.6 ) {
		red = 255;
		green = 255;
	}

	// Very less free space remaining
	else {
		green = ( int )( ( 1 - percentUsed ) * 638 );
		red = ( int )( 255 );
	}

	painter.setPen( Qt::NoPen );
	painter.setBrush( QColor( red, green, 0 ) );
	painter.drawRoundedRect( QRect( 32, 20, ( width() - 32 - 10 ) * percentUsed, 6 ), 4.0, 4.0 );

	painter.restore();

	painter.end();
};

/* Overriding QLabel::enterEvent to emit entered signal */
void NBVfsAction::enterEvent( QEvent *eEvent ) {

	select = true;
	repaint();
};

/* Overriding QLabel::leaveEvent to emit exited signal */
void NBVfsAction::leaveEvent( QEvent *lEvent ) {

	select = false;
	repaint();
};

void NBVfsAction::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBVfsAction::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	const QMimeData *mData = dmEvent->mimeData();
	if ( not mData->hasUrls() ) {
		dmEvent->ignore();
		return;
	}

	if ( not isWritable( mMountPoint ) ) {
		dmEvent->ignore();
		return;
	}

	else if ( dirName( dmEvent->mimeData()->urls().at( 0 ).toLocalFile() ) == mMountPoint ) {
		dmEvent->ignore();
		return;
	}

	else {
		dmEvent->setDropAction( Qt::CopyAction );
		dmEvent->accept();
	}
};

void NBVfsAction::dropEvent( QDropEvent *dpEvent ) {

	if ( not dpEvent->mimeData()->hasUrls() ) {
		dpEvent->ignore();
		return;
	}

	if ( not isWritable( mMountPoint ) ) {
		dpEvent->ignore();
		return;
	}

	if ( dirName( dpEvent->mimeData()->urls().at( 0 ).toLocalFile() ) == mMountPoint ) {
		dpEvent->ignore();
		return;
	}

	NBProcess::Progress *progress = new NBProcess::Progress;
	progress->sourceDir = dirName( dpEvent->mimeData()->urls().at( 0 ).toLocalFile() );
	progress->targetDir = mMountPoint;

	QStringList srcList;
	Q_FOREACH( QUrl url, dpEvent->mimeData()->urls() )
		srcList << url.toLocalFile().replace( progress->sourceDir, "" );

	progress->type = NBProcess::Copy;

	NBIOProcess *proc = new NBIOProcess( srcList, progress );
	NBProcessManager::instance()->addProcess( progress, proc );

	progress->startTime = QTime::currentTime();

	proc->start();

	dpEvent->accept();
};

NBVfsMenu::NBVfsMenu( QWidget *parent ) : QMenu( parent ) {

	connect( this, SIGNAL( hovered( QAction* ) ), this, SLOT( highlightAction( QAction* ) ) );
};

void NBVfsMenu::clear() {

	QMenu::clear();
	actionList.clear();
};

void NBVfsMenu::addAction( QWidgetAction *act ) {

	actionList << act;
	QMenu::addAction( act );
};

void NBVfsMenu::highlightAction( QAction *act ) {

	/* Remove highlighting from all QWidgetActions */
	Q_FOREACH( QWidgetAction *wa, actionList ) {
		NBVfsAction *devAct = qobject_cast<NBVfsAction*>( wa->defaultWidget() );
			if ( devAct )
				devAct->highlight( false );
	}

	/* Get the corresponding QWidgetAction */
	QWidgetAction *wAct = qobject_cast<QWidgetAction*>( act );
	if ( not wAct )
		return;

	/* Highlight only the one which is hovered */
	NBVfsAction *devAct = qobject_cast<NBVfsAction*>( wAct->defaultWidget() );
	if ( devAct )
		devAct->highlight( true );
};
