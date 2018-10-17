/*
	*
	* NBSidePanelItem.cpp - SidePanelItem class for NewBreeze
	*
*/

#include "NBSidePanelItem.hpp"

NBSidePanelLabel::NBSidePanelLabel( QString icon, QString name, QWidget *parent ) : QWidget( parent ) {

	setFixedHeight( 32 );
	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );

	mName = name;
	mIcon = QIcon( icon ).pixmap( 24 );

	setCursor( Qt::PointingHandCursor );
	setAcceptDrops( false );
};

void NBSidePanelLabel::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	if ( mrEvent->button() == Qt::LeftButton )
		emit clicked();

	mrEvent->accept();
};

void NBSidePanelLabel::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setPen( Qt::NoPen );

	/* Paint Icon */
	QRect iconRect = QRect( 4, 4, 24, 24 );
	painter.drawPixmap( iconRect, mIcon );

	QRectF textRect = QRectF( 32, 0, width() - 32, 32 );
	painter.setPen( palette().color( QPalette::WindowText ) );

	QFont boldFont( font() );
	boldFont.setBold( true );
	painter.setFont( boldFont );

	painter.drawText( textRect, Qt::AlignLeft | Qt::AlignVCenter, mName );

	// painter.setPen( Qt::darkGray );
	// painter.drawLine( rect().topRight(), rect().bottomRight() );

	painter.end();

	pEvent->accept();
};

NBSidePanelItem::NBSidePanelItem( QString name, QString icon, QString target, int type, QWidget *parent ) : QWidget( parent ) {

	setFixedHeight( 20 );
	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );

	mName = name;
	mIcon = icon;
	mTarget = target;
	mType = type;

	if ( isDir( mTarget ) and not mTarget.endsWith( "/" ) )
		mTarget += "/";

	setToolTip( mTarget );

	mHover = false;
	mPressed = false;
	mHighlight = false;

	setCursor( Qt::PointingHandCursor );
	setAcceptDrops( true );
};

QString NBSidePanelItem::name() {

	return mName;
};

QIcon NBSidePanelItem::icon() {

	return QIcon( mIcon );
};

QString NBSidePanelItem::target() {

	return mTarget;
};

int NBSidePanelItem::type() {

	return mType;
};

bool NBSidePanelItem::isHighlighted() {

	return mHighlight;
};

void NBSidePanelItem::setHighlighted( bool hilite ) {

	mHighlight = hilite;

	if ( mHighlight ) {
		QPalette pltt( qApp->palette() );
		pltt.setColor( QPalette::WindowText, pltt.color( QPalette::HighlightedText ) );
		setPalette( pltt );
	}

	else {
		setPalette( qApp->palette() );
	}

	repaint();
};

bool NBSidePanelItem::operator==( NBSidePanelItem* other ) {

	return ( ( other->target() == mTarget ) and ( other->parent() == parent() ) );
};

void NBSidePanelItem::enterEvent( QEvent *eEvent ) {

	mHover = true;
	repaint();

	eEvent->accept();
};

void NBSidePanelItem::leaveEvent( QEvent *lEvent ) {

	mHover = false;
	repaint();

	lEvent->accept();
};

void NBSidePanelItem::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBSidePanelItem::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	const QMimeData *mData = dmEvent->mimeData();
	if ( not mData->hasUrls() ) {
		dmEvent->ignore();
		return;
	}

	if ( not isWritable( mTarget ) and not mTarget.startsWith( "NB://Tr" ) ) {
		dmEvent->ignore();
		return;
	}

	else if ( dirName( dmEvent->mimeData()->urls().at( 0 ).toLocalFile() ) == mTarget ) {
		dmEvent->ignore();
		return;
	}

	else {
		if ( mTarget == "NB://Trash" ) {
			dmEvent->setDropAction( Qt::MoveAction );
			dmEvent->accept();
		}

		else {
			dmEvent->setDropAction( Qt::CopyAction );
			dmEvent->accept();
		}
	}
};

