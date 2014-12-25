/*
	*
	* NBSidePanelView.cpp - The side panel view showing mounted devices or bookmarks
	*
*/

#include <NBSidePanelView.hpp>
#include <NBTools.hpp>
#include <sys/mount.h>

NBSidePanelView::NBSidePanelView() : QListView() {

	showingDevices = true;

	/* Set up the model */
	spModel = new NBSidePanelModel();
	setModel( spModel );

	/* setup the root index */
	setRootIndex( spModel->rootIndex() );

	/* Set up the delegate */
	iDelegate = new NBSidePanelDelegate();
	setItemDelegate( iDelegate );

	updateDevices();
	setupView();
}

int NBSidePanelView::idealWidth() {

	return mIdealWidth;
};

void NBSidePanelView::setupView() {

	/* No Focus */
	setFocusPolicy( Qt::NoFocus );

	/* StyleSheet and Palette */
	setStyleSheet( getStyleSheet( "NBSidePanelView", Settings->General.Style ) );
	setPalette( NBStyleManager::transparentPalette() );

	/* Selection */
	setSelectionMode( QTreeView::NoSelection );

	/* Mouse Tracking */
	setMouseTracking( true );

	/* Grid and Item Sizes */
	setGridSize( QSize( sizeHintForColumn( 0 ), 32 ) );
	setIconSize( QSize( 28, 28 ) );

	/* DragAndDrop */
	viewport()->setAcceptDrops(true);
	setDragDropMode( QListView::DragDrop );
	setDropIndicatorShown( true );
	setDragEnabled( true );
	setAcceptDrops( true );

	setFixedWidth( 0 );

	/* No ScrollBars */
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	/* Unifor Item Sizes */
	setUniformItemSizes( true );
};

void NBSidePanelView::showMenu( QModelIndex idx, QPoint pos ) {

	int row = idx.row();

	if ( showingDevices ) {
		QAction *unmountAct;

		unmountAct = new QAction( QIcon::fromTheme( "emblem-unmounted" ), "&Unmount", this );
		unmountAct->setData( idx.data( Qt::UserRole + 2 ) );
		connect( unmountAct, SIGNAL( triggered() ), this, SLOT( unmount() ) );

		NBMenu *menu = new NBMenu();
		connect( menu, SIGNAL( aboutToShow() ), this, SIGNAL( showingMenu() ) );
		connect( menu, SIGNAL( aboutToHide() ), this, SIGNAL( hidingMenu() ) );

		menu->addAction( unmountAct );

		menu->exec( pos );
	}

	else {
		QAction *moveUpAct, *moveDownAct, *deleteAct;

		deleteAct = new QAction( QIcon( ":/icons/delete.png" ), "&Delete BookMark", this );
		deleteAct->setData( row );
		connect( deleteAct, SIGNAL( triggered() ), this, SLOT( removeBookmark() ) );

		moveUpAct = new QAction( QIcon( ":/icons/arrow-up.png" ), "&Move Up", this );
		moveUpAct->setData( row );
		connect( moveUpAct, SIGNAL( triggered() ), this, SLOT( moveBookmarkUp() ) );

		moveDownAct = new QAction( QIcon( ":/icons/arrow-down.png" ), "&Move Down", this );
		moveDownAct->setData( row );
		connect( moveDownAct, SIGNAL( triggered() ), this, SLOT( moveBookmarkDown() ) );

		NBMenu *menu = new NBMenu();
		connect( menu, SIGNAL( aboutToShow() ), this, SIGNAL( showingMenu() ) );
		connect( menu, SIGNAL( aboutToHide() ), this, SIGNAL( hidingMenu() ) );

		if ( row > 0 )
			menu->addAction( moveUpAct );

		if ( row < spModel->rowCount() - 1 )
			menu->addAction( moveDownAct );

		menu->addSeparator();
		menu->addAction( deleteAct );
		menu->exec( pos );
	}
};

void NBSidePanelView::updateDevices() {

	showingDevices = true;
	spModel->updateDeviceData();

	setFixedHeight( spModel->rowCount() * 32 + contentsMargins().top() * 2 );
	mIdealWidth = sizeHintForColumn( 0 ) + contentsMargins().left() * 2;
};

void NBSidePanelView::updateBookmarks() {

	showingDevices = false;
	spModel->updateBookmarkData();

	setFixedHeight( spModel->rowCount() * 32 + contentsMargins().top() * 2 );
	mIdealWidth = sizeHintForColumn( 0 ) + contentsMargins().left() * 2;
};

void NBSidePanelView::handleClick( const QModelIndex clickedIndex ) {

	/* Some device or bookmark has been clicked */
	emit driveClicked( clickedIndex.data( Qt::UserRole + 1 ).toString() );
};

