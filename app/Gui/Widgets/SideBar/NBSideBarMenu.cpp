/*
	*
	* NBSideBarMenu.cpp - SidePanelItem class for NewBreeze
	*
*/

#include "NBSideBarMenu.hpp"
#include "NBBookmarkInfo.hpp"
#include "NBIconManager.hpp"

static const QString tooltipSkel = QString(
	"%1"
	"<table width = '100%' style = 'background-color: gray; font-size: 3pt;' CELLPADDING = 0 CELLSPACING = 0 >"
	"	<tr>"
	"		<td width = '%2%' style = 'background-color: %3;'></td>"
	"		<td></td>"
	"	</tr>"
	"</table>"
);

/* NBSideBarMenu */
NBSideBarMenu::NBSideBarMenu( QString icon, QString name, bool autoClose, QWidget *parent ) : QWidget( parent ) {

	setMaximumWidth( 120 + 32 + 4 );

	mName = name;
	mIcon = QIcon( icon ).pixmap( 24 );

	mAutoClose = autoClose;

	/* SideBar width + MenuEntry Icon width */
	mWidth = 0;

	/* Title label height: 36 */
	mHeight = 36;

	fm = new QFontMetrics( font() );

	setAcceptDrops( false );
	setWindowFlags( Qt::Popup );

	buildMenu();
};

void NBSideBarMenu::buildMenu() {

	QGridLayout *menuLyt = new QGridLayout();
	menuLyt->setContentsMargins( 1, 1, 1, 1 );
	menuLyt->setSpacing( 0 );

	QLabel *menuIconLbl = new QLabel();
	menuIconLbl->setFixedSize( QSize( 32, 36 ) );
	menuIconLbl->setAlignment( Qt::AlignCenter );
	menuIconLbl->setPixmap( mIcon );

	QLabel *titleLbl = new QLabel();
	titleLbl->setAlignment( Qt::AlignVCenter | Qt::AlignLeft );
	titleLbl->setFixedHeight( 36 );
	titleLbl->setText( "<h4>&nbsp;&nbsp;" + mName + "</h4>" );

	/* These two make up the Menu Title */
	menuLyt->addWidget( menuIconLbl, 0, 0 );
	menuLyt->addWidget( titleLbl, 0, 1 );

	/* Add the respective entries */
	if ( mName == "Devices" )
		menuLyt->addLayout( addDeviceEntries(), 1, 1 );

	else if ( mName == "Catalogs" )
		menuLyt->addLayout( addCatalogEntries(), 1, 1 );

	else if ( mName == "Bookmarks" )
		menuLyt->addLayout( addBookmarkEntries(), 1, 1 );

	else if ( mName == "Quick Files" )
		menuLyt->addLayout( addQuickFileEntries(), 1, 1 );

	setLayout( menuLyt );

	if ( mWidth > 120 )
		setFixedSize( QSize( 120 + 32 + 5, mHeight + 2 ) );

	else if ( mWidth + 5 < 120 )
		setFixedSize( QSize( 120, mHeight + 2 ) );

	else
		setFixedSize( QSize( mWidth + 5, mHeight + 2 ) );

	QRegion region( 0, 0, width(), mHeight + 2 );
	region = region.subtracted( QRegion( 0, 37, 31, mHeight - 36 ) );

	setMask( region );
};

QVBoxLayout *NBSideBarMenu::addDeviceEntries() {

	QVBoxLayout *entryLyt = new QVBoxLayout();
	entryLyt->setContentsMargins( QMargins() );
	entryLyt->setSpacing( 0 );

	Q_FOREACH( NBDeviceInfo info, NBDeviceManager::allDrives() ) {
		NBSideBarMenuEntry *item = new NBSideBarMenuEntry( info.displayName(), ":/icons/" + info.deviceType() + ".png", info.mountPoint(), NBSideBarMenuEntry::Device, this );
		connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
		connect( item, SIGNAL( clicked() ), this, SLOT( close() ) );

		entryLyt->addWidget( item );
		mHeight += 24;
		if ( 56 + fm->horizontalAdvance( info.displayName() ) > mWidth )
			mWidth = 56 + fm->horizontalAdvance( info.displayName() );

		/* Special tooltip hack */
		int percent = 100 * info.bytesUsed() / info.bytesTotal();
		item->setToolTip( tooltipSkel.arg( info.mountPoint() ).arg( percent ).arg( percent < 90 ? "darkgreen" : "darkred" ) );
	}

	return entryLyt;
};