void NBSidePanelItem::dropEvent( QDropEvent *dpEvent ) {

	if ( not dpEvent->mimeData()->hasUrls() ) {
		dpEvent->ignore();
		return;
	}

	if ( not isWritable( mTarget ) and not mTarget.startsWith( "NB://Tr" ) ) {
		dpEvent->ignore();
		return;
	}

	if ( dirName( dpEvent->mimeData()->urls().at( 0 ).toLocalFile() ) == mTarget ) {
		dpEvent->ignore();
		return;
	}

	const QMimeData *mData = dpEvent->mimeData();

	if ( mTarget == "NB://Trash" ) {
		NBProcess::Progress *progress = new NBProcess::Progress;
		progress->sourceDir = dirName( mData->urls().at( 0 ).toLocalFile() );
		progress->targetDir = QString();
		progress->type = NBProcess::Trash;

		QStringList toBeDeleted;
		Q_FOREACH( QUrl url, mData->urls() )
			toBeDeleted << url.toLocalFile().replace( progress->sourceDir, "" );

		NBDeleteProcess *proc = new NBDeleteProcess( toBeDeleted, progress );
		NBProcessManager::instance()->addProcess( progress, proc );

		progress->startTime = QTime::currentTime();
		proc->start();
	}

	else {
		NBProcess::Progress *progress = new NBProcess::Progress;
		progress->sourceDir = dirName( mData->urls().at( 0 ).toLocalFile() );
		progress->targetDir = mTarget;
		progress->type = NBProcess::Copy;

		QStringList srcList;
		Q_FOREACH( QUrl url, mData->urls() )
			srcList << url.toLocalFile().replace( progress->sourceDir, "" );

		NBIOProcess *proc = new NBIOProcess( srcList, progress );
		NBProcessManager::instance()->addProcess( progress, proc );

		progress->startTime = QTime::currentTime();

		proc->start();
	}

	dpEvent->accept();
};

void NBSidePanelItem::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( not mHighlight ) {

		if ( mpEvent->button() == Qt::LeftButton )
			mPressed = true;
	}

	if ( mpEvent->button() == Qt::RightButton ) {
		QMenu menu;
		menu.setAttribute( Qt::WA_DeleteOnClose );
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

void NBSidePanelItem::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	if ( not mHighlight ) {
		if ( mrEvent->button() == Qt::LeftButton ) {
			mPressed = false;
			emit clicked( mTarget );
			emit clicked();
		}
	}

	repaint();
	mrEvent->accept();
};

void NBSidePanelItem::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setPen( Qt::NoPen );

	painter.save();
	if ( mPressed ) {

		painter.setBrush( palette().color( QPalette::Highlight ).darker( 110 ) );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	/* No highlight and no hover */
	else if ( not ( mHighlight or mHover ) ) {

		painter.setBrush( palette().color( QPalette::Window ) );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	/* Highlight and no hover */
	else if ( mHighlight and not mHover ) {

		painter.setBrush( palette().color( QPalette::Highlight ) );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	/* Hover and no highlight */
	else if ( not mHighlight and mHover ) {

		painter.setBrush( palette().color( QPalette::Highlight ).lighter() );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	/* Both highlight and hover */
	else {

		painter.setBrush( palette().color( QPalette::Highlight ).darker( 125 ) );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}
	painter.restore();

	/* Paint Icon */
	QRect iconRect = QRect( 25, 2, 16, 16 );
	painter.drawPixmap( iconRect, QPixmap( mIcon ).scaled( 16, 16 ) );

	QRectF textRect = QRectF( 47, 0, width() - 47, 20 );

	QFontMetrics fm( font() );
	QString text = fm.elidedText( mName, Qt::ElideRight, width() - 47 );

	painter.setPen( palette().color( QPalette::WindowText ) );
	painter.drawText( textRect, Qt::AlignLeft | Qt::AlignVCenter, text );

	painter.end();

	pEvent->accept();
};

void NBSidePanelItem::removeBookmark() {

	if ( not mTarget.endsWith( "/" ) )
		mTarget += "/";

	bookmarkSettings.remove( QUrl::toPercentEncoding( mTarget ) );
	QStringList order = bookmarkSettings.value( "Order" ).toStringList();
	order.removeAll( mTarget );

	bookmarkSettings.setValue( "Order", order );
	bookmarkSettings.sync();

	deleteLater();
};

void NBSidePanelItem::removeQuickFile() {

	QSettings superStart( "NewBreeze", "SuperStart" );
	qDebug() << "Removing" << mName << "from SuperStart";
	superStart.remove( "Files/" + mName );
	superStart.sync();

	deleteLater();
};
