/*
	*
	* NBSideBarEntry.cpp - SidePanelItem class for NewBreeze
	*
*/

#include "NBSideBarEntry.hpp"

/* NBSideBarEntry */
NBSideBarEntry::NBSideBarEntry( QString icon, QString name, QWidget *parent ) : QLabel( parent ) {

	setFixedSize( QSize( 32, 36 ) );
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

	setAlignment( Qt::AlignCenter );

	mName = name;
	setToolTip( mName );

	mIconStr = icon;
	setPixmap( QIcon( icon ).pixmap( 24 ) );

	setCursor( Qt::PointingHandCursor );
	setAcceptDrops( false );

	connect( this, SIGNAL( clicked() ), SLOT( showMenu() ) );
};

void NBSideBarEntry::showMenu() {

	NBSideBarMenu *menu = new NBSideBarMenu( mIconStr, mName, this );
	connect( menu, SIGNAL( driveClicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );

	menu->show( mapToGlobal( QPoint( 0, 0 ) ) );

	delete menu;
};

void NBSideBarEntry::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	if ( mrEvent->button() == Qt::LeftButton )
		emit clicked();

	mrEvent->accept();
};

/* NBSideBarTrashEntry */
NBSideBarTrashEntry::NBSideBarTrashEntry( QString icon, QString name, QWidget *parent ) : NBSideBarEntry( icon, name, parent ) {

	setFixedSize( QSize( 32, 32 ) );
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

	setAcceptDrops( true );
};

void NBSideBarTrashEntry::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBSideBarTrashEntry::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	const QMimeData *mData = dmEvent->mimeData();
	if ( not mData->hasUrls() ) {
		dmEvent->ignore();
		return;
	}

	dmEvent->setDropAction( Qt::MoveAction );
	dmEvent->accept();
};

void NBSideBarTrashEntry::dropEvent( QDropEvent *dpEvent ) {

	if ( not dpEvent->mimeData()->hasUrls() ) {
		dpEvent->ignore();
		return;
	}

	const QMimeData *mData = dpEvent->mimeData();

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

	dpEvent->accept();
};