QVBoxLayout *NBSideBarMenu::addCatalogEntries() {

	QVBoxLayout *entryLyt = new QVBoxLayout();
	entryLyt->setContentsMargins( QMargins() );
	entryLyt->setSpacing( 0 );

	QSettings ctlList( "NewBreeze", "Catalogs" );
	Q_FOREACH( QString key, ctlList.childKeys() ) {
		if ( ctlList.value( key ).toStringList().count() ) {
			NBSideBarMenuEntry *item = new NBSideBarMenuEntry( key, ":/icons/catalogs.png", "NB://Catalogs/" + key + "/", NBSideBarMenuEntry::Catalogs, this );
			connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
			connect( item, SIGNAL( clicked() ), this, SLOT( close() ) );
			item->setToolTip( key + " Catalog" );

			entryLyt->addWidget( item );
			mHeight += 24;

			if ( 56 + fm->horizontalAdvance( key ) > mWidth )
				mWidth = 56 + fm->horizontalAdvance( key );
		}
	}

	ctlList.beginGroup( "Custom" );
	Q_FOREACH( QString key, ctlList.childKeys() ) {
		if ( ctlList.value( key ).toStringList().count() ) {
			NBSideBarMenuEntry *item = new NBSideBarMenuEntry( key, ":/icons/catalogs.png", "NB://Catalogs/" + key + "/", NBSideBarMenuEntry::Catalogs, this );
			connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
			connect( item, SIGNAL( clicked() ), this, SLOT( close() ) );
			item->setToolTip( key + " Catalog" );

			entryLyt->addWidget( item );
			mHeight += 24;

			if ( 56 + fm->horizontalAdvance( key ) > mWidth )
				mWidth = 56 + fm->horizontalAdvance( key );
		}
	}
	ctlList.endGroup();

	return entryLyt;
};

QVBoxLayout *NBSideBarMenu::addBookmarkEntries() {

	QVBoxLayout *entryLyt = new QVBoxLayout();
	entryLyt->setContentsMargins( QMargins() );
	entryLyt->setSpacing( 0 );

	Q_FOREACH( NBBookmarkInfo info, NBBookmarkInfo::allBookmarks() ) {
		NBSideBarMenuEntry *item = new NBSideBarMenuEntry( info.displayLabel, NBIconManager::instance()->icon( "folder-favorites" ).at( 0 ), info.mountPoint, NBSideBarMenuEntry::Bookmark, this );
		connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
		connect( item, SIGNAL( clicked() ), this, SLOT( close() ) );
		item->setToolTip( info.mountPoint );

		entryLyt->addWidget( item );
		mHeight += 24;

		if ( 56 + fm->horizontalAdvance( info.displayLabel ) > mWidth )
			mWidth = 56 + fm->horizontalAdvance( info.displayLabel );	}

	return entryLyt;
};

QVBoxLayout *NBSideBarMenu::addQuickFileEntries() {

	QVBoxLayout *entryLyt = new QVBoxLayout();
	entryLyt->setContentsMargins( QMargins() );
	entryLyt->setSpacing( 0 );

	QSettings qfList( "NewBreeze", "SuperStart" );
	qfList.beginGroup( "Files" );

	Q_FOREACH( QString key, qfList.allKeys() ) {
		NBSideBarMenuEntry *item = new NBSideBarMenuEntry( key, NBIconManager::instance()->icon( "bookmarks" ).at( 0 ), qfList.value( key ).toString(), NBSideBarMenuEntry::QuickFile, this );
		connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
		connect( item, SIGNAL( clicked() ), this, SLOT( close() ) );
		item->setToolTip( qfList.value( key ).toString() );

		entryLyt->addWidget( item );
		mHeight += 24;

		if ( 56 + fm->horizontalAdvance( key ) > mWidth )
			mWidth = 56 + fm->horizontalAdvance( key );
	}

	qfList.endGroup();

	return entryLyt;
};