void NBSidePanelView::unmount() {

	QAction *act = qobject_cast<QAction*>( sender() );
	NBDeviceInfo info = act->data().value<NBDeviceInfo>();

	QProcess *unmountProc = new QProcess( this );
	unmountProc->start( "umount", QStringList() << info.mountPoint() );
	unmountProc->waitForFinished();
};

void NBSidePanelView::moveBookmarkUp() {

	QAction *mover = qobject_cast<QAction*>( sender() );
	int bookmarkIndex = mover->data().toInt();

	QStringList order = bookmarkSettings.value( "Order" ).toStringList();
	order.swap( bookmarkIndex, bookmarkIndex - 1 );

	bookmarkSettings.setValue( "Order", order );
	bookmarkSettings.sync();

	updateBookmarks();
};

void NBSidePanelView::moveBookmarkDown() {

	QAction *mover = qobject_cast<QAction*>( sender() );
	int bookmarkIndex = mover->data().toInt();

	QStringList order = bookmarkSettings.value( "Order" ).toStringList();
	order.swap( bookmarkIndex, bookmarkIndex + 1 );

	bookmarkSettings.setValue( "Order", order );
	bookmarkSettings.sync();

	updateBookmarks();
};

void NBSidePanelView::removeBookmark() {

	QAction *deleter = qobject_cast<QAction*>( sender() );
	int bookmarkIndex = deleter->data().toInt();

	QModelIndex idx = spModel->index( bookmarkIndex, 0 );
	bookmarkSettings.remove( QUrl::toPercentEncoding( spModel->data( idx, Qt::UserRole + 1 ).toString() ) );

	QStringList order = bookmarkSettings.value( "Order" ).toStringList();
	order.removeAt( bookmarkIndex );

	bookmarkSettings.setValue( "Order", order );
	bookmarkSettings.sync();

	updateBookmarks();
	emit bookmarkRemoved();
};

void NBSidePanelView::mousePressEvent( QMouseEvent *mEvent ) {

	if ( indexAt( mEvent->pos() ).isValid() ) {

		QModelIndex idx = indexAt( mEvent->pos() );

		if( mEvent->modifiers() == Qt::ShiftModifier ) {
			// renameBookMark( idx );
		}

		else if ( mEvent->button() == Qt::LeftButton ) {
			handleClick( idx );
		}

		else if ( mEvent->button() == Qt::RightButton ) {
			showMenu( idx, mEvent->globalPos() );
		}

		mEvent->accept();
		return;
	}

	QListView::mousePressEvent( mEvent );
	mEvent->accept();
};

void NBSidePanelView::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBSidePanelView::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	if ( indexAt( dmEvent->pos() ).isValid() ) {
		QModelIndex idx = indexAt( dmEvent->pos() );
		QString mtpt = spModel->data( idx, Qt::UserRole + 1 ).toString();

		if ( QFileInfo( mtpt ).isWritable() ) {
			dmEvent->setDropAction( Qt::CopyAction );
			dmEvent->accept();
		}

		else
			dmEvent->ignore();
	}

	else
		dmEvent->ignore();
};

void NBSidePanelView::dropEvent( QDropEvent *dpEvent ) {

	QModelIndex idx = indexAt( dpEvent->pos() );
	QString mtpt = spModel->data( idx, Qt::UserRole + 1 ).toString();

	const QMimeData *mData = dpEvent->mimeData();
	if ( mData->hasUrls() ) {

		QStringList args;
		foreach( QUrl url, mData->urls() )
			args << url.toLocalFile();

		// Copy -> Ctrl
		// Move -> Shift
		// Link -> Ctrl + Shift
		if ( dpEvent->keyboardModifiers() == Qt::ShiftModifier ) {

			emit move( args, mtpt, NBIOMode::Move );
		}

		else if ( ( dpEvent->keyboardModifiers() == Qt::ControlModifier ) or ( dpEvent->keyboardModifiers() == Qt::NoModifier ) ) {

			emit copy( args, mtpt, NBIOMode::Copy );
		}

		else if ( dpEvent->keyboardModifiers() == ( Qt::ControlModifier | Qt::ShiftModifier ) ) {

			foreach( QString node, args )
				QFile::link( node, QDir( mtpt ).filePath( baseName( node ) ) );
		}

		else if ( dpEvent->keyboardModifiers() == ( Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier ) ) {

			qDebug() << "Ctrl+Shift+Alt+Drop. Alphabetical Copy activated";
			QProcess::startDetached( "sh", QStringList() << "find -type f -print0 | sort -z | cpio -0 -pd " + mtpt );
			emit copy( args, mtpt, NBIOMode::ACopy );
		}

		else {

			dpEvent->ignore();
			return;
		}
	}

	dpEvent->accept();
};
