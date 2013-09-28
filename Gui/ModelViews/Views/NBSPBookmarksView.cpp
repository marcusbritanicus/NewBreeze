/*
	*
	* NBSPBookmarksView.cpp - BookMark Display Widget Class
	*
*/

#include <NBSPBookmarksView.hpp>
#include <NBTools.hpp>

NBBookmarkEdit::NBBookmarkEdit( QString text, int row ) : QLineEdit() {

	bookmarkIndex = row;
	setText( text );

	selectAll();
	setWindowFlags( Qt::FramelessWindowHint );
	setStyleSheet( "border: none; outline: none;" );

	connect( this, SIGNAL( editingFinished() ), this, SLOT( close() ) );
};

void NBBookmarkEdit::keyPressEvent( QKeyEvent *kEvent ) {

	if ( kEvent->key() == Qt::Key_Escape )
		close();

	else
		QLineEdit::keyPressEvent( kEvent );

	kEvent->accept();
};

NBSPBookmarksView::NBSPBookmarksView() : QListView() {

	// No Focus
	setFocusPolicy( Qt::NoFocus );

	// Set Data Model
	dataModel = new NBBookmarksModel();
	setModel( dataModel );

	// Item Delegate
	dataDelegate = new NBSPBookmarksDelegate();
	setItemDelegate( dataDelegate );

	// Watcher
	settingsWatcher = new QFileSystemWatcher();
	settingsWatcher->addPath( bookmarkSettings.fileName() );
	connect( settingsWatcher, SIGNAL( fileChanged( QString ) ), dataModel, SLOT( updateModelData() ) );

	// All the items have uniform sizes
	setUniformItemSizes( true );

	setResizeMode( QListView::Adjust );
	setMovement( QListView::Static );

	// DragAndDrop
	setDragDropMode( QAbstractItemView::NoDragDrop );
	setAcceptDrops( false );

	// Selection
	setSelectionMode( QAbstractItemView::NoSelection );

	// Context Menu
	setContextMenuPolicy( Qt::NoContextMenu );

	// Set StyleSheet
	setStyleSheet( getStyleSheet( "NBSidePanel", Settings.General.Style ) );
};

QSize NBSPBookmarksView::sizeHint() const {

	int heightHint = ( dataModel->rowCount() ) * 24;

	return QSize( dataModel->maxItemWidth(), heightHint );
};

QSize NBSPBookmarksView::minimumSizeHint() const {

	return sizeHint();
};

int NBSPBookmarksView::maxItemWidth() const {

	return dataModel->maxItemWidth();
};

void NBSPBookmarksView::mousePressEvent( QMouseEvent *mEvent ) {

	if ( indexAt( mEvent->pos() ).isValid() ) {
		QModelIndex idx = indexAt( mEvent->pos() );
		if( mEvent->modifiers() == Qt::ShiftModifier )
			renameBookMark( idx );

		else if ( mEvent->button() == Qt::LeftButton )
			emit mountDrive( dataModel->bookmarkPath( idx.row() ) );

		else if ( mEvent->button() == Qt::RightButton )
			showMenu( idx, mEvent->globalPos() );

		mEvent->accept();
	}

	else
		mEvent->ignore();
};

void NBSPBookmarksView::renameBookMark( QModelIndex idx ) {

	QString bookMarkText = idx.data().toString();

	bookmarkEdit = new NBBookmarkEdit( bookMarkText, idx.row() );
	bookmarkEdit->move( QWidget::mapToGlobal( rectForIndex( idx ).topLeft() ) + QPoint( 32, 0 ) );
	bookmarkEdit->resize( rectForIndex( idx ).size() - QSize( 32, 0 ) );

	bookmarkEdit->show();
	bookmarkEdit->setFocus();

	connect( bookmarkEdit, SIGNAL( textChanged( QString ) ), this, SLOT( updateText() ) );
};

void NBSPBookmarksView::showMenu( QModelIndex idx, QPoint pos ) {

	int row = idx.row();

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

	QMenu *menu = new QMenu();
	if ( ( Settings.General.Style == QString( "TransDark" ) ) or ( Settings.General.Style == QString( "TransLight" ) ) )
		menu->setAttribute( Qt::WA_TranslucentBackground );
    menu->setStyleSheet( getStyleSheet( "NBMenu", Settings.General.Style ) );

	if ( row > 0 )
		menu->addAction( moveUpAct );

	if ( row < dataModel->bookmarks().count() - 1 )
		menu->addAction( moveDownAct );

	menu->addSeparator();
	menu->addAction( deleteAct );
	menu->exec( pos );
};

void NBSPBookmarksView::updateText() {

	QString bookMarkText = bookmarkEdit->text();
	QString bookMarkPath = dataModel->bookmarkPath( bookmarkEdit->bookmarkIndex );
	bookmarkSettings.setValue( QUrl::toPercentEncoding( bookMarkPath ), bookMarkText );
	bookmarkSettings.sync();

	dataModel->updateModelData();
};

void NBSPBookmarksView::moveBookmarkUp() {

	QAction *deleter = qobject_cast<QAction*>( sender() );
	int bookmarkIndex = deleter->data().toInt();

	QStringList order = bookmarkSettings.value( "Order" ).toStringList();
	order.swap( bookmarkIndex, bookmarkIndex - 1 );

	bookmarkSettings.setValue( "Order", order );
	bookmarkSettings.sync();

	dataModel->updateModelData();
};

void NBSPBookmarksView::moveBookmarkDown() {

	QAction *deleter = qobject_cast<QAction*>( sender() );
	int bookmarkIndex = deleter->data().toInt();

	QStringList order = bookmarkSettings.value( "Order" ).toStringList();
	order.swap( bookmarkIndex, bookmarkIndex + 1 );

	bookmarkSettings.setValue( "Order", order );
	bookmarkSettings.sync();

	dataModel->updateModelData();
};

void NBSPBookmarksView::removeBookmark() {

	QAction *deleter = qobject_cast<QAction*>( sender() );
	int bookmarkIndex = deleter->data().toInt();

	bookmarkSettings.remove( QUrl::toPercentEncoding( dataModel->bookmarkPath( bookmarkIndex ) ) );

	QStringList order = bookmarkSettings.value( "Order" ).toStringList();
	order.removeAt( bookmarkIndex );

	bookmarkSettings.setValue( "Order", order );
	bookmarkSettings.sync();

	dataModel->updateModelData();
};