void NBSideBarMenu::show( QPoint point ) {

	move( point );
	QWidget::show();

	menuCloseTimer.start( 250, this );

	QEventLoop loop;
	connect( this, SIGNAL( closed() ), &loop, SLOT( quit() ) );

	loop.exec();
};

void NBSideBarMenu::closeEvent( QCloseEvent *cEvent ) {

	emit closed();
	cEvent->accept();
};

void NBSideBarMenu::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( mpEvent->button() == Qt::LeftButton ) {
		if ( QRect( QPoint( 0, 37 ), QPoint( 31, rect().bottom() ) ).contains( mpEvent->pos() ) )
			close();
	}

	QWidget::mousePressEvent( mpEvent );
};

void NBSideBarMenu::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );

	painter.setPen( Qt::gray );

	painter.drawLine( rect().topLeft(), rect().topRight() );
	painter.drawLine( rect().topRight(), rect().bottomRight() );
	painter.drawLine( QPoint( 0, 36 ), QPoint( 32, 36 ) );
	painter.drawLine( QPoint( 32, 36 ), QPoint( 32, rect().bottom() ) );
	painter.drawLine( QPoint( 32, rect().bottom() ), rect().bottomRight() );

	painter.end();

	pEvent->accept();
};

void NBSideBarMenu::timerEvent( QTimerEvent *teEvent ) {

	if ( teEvent->timerId() == menuCloseTimer.timerId() ) {
		QPoint cPos = mapFromGlobal( QCursor::pos() );
		bool good = rect().contains( cPos );
		bool bad = QRect( QPoint( 0, 37 ), QPoint( 31, rect().bottom() ) ).contains( cPos );
		if ( bad or not good ) {
			close();
		}
	}

	QWidget::timerEvent( teEvent );
	teEvent->accept();
};

/* NBSideBarMenuEntry */
NBSideBarMenuEntry::NBSideBarMenuEntry( QString name, QString icon, QString target, int type, QWidget *parent ) : QWidget( parent ) {

	setFixedHeight( 24 );
	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
	setMaximumWidth( 120 );

	mName = name;
	mIcon = icon;
	mTarget = target;
	mType = type;

	if ( isDir( mTarget ) and not mTarget.endsWith( "/" ) )
		mTarget += "/";

	setToolTip( mTarget );

	mHover = false;
	mPressed = false;

	setCursor( Qt::PointingHandCursor );
	setAcceptDrops( true );
};

QString NBSideBarMenuEntry::name() {

	return mName;
};

QIcon NBSideBarMenuEntry::icon() {

	return QIcon( mIcon );
};

QString NBSideBarMenuEntry::target() {

	return mTarget;
};

int NBSideBarMenuEntry::type() {

	return mType;
};

bool NBSideBarMenuEntry::operator==( NBSideBarMenuEntry* other ) {

	return ( ( other->target() == mTarget ) and ( other->parent() == parent() ) );
};

void NBSideBarMenuEntry::enterEvent( QEvent *eEvent ) {

	mHover = true;
	repaint();

	eEvent->accept();
};

void NBSideBarMenuEntry::leaveEvent( QEvent *lEvent ) {

	mHover = false;
	repaint();

	lEvent->accept();
};

void NBSideBarMenuEntry::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBSideBarMenuEntry::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	const QMimeData *mData = dmEvent->mimeData();
	if ( not mData->hasUrls() ) {
		dmEvent->ignore();
		return;
	}

	if ( not isWritable( mTarget ) ) {
		dmEvent->ignore();
		return;
	}

	else if ( dirName( dmEvent->mimeData()->urls().at( 0 ).toLocalFile() ) == mTarget ) {
		dmEvent->ignore();
		return;
	}

	else {
		dmEvent->setDropAction( Qt::CopyAction );
		dmEvent->accept();
	}
};

