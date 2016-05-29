/*
	*
	* NBSideBarGroup.cpp - SideBarGruop class for NewBreeze
	*
*/

#include <NBSideBarItem.hpp>

NBSideBarItem::NBSideBarItem( QWidget *parent ) : QLabel( parent ) {

	mHover = false;
	mPressed = false;
	mHighlight = false;

	setFixedHeight( 20 );
	setCursor( Qt::PointingHandCursor );
};

NBSideBarItem::NBSideBarItem( QString name, QString icon, QString target, QWidget *parent ) : QLabel( parent ) {

	mName = name;
	mIcon = icon;
	mTarget = target;

	if ( exists( mTarget ) and not mTarget.endsWith( "/" ) )
		mTarget += "/";

	if ( parent->objectName() == "SideBarGroup" ) {
		setContentsMargins( 20, 0, 0, 0 );
		setText( QString( "<img src='%1' height = 13 width = 13> %2" ).arg( mIcon ).arg( mName ) );
	}

	else {
		setContentsMargins( 10, 0, 0, 0 );
		setText( QString( "<img src='%1' height = 13 width = 13> <b>%2</b>" ).arg( mIcon ).arg( mName ) );
	}

	setToolTip( mTarget );

	mHover = false;
	mPressed = false;
	mHighlight = false;

	setFixedHeight( 20 );
	setCursor( Qt::PointingHandCursor );
};

QString NBSideBarItem::name() {

	return mName;
};

void NBSideBarItem::setName( QString newName ) {

	mName = newName;

	if ( parent()->objectName() == "SideBarGroup" ) {
		setContentsMargins( 20, 0, 0, 0 );
		setText( QString( "<img src='%1' height = 13 width = 13> %2" ).arg( mIcon ).arg( mName ) );
	}

	else {
		setContentsMargins( 10, 0, 0, 0 );
		setText( QString( "<img src='%1' height = 13 width = 13> <b>%2</b>" ).arg( mIcon ).arg( mName ) );
	}
};

QIcon NBSideBarItem::icon() {

	return QIcon( mIcon );
};

void NBSideBarItem::setIcon( QString newIcon ) {

	mIcon = newIcon;

	if ( parent()->objectName() == "SideBarGroup" ) {
		setContentsMargins( 20, 0, 0, 0 );
		setText( QString( "<img src='%1' height = 13 width = 13> %2" ).arg( mIcon ).arg( mName ) );
	}

	else {
		setContentsMargins( 10, 0, 0, 0 );
		setText( QString( "<img src='%1' height = 13 width = 13> <b>%2</b>" ).arg( mIcon ).arg( mName ) );
	}
};

QString NBSideBarItem::target() {

	return mTarget;
};

void NBSideBarItem::setTarget( QString newTarget ) {

	mTarget = newTarget;
	setToolTip( mTarget );
};

bool NBSideBarItem::isHighlighted() {

	return mHighlight;
};

void NBSideBarItem::setHighlighted( bool hilite ) {

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

bool NBSideBarItem::operator==( NBSideBarItem* other ) {

	return ( ( other->target() == mTarget ) and ( other->parent() == parent() ) );
};

void NBSideBarItem::enterEvent( QEvent *eEvent ) {

	mHover = true;
	repaint();

	eEvent->accept();
};

void NBSideBarItem::leaveEvent( QEvent *lEvent ) {

	mHover = false;
	repaint();

	lEvent->accept();
};

void NBSideBarItem::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBSideBarItem::dragMoveEvent( QDragMoveEvent *dmEvent ) {

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

void NBSideBarItem::dropEvent( QDropEvent *dpEvent ) {

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

	NBProcess::Progress *progress = new NBProcess::Progress;
	progress->sourceDir = dirName( dpEvent->mimeData()->urls().at( 0 ).toLocalFile() );
	progress->targetDir = mTarget;

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

void NBSideBarItem::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( not mHighlight ) {

		if ( mpEvent->button() == Qt::LeftButton )
			mPressed = true;
	}

	repaint();
	mpEvent->accept();
};

void NBSideBarItem::mouseReleaseEvent( QMouseEvent *mrEvent ) {

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

void NBSideBarItem::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setPen( Qt::NoPen );

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

	/* Hiver and no highlight */
	else if ( not mHighlight and mHover ) {

		painter.setBrush( palette().color( QPalette::Highlight ).lighter() );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	/* Both highlight and hover */
	else {

		painter.setBrush( palette().color( QPalette::Highlight ).darker( 125 ) );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	painter.end();

	QLabel::paintEvent( pEvent );
	pEvent->accept();
};
