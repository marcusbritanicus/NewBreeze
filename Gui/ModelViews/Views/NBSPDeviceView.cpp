/*
	*
	* NBSPDeviceView.cpp - BookMark Display Widget Class
	*
*/

#include <NBSPDeviceView.hpp>
#include <NBTools.hpp>

NBSPDeviceView::NBSPDeviceView() : QListView() {

	// Width Hint
	changeWidthHint();

	// Watcher
	settingsWatcher = new QFileSystemWatcher();
	settingsWatcher->addPath( settings.fileName() );
	connect( settingsWatcher, SIGNAL( fileChanged( QString ) ), this, SLOT( changeWidthHint() ) );

	// No Focus
	setFocusPolicy( Qt::NoFocus );

	// Set Data Model
	dataModel = new NBDeviceModel();
	setModel( dataModel );

	// Item Delegate
	dataDelegate = new NBSPDeviceDelegate();
	setItemDelegate( dataDelegate );

	// All the items have uniform sizes
	setUniformItemSizes( true );

	setResizeMode( QListView::Adjust );
	setMovement( QListView::Static );

	// DragAndDrop
	viewport()->setAcceptDrops(true);
	setDragDropMode( QListView::DragDrop );
	setDropIndicatorShown( true );
	setDragEnabled( true );
	setAcceptDrops( true );

	// Selection
	setSelectionMode( QAbstractItemView::NoSelection );

	// Context Menu
	setContextMenuPolicy( Qt::NoContextMenu );

	// Handle index clicked, i.e. clicked( const QModelIndex )
	connect( this, SIGNAL( clicked( const QModelIndex & ) ), this, SLOT( handleMouseClick( const QModelIndex & ) ) );
	setStyleSheet( getStyleSheet( "NBSidePanel", Settings.General.Style ) );
};

QSize NBSPDeviceView::sizeHint() const {

	int height = ( dataModel->rowCount() ) * 24;
	return QSize( dataModel->maxItemWidth(), height );
};

QSize NBSPDeviceView::minimumSizeHint() const {

	return sizeHint();
};

int NBSPDeviceView::maxItemWidth() const {

	return dataModel->maxItemWidth();
};

void NBSPDeviceView::handleMouseClick( const QModelIndex &index ) {

	int row = index.row();
	emit mountDrive( dataModel->deviceData( row ).mountPoint() );
};

void NBSPDeviceView::changeWidthHint() {

	widthHint = settings.value( "Session/Splitter" ).toSize().width() - 30;
};

void NBSPDeviceView::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBSPDeviceView::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	if ( indexAt( dmEvent->pos() ).isValid() ) {
		QModelIndex idx = indexAt( dmEvent->pos() );
		QString mtpt = dataModel->deviceData( idx.row() ).mountPoint();

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

void NBSPDeviceView::dropEvent( QDropEvent *dpEvent ) {

	QModelIndex idx = indexAt( dpEvent->pos() );
	QString mtpt = dataModel->deviceData( idx.row() ).mountPoint();

	const QMimeData *mData = dpEvent->mimeData();
	if ( mData->hasUrls() ) {

		QString qfmCmd = qApp->applicationFilePath();
		QStringList args;
		args << "--copypastemode" <<  "--target" << mtpt << "--sources";

		foreach( QUrl url, mData->urls() )
			args << url.toLocalFile();

		QProcess::startDetached( qfmCmd, args );
	}

	dpEvent->accept();
};