void NBSideBarMenuEntry::dropEvent( QDropEvent *dpEvent ) {

	if ( not dpEvent->mimeData()->hasUrls() ) {
		dpEvent->ignore();
		return;
	}

	if ( not isWritable( mTarget ) ) {
		dpEvent->ignore();
		return;
	}

	if ( dirName( dpEvent->mimeData()->urls().at( 0 ).toLocalFile() ) == mTarget ) {
		dpEvent->ignore();
		return;
	}

	const QMimeData *mData = dpEvent->mimeData();

	NBProcess::Process *progress = new NBProcess::Process;
	progress->sourceDir = dirName( mData->urls().at( 0 ).toLocalFile() );
	progress->targetDir = mTarget;
	progress->type = NBProcess::Copy;

	QStringList srcList;
	Q_FOREACH( QUrl url, mData->urls() )
		srcList << url.toLocalFile().replace( progress->sourceDir, "" );

	NBIOProcess *proc = new NBIOProcess( srcList, progress, this );
	NBProcessManager::instance()->addProcess( progress, proc );

	progress->startTime = QDateTime::currentDateTime();

	proc->start();

	dpEvent->accept();

	emit clicked();
};

void NBSideBarMenuEntry::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( mpEvent->button() == Qt::LeftButton )
		mPressed = true;

	if ( mpEvent->button() == Qt::RightButton ) {
		QMenu menu;
		if ( mType == Bookmark ) {
			menu.addAction( QIcon( ":/icons/delete.png" ), "&Remove Bookmark", this, SLOT( removeBookmark() ) );
			menu.exec( mapToGlobal( mpEvent->pos() ) );
		}

		else if ( mType == QuickFile ) {
			menu.addAction( QIcon( ":/icons/delete.png" ), "&Remove from Quick Files", this, SLOT( removeQuickFile() ) );
			menu.exec( mapToGlobal( mpEvent->pos() ) );
		}
	}

	repaint();
	mpEvent->accept();
};

void NBSideBarMenuEntry::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	if ( mrEvent->button() == Qt::LeftButton ) {
		/* A drop event can cause a mouseReleaseEvent */
		if ( mPressed ) {
			emit clicked( mTarget );
			emit clicked();
		}

		mPressed = false;
	}

	repaint();
	mrEvent->accept();
};

void NBSideBarMenuEntry::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setPen( Qt::NoPen );

	painter.save();
	/* Pressed */
	if ( mPressed ) {

		painter.setBrush( palette().color( QPalette::Highlight ).darker( 110 ) );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	/* Hovered */
	else if ( mHover ) {

		painter.setBrush( palette().color( QPalette::Highlight ).lighter() );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	/* No press and no hover */
	else {

		painter.setBrush( palette().color( QPalette::Window ) );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	painter.restore();

	/* Paint Icon */
	QRect iconRect = QRect( 4, 4, 16, 16 );
	painter.drawPixmap( iconRect, QPixmap( mIcon ).scaled( 16, 16 ) );

	QRectF textRect = QRectF( 24, 0, width() - 24, 24 );

	QFontMetrics fm( font() );
	QString text = fm.elidedText( mName, Qt::ElideRight, width() - 24 );

	painter.setPen( palette().color( QPalette::WindowText ) );
	painter.drawText( textRect, Qt::AlignLeft | Qt::AlignVCenter, text );

	painter.end();

	pEvent->accept();
};

void NBSideBarMenuEntry::removeBookmark() {

	if ( not mTarget.endsWith( "/" ) )
		mTarget += "/";

	bookmarkSettings.remove( QUrl::toPercentEncoding( mTarget ) );
	QStringList order = bookmarkSettings.value( "Order" ).toStringList();
	order.removeAll( mTarget );

	bookmarkSettings.setValue( "Order", order );
	bookmarkSettings.sync();

	deleteLater();
};

void NBSideBarMenuEntry::removeQuickFile() {

	QSettings superStart( "NewBreeze", "SuperStart" );
	qDebug() << "Removing" << mName << "from SuperStart";
	superStart.remove( "Files/" + mName );
	superStart.sync();

	deleteLater();
};
