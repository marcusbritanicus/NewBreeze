/*
	*
	* NBFolderView.cpp - NewBreeze FolderView Class
	*
*/

#include <NBFolderView.hpp>

NBFolderView::NBFolderView() : QStackedWidget() {

	// Set Data Model
	fsModel = new NBFileSystemModel();
	fsModel->setReadOnly( false );
	fsModel->setCategorizationEnabled( Settings->Session.SortCategory );

	// Setup the views
	IconView = new NBIconView( fsModel );
	ApplicationsView = new NBApplicationsView();
	CatalogView = new NBCatalogView();

	addWidget( IconView );
	addWidget( ApplicationsView );
	addWidget( CatalogView );

	// Minimum Width - 640px
	setMinimumWidth( 640 );

	// Init moveItems
	moveItems = false;

	// Create and setup actions like copy, move, rename, etc, etc
	createAndSetupActions();

	// Do not accept focus
	setFocusPolicy( Qt::NoFocus );
};

void NBFolderView::updateViewMode() {

	IconView->updateViewMode();
};

bool NBFolderView::hasSelection() {

	return IconView->selectionModel()->hasSelection();
};

void NBFolderView::createAndSetupActions() {

	connect( IconView, SIGNAL( open( QModelIndex ) ), this, SLOT( doOpen( QModelIndex ) ) );

	connect( IconView, SIGNAL( contextMenuRequested( QPoint ) ), this, SLOT( showContextMenu( QPoint ) ) );

	connect(
		IconView->selectionModel(), SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
		this, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) )
	);

	connect( IconView, SIGNAL( link( QStringList, QString ) ), this, SLOT( link( QStringList, QString ) ) );

	// DragDrop copy
	connect( IconView, SIGNAL( acopy( QStringList, QString ) ), this, SLOT( acopy( QStringList, QString ) ) );

	// DragDrop copy
	connect( IconView, SIGNAL( copy( QStringList, QString ) ), this, SLOT( copy( QStringList, QString ) ) );

	// DragDrop move
	connect( IconView, SIGNAL( move( QStringList, QString ) ), this, SLOT( move( QStringList, QString ) ) );

	connect( fsModel, SIGNAL( loadFolders() ), this, SLOT( showFolders() ) );
	connect( fsModel, SIGNAL( loadApplications() ), this, SLOT( showApplications() ) );
	connect( fsModel, SIGNAL( loadCatalogs() ), this, SLOT( showCatalogs() ) );

	// Home
	actHomeDir = new QAction( QIcon( ":/icons/home.png" ), "&Home", this );
	actHomeDir->setShortcuts( Settings->Shortcuts.GoHome );

	connect( actHomeDir, SIGNAL( triggered() ), this, SLOT( doOpenHome() ) );
	addAction( actHomeDir );

	// Up
	actParDir = new QAction( QIcon( ":/icons/up.png" ), "&Up", this );
	actParDir->setShortcuts( Settings->Shortcuts.GoUp );

	connect( actParDir, SIGNAL( triggered() ), this, SLOT( goUp() ) );
	addAction( actParDir );

	// Back
	actPrevDir = new QAction( QIcon( ":/icons/prev.png" ), "&Back", this );
	actPrevDir->setShortcuts( Settings->Shortcuts.GoLeft );

	connect( actPrevDir, SIGNAL( triggered() ), this, SLOT( goBack() ) );
	addAction( actPrevDir );

	// Forward
	actNextDir = new QAction( QIcon( ":/icons/next.png" ), "&Forward", this );
	actNextDir->setShortcuts( Settings->Shortcuts.GoRight );

	connect( actNextDir, SIGNAL( triggered() ), this, SLOT( goForward() ) );
	addAction( actNextDir );

	// New Folder
	actNewDir = new QAction( QIcon::fromTheme( "folder-new" ), "New folder", this );
	actNewDir->setShortcuts( Settings->Shortcuts.NewFolder );

	connect( actNewDir, SIGNAL( triggered() ), this, SLOT( newFolder() ) );
	addAction( actNewDir );

	// New file
	actNewFile = new QAction( QIcon::fromTheme( "document-new" ), "New File", this );
	actNewFile->setShortcuts( Settings->Shortcuts.NewFile );

	connect( actNewFile, SIGNAL( triggered() ), this, SLOT( newFile() ) );
	addAction( actNewFile );

	// Peek
	peekAct = new QAction( QIcon( ":/icons/peek.png" ), "Pee&k", this );
	peekAct->setShortcuts( Settings->Shortcuts.Peek );

	connect( peekAct, SIGNAL( triggered() ), this, SLOT( doPeek() ) );
	addAction( peekAct );

	// Copy
	copyAct = new QAction( QIcon( ":/icons/copy.png" ), "&Copy", this );
	copyAct->setShortcuts( Settings->Shortcuts.Copy );

	connect( copyAct, SIGNAL( triggered() ), this, SLOT( prepareCopy() ) );
	addAction( copyAct );

	// Move
	moveAct = new QAction( QIcon( ":/icons/cut.png" ), "Cu&t", this );
	moveAct->setShortcuts( Settings->Shortcuts.Cut );

	connect( moveAct, SIGNAL( triggered() ), this, SLOT( prepareMove() ) );
	addAction( moveAct );

	// Paste
	pasteAct = new QAction( QIcon( ":/icons/paste.png" ), "&Paste", this );
	pasteAct->setShortcuts( Settings->Shortcuts.Paste );

	connect( pasteAct, SIGNAL( triggered() ), this, SLOT( prepareIO() ) );
	addAction( pasteAct );

	// Rename
	renameAct = new QAction( QIcon( ":/icons/rename.png" ), "&Rename", this );
	renameAct->setShortcuts( Settings->Shortcuts.Rename );

	connect( renameAct, SIGNAL( triggered() ), this, SLOT( doRename() ) );
	addAction( renameAct );

	// Reload
	reloadAct = new QAction( QIcon( ":/icons/reload.png" ), "Re&fresh", this );
	reloadAct->setShortcuts( Settings->Shortcuts.Reload );

	connect( reloadAct, SIGNAL( triggered() ), this, SLOT( doReload() ) );
	addAction( reloadAct );

	// showDotFiles
	showHideDotFiles = new QAction( QIcon( ":/icons/showDotFiles.png" ), "Show &Hidden", this );
	showHideDotFiles->setShortcuts( Settings->Shortcuts.ToggleHidden );

	connect( showHideDotFiles, SIGNAL( triggered() ), this, SLOT( doToggleHidden() ) );
	addAction( showHideDotFiles );

	// Trash
	trashAct = new QAction( QIcon( ":/icons/trash.png" ), "Move to trash", this );
	trashAct->setShortcuts( Settings->Shortcuts.Trash );

	connect( trashAct, SIGNAL( triggered() ), this, SLOT( doSendToTrash() ) );
	addAction( trashAct );

	// Delete
	delAct = new QAction( QIcon( ":/icons/delete.png" ), "Delete", this );
	delAct->setShortcuts( Settings->Shortcuts.Delete );

	connect( delAct, SIGNAL( triggered() ), this, SLOT( doDelete() ) );
	addAction( delAct );

	// Properties
	propertiesAct = new QAction( QIcon( ":/icons/props.png" ), "&Properties", this );
	propertiesAct->setShortcuts( Settings->Shortcuts.Properties );

	connect( propertiesAct, SIGNAL( triggered() ), this, SIGNAL( showProperties() ) );
	addAction( propertiesAct );

	// Permissions
	permissionsAct = new QAction( QIcon::fromTheme( "system-users" ), "P&ermissions", this );
	permissionsAct->setShortcuts( Settings->Shortcuts.Permissions );

	connect( permissionsAct, SIGNAL( triggered() ), this, SIGNAL( showPermissions() ) );
	addAction( permissionsAct );

	// Open a virtual terminal emulator
	openVTE = new QAction( "Open &VTE", this );
	openVTE->setShortcuts( Settings->Shortcuts.Terminal );

	connect( openVTE, SIGNAL( triggered() ), this, SLOT( openTerminal() ) );
	addAction( openVTE );

	// Select All
	QAction *selectAllAct = new QAction( "&Select All", this );
	selectAllAct->setShortcuts( Settings->Shortcuts.SelectAll );

	connect( selectAllAct, SIGNAL( triggered() ), this, SLOT( selectAll() ) );
	addAction( selectAllAct );

	// Sorting
	sortByNameAct = new QAction( QIcon::fromTheme( "format-text-underline" ), "&Name", this );
	sortByNameAct->setCheckable( true );
	connect( sortByNameAct, SIGNAL( triggered() ), this, SLOT( sortByName() ) );

	sortByTypeAct = new QAction( QIcon::fromTheme( "preferences-other" ), "&Type", this );
	sortByTypeAct->setCheckable( true );
	connect( sortByTypeAct, SIGNAL( triggered() ), this, SLOT( sortByType() ) );

	sortBySizeAct = new QAction( QIcon( ":/icons/size.png" ), "&Size", this );
	sortBySizeAct->setCheckable( true );
	connect( sortBySizeAct, SIGNAL( triggered() ), this, SLOT( sortBySize() ) );

	sortByDateAct = new QAction( QIcon::fromTheme( "office-calendar" ), "&Date", this );
	sortByDateAct->setCheckable( true );
	connect( sortByDateAct, SIGNAL( triggered() ), this, SLOT( sortByDate() ) );

	QActionGroup *sortGroup = new QActionGroup( this );
	sortGroup->addAction( sortByNameAct );
	sortGroup->addAction( sortByTypeAct );
	sortGroup->addAction( sortBySizeAct );
	sortGroup->addAction( sortByDateAct );

	switch( Settings->Session.SortColumn ) {
		case 0: {
			sortByNameAct->setChecked( true );
			break;
		}
		case 1: {
			sortBySizeAct->setChecked( true );
			break;
		}
		case 2: {
			sortByTypeAct->setChecked( true );
			break;
		}
		case 4: {
			sortByDateAct->setChecked( true );
			break;
		}
	}

	groupsAct = new QAction( QIcon::fromTheme( "view-group", QIcon( ":/icons/groups.png" ) ), "Show in &Groups", this );
	groupsAct->setCheckable( true );
	groupsAct->setChecked( Settings->Session.SortCategory );
	connect( groupsAct, SIGNAL( triggered() ), this, SIGNAL( toggleGroups() ) );

	// Focus the search bar
	// QAction *focusSearchAct = new QAction( "Focus SearchBar", this );
	// focusSearchAct->setShortcuts( Settings->Shortcuts.FocusSearchBar );

	// connect( focusSearchAct, SIGNAL( triggered() ), this, SIGNAL( focusSearchBar() ) );
	// addAction( focusSearchAct );

	// Clear the search bar
	// QAction *clearSearchAct = new QAction( "Clear SearchBar", this );
	// clearSearchAct->setShortcuts( Settings->Shortcuts.ClearSearchBar );

	// connect( clearSearchAct, SIGNAL( triggered() ), this, SIGNAL( clearSearchBar() ) );
	// addAction( clearSearchAct );
};

QModelIndexList NBFolderView::getSelection() {

	QModelIndexList selectedList;
	selectedList << IconView->selectionModel()->selectedIndexes();
	foreach( QModelIndex idx, selectedList )
		if ( idx.column() )
			selectedList.removeAt( selectedList.indexOf( idx ) );

	return selectedList;
};

void NBFolderView::goUp() {

	fsModel->goUp();
};

void NBFolderView::goBack() {

	if ( fsModel->canGoBack() ) {
		emit updateAddressBar( fsModel->previousDir() );
		fsModel->goBack();
	}
};

void NBFolderView::goForward() {

	if ( fsModel->canGoForward() ) {
		emit updateAddressBar( fsModel->nextDir() );
		fsModel->goForward();
	}
};

void NBFolderView::doOpenHome() {

	fsModel->goHome();
};

void NBFolderView::newFile() {

	NBNewFileFolderDialog *newFile = new NBNewFileFolderDialog( "file", QDir( fsModel->currentDir() ) );
	newFile->exec();
};

void NBFolderView::newFolder() {

	NBNewFileFolderDialog *newFolder = new NBNewFileFolderDialog( "dir", QDir( fsModel->currentDir() ) );
	newFolder->exec();
};

void NBFolderView::doOpen( QString loc ) {

	if ( loc.startsWith( "NB://" ) ) {
		fsModel->setRootPath( loc );
		return;
	}

	if ( not isReadable( loc ) ) {
		QString title = tr( "Access Error" );
		QString text = tr( "You do not have enough permissions to open <b>%1</b>. " ).arg( baseName( loc ) );
		if ( isDir( loc ) )
			text += tr( "Please change the permissions of the directory to enter it." );

		else
			text += tr( "Please change the permissions of the file to edit/view it." );

		NBMessageDialog::error( title, text, QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Ok );
		return;
	}

	if ( isDir( loc ) ) {
		NBDebugMsg( DbgMsgPart::ONESHOT, "Opening dir: %s", qPrintable( loc ) );
		fsModel->setRootPath( loc );
	}

	else if ( isFile( loc ) ) {
		NBDebugMsg( DbgMsgPart::HEAD, "Opening file: %s", qPrintable( loc ) );

		NBAppFile app = NBAppEngine::instance()->xdgDefaultApp( mimeDb.mimeTypeForFile( loc ) );
		QStringList exec = app.execArgs();

		// Prepare @v exec
		if ( app.takesArgs() ) {
			if ( app.multipleArgs() ) {
				int idx = exec.indexOf( "<#NEWBREEZE-ARG-FILES#>" );
				exec.removeAt( idx );
				exec.insert( idx, loc );
			}

			else {
				int idx = exec.indexOf( "<#NEWBREEZE-ARG-FILE#>" );
				exec.removeAt( idx );
				exec.insert( idx, loc );
			}
		}
		else {
			exec << loc;
		}

		NBDebugMsg( DbgMsgPart::TAIL, ( QProcess::startDetached( exec.takeFirst(), exec ) ? "[DONE]" : " [FAILED]" ) );
	}

	else {
		QString title = QString( "Error opening file" );
		QString text = QString( "I really do not have any idea how to open <tt><b>%1</b></tt>" ).arg( loc );

		NBMessageDialog::error( title, text, QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Ok );
		return;
	}

	currentWidget()->setFocus();
};

void NBFolderView::doOpen( QModelIndex idx ) {

	Q_UNUSED( idx );
	QList<QModelIndex> selectedList = getSelection();

	if ( isDir( fsModel->nodePath( idx ) ) ) {
		NBDebugMsg( DbgMsgPart::ONESHOT, "Opening dir: %s", qPrintable( fsModel->nodePath( idx ) ) );
		fsModel->setRootPath( fsModel->nodePath( idx ) );
		return;
	}

	foreach( QModelIndex index, selectedList ) {
		QString fileToBeOpened = fsModel->nodePath( index );

		if ( not isReadable( fileToBeOpened ) ) {
			QString title = tr( "Access Error" );
			QString text = tr( "You do not have enough permissions to open <b>%1</b>. " ).arg( baseName( fileToBeOpened ) );
			if ( isDir( fileToBeOpened ) )
				text += tr( "Please change the permissions of the directory to enter it." );

			else
				text += tr( "Please change the permissions of the file to edit/view it." );

			NBMessageDialog::error( title, text, QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Ok );
			return;
		}

		if ( isDir( fileToBeOpened ) ) {
			NBDebugMsg( DbgMsgPart::ONESHOT, "Opening dir: %s", qPrintable( fileToBeOpened ) );
			fsModel->setRootPath( fileToBeOpened );
		}

		else if ( isFile( fileToBeOpened ) ) {
			if ( isExec( fileToBeOpened ) ) {
				/*
					*
					* We make sure that @v fileToBeOpened is really an executable file,
					* i.e it is one of shellscript, install file, or x-exec or x-sharedlib
					* or something of the sort and not a jpg file with exec perms
					*
				*/
				NBDebugMsg( DbgMsgPart::HEAD, "Executing: %s... ", qPrintable( fileToBeOpened ) );
				NBDebugMsg( DbgMsgPart::TAIL, ( QProcess::startDetached( fileToBeOpened ) ? "[DONE]" : "[FAILED]" ) );
			}

			else {
				NBDebugMsg( DbgMsgPart::HEAD, "Opening file: %s", qPrintable( fileToBeOpened ) );
				NBAppFile app = NBAppEngine::instance()->xdgDefaultApp( mimeDb.mimeTypeForFile( fileToBeOpened ) );
				QStringList exec = app.execArgs();

				// Prepare @v exec
				if ( app.takesArgs() ) {
					if ( app.multipleArgs() ) {
						int idx = exec.indexOf( "<#NEWBREEZE-ARG-FILES#>" );
						exec.removeAt( idx );
						exec.insert( idx, fileToBeOpened );
					}

					else {
						int idx = exec.indexOf( "<#NEWBREEZE-ARG-FILE#>" );
						exec.removeAt( idx );
						exec.insert( idx, fileToBeOpened );
					}
				}
				else {
					exec << fileToBeOpened;
				}

				NBDebugMsg( DbgMsgPart::TAIL, ( QProcess::startDetached( exec.takeFirst(), exec ) ? "[DONE]" : " [FAILED]" ) );
			}
		}

		else {
			QString title = QString( "Error" );
			QString text = QString( "I really do not have any idea how to open <b>%1</b>." ).arg( index.data().toString() );

			NBMessageDialog::error( title, text, QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Ok );
			return;
		}
	}

	currentWidget()->setFocus();
};

void NBFolderView::doOpenWith() {

	QStringList cmdList = qobject_cast<QAction *>( sender() )->data().toStringList();
	QString cmd = cmdList.takeFirst();

	QProcess::startDetached( cmd, cmdList );
};

void NBFolderView::doOpenInNewWindow() {

	emit newWindow( qobject_cast<QAction *>( sender() )->data().toString() );
};

void NBFolderView::doOpenWithCmd() {

	QStringList files = qobject_cast<QAction *>( sender() )->data().toStringList();

	NBRunCmdDialog *runCmd = new NBRunCmdDialog( QFileInfo( files[ 0 ] ).fileName() );
	runCmd->exec();

	if ( runCmd->canRun() ) {
		QStringList exec = runCmd->commandString().split( " " );

		// Prepare @v exec
		if ( exec.contains( "<#NEWBREEZE-ARG-FILES#>" ) ) {
			int idx = exec.indexOf( "<#NEWBREEZE-ARG-FILES#>" );
			exec.removeAt( idx );
			foreach( QString file, files )
				exec.insert( idx, file );
		}

		else if ( exec.contains( "<#NEWBREEZE-ARG-FILE#>" ) ) {
			int idx = exec.indexOf( "<#NEWBREEZE-ARG-FILE#>" );
			exec.removeAt( idx );
			exec.insert( idx, files[ 0 ] );
		}

		else
			exec << files;

		QString cmd = exec.takeFirst();
		QProcess::startDetached( cmd, exec );
	}
};

void NBFolderView::showApplications() {

	setCurrentIndex( 1 );
	currentWidget()->setFocus();
};

void NBFolderView::showCatalogs() {

	setCurrentIndex( 2 );
	currentWidget()->setFocus();
};

void NBFolderView::showFolders() {

	setCurrentIndex( 0 );
	currentWidget()->setFocus();
};

void NBFolderView::doPeek() {

	QModelIndex curIndex = IconView->currentIndex();

	if ( !curIndex.isValid() ) {
		emit showProperties();
		return;
	}

	if ( getSelection().count() > 1 ) {
		emit showProperties();
		return;
	}

	QString currentNode = QDir( fsModel->currentDir() ).absoluteFilePath( curIndex.data().toString() );
	QString mimeType = getMimeType( currentNode );

	if ( not isReadable( currentNode ) ) {
		NBMessageDialog::error(
			QString( "Cannot Open Location" ),
			QString(
				"You do not have enough permissions to preview the location. "		\
				"Please change the permissions of the directory to see its contents"
			)
		);

		return;
	}

	if ( isDir( currentNode ) ) {
		NBDebugMsg( DbgMsgPart::ONESHOT, "Previewing folder: %s", qPrintable( currentNode ) );
		NBFolderFlash *previewer = new NBFolderFlash( currentNode );

		connect( previewer, SIGNAL( loadFolder( QString ) ), this, SLOT( doOpen( QString ) ) );
		previewer->show();
	}

	else if ( mimeType.contains( "html" ) ) {
		NBDebugMsg( DbgMsgPart::ONESHOT, "Previewing html: %s", qPrintable( currentNode ) );
		NBWebWatch *previewer = new NBWebWatch( currentNode );
		previewer->show();
	}

	else if ( isText( currentNode ) ) {
		NBDebugMsg( DbgMsgPart::ONESHOT, "Previewing text: %s", qPrintable( currentNode ) );
		NBWordView *previewer = new NBWordView( currentNode );
		previewer->show();
	}

	else if ( mimeType.contains( "image" ) or mimeType.contains( "mng" ) or mimeType.contains( "gif" ) ) {
		NBDebugMsg( DbgMsgPart::ONESHOT, "Previewing image: %s", qPrintable( currentNode ) );
		NBImagePeek *previewer = new NBImagePeek( currentNode );
		previewer->show();
	}

	else if ( mimeType.contains( "opendocument.text" ) ) {
		NBDebugMsg( DbgMsgPart::ONESHOT, "Previewing image: %s", qPrintable( currentNode ) );
		NBOdfOgle *previewer = new NBOdfOgle( currentNode );
		previewer->show();
	}

	else if ( mimeType.contains( "pdf" ) ) {
		NBDebugMsg( DbgMsgPart::ONESHOT, "Previewing PDF: %s", qPrintable( currentNode ) );
		NBPdfPeep *previewer = new NBPdfPeep( currentNode );
		previewer->show();
	}

	else {
		// Custom Peeking
		NBDebugMsg( DbgMsgPart::ONESHOT, "Previewing file: %s", qPrintable( currentNode ) );
		NBCustomPeek *previewer = new NBCustomPeek( currentNode );
		previewer->show();
	}

	currentWidget()->setFocus();
};

void NBFolderView::doReload() {

	NBDebugMsg( DbgMsgPart::ONESHOT, "Reloading..." );
	fsModel->reload();
};

void NBFolderView::doToggleHidden() {

	if ( fsModel->showHidden() ) {
		NBDebugMsg( DbgMsgPart::ONESHOT, "Hiding dot files..." );
		fsModel->setShowHidden( false );
	}

	else {
		NBDebugMsg( DbgMsgPart::ONESHOT, "Showing dot files..." );
		fsModel->setShowHidden( true );
	}

	Settings->Session.ShowHidden = fsModel->showHidden();
};

void NBFolderView::prepareCopy() {

	if ( not hasSelection() )
		return;

	moveItems = false;
	QModelIndexList copyList = getSelection();

	QList<QUrl> urlList;
	foreach( QModelIndex item, copyList )
		urlList << QUrl::fromLocalFile( fsModel->nodePath( item.data().toString() ) );

	QMimeData *mimedata = new QMimeData();
	mimedata->setUrls( urlList );

	ClipBoard->setMimeData( mimedata );
};

void NBFolderView::prepareMove() {

	if ( not hasSelection() )
		return;

	moveItems = true;
	QModelIndexList copyList = getSelection();

	QList<QUrl> urlList;
	foreach( QModelIndex item, copyList )
		urlList << QUrl( fsModel->nodePath( item.data().toString() ) );

	QMimeData *mimedata = new QMimeData();
	mimedata->setUrls( urlList );

	ClipBoard->setMimeData( mimedata );;
};

void NBFolderView::acopy( QStringList srcList, QString tgt ) {

	emit copy( srcList, tgt, NBIOMode::ACopy );
};

void NBFolderView::copy( QStringList srcList, QString tgt ) {

	emit copy( srcList, tgt, NBIOMode::Copy );
};

void NBFolderView::move( QStringList srcList, QString tgt ) {

	emit copy( srcList, tgt, NBIOMode::Move );
};

void NBFolderView::link( QStringList linkList, QString path ) {

	foreach( QString node, linkList )
		QFile::link( node, QDir( path ).filePath( baseName( node ) ) );
};

void NBFolderView::prepareIO() {

	const QMimeData *mimeData = ClipBoard->mimeData();

	if ( mimeData->hasUrls() ) {
		QStringList srcList;
		foreach( QUrl url, mimeData->urls() )
			srcList << url.toLocalFile();

		QString target = fsModel->currentDir();

		if ( moveItems ) {
			move( srcList, target );
		}

		else {
			copy( srcList, target );
		}
	}

	else {
		NBNewFileFolderDialog *newFolder = new NBNewFileFolderDialog( "else", fsModel->currentDir(), mimeData->text() );
		newFolder->exec();
	}
};

void NBFolderView::doSendToTrash() {

	if ( !hasSelection() ) {
		return;
	}

	QList<QModelIndex> selectedList = getSelection();

	QStringList toBeDeleted;
	foreach( QModelIndex idx, selectedList )
		toBeDeleted << QDir( fsModel->rootPath() ).filePath( idx.data().toString() );

	NBDeleteManager *deleteManager = new NBDeleteManager( this, true );
	connect(
		deleteManager, SIGNAL( trashOperationComplete( QStringList, QStringList ) ),
		this, SLOT( handleDeleteFailure( QStringList, QStringList ) )
	);

	deleteManager->sendToTrash( toBeDeleted );
};

void NBFolderView::doDelete() {

	if ( !hasSelection() ) {
		return;
	}

	QList<QModelIndex> selectedList = getSelection();

	QStringList toBeDeleted;
	foreach( QModelIndex idx, selectedList )
		toBeDeleted << QDir( fsModel->rootPath() ).filePath( idx.data().toString() );

	toBeDeleted.removeDuplicates();

	NBConfirmDeleteDialog *deleteMsg = new NBConfirmDeleteDialog( toBeDeleted, true );
	if ( not deleteMsg->exec() )
		return;

	NBDeleteManager *deleteManager = new NBDeleteManager( this, false );
	connect(
		deleteManager, SIGNAL( deleteOperationComplete( QStringList, QStringList ) ),
		this, SLOT( handleDeleteFailure( QStringList, QStringList ) )
	);

	deleteManager->deleteFromDisk( toBeDeleted );
};

void NBFolderView::doRename() {

	QModelIndex curIndex = IconView->currentIndex();

	if ( !curIndex.isValid() )
		return;

	QString curFile = curIndex.data().toString();
	NBRenameDialog *renamer = new NBRenameDialog( curFile, QDir( fsModel->currentDir() ) );
	renamer->exec();

	currentWidget()->setFocus();

	if ( !renamer->renameOk ) {
		qDebug() << "Renaming" << curFile << "[Cancelled]";
		return;
	}

	QString opath = QDir( fsModel->currentDir() ).filePath( curFile );
	QString npath = QDir( fsModel->currentDir() ).filePath( renamer->le->text() );

	NBDebugMsg( DbgMsgPart::HEAD, "Renaming %s to %s... ", qPrintable( opath ), qPrintable( npath ) );
	if ( rename( qPrintable( opath ), qPrintable( npath ) ) )
		NBDebugMsg( DbgMsgPart::TAIL, "[Failed]" );

	else
		NBDebugMsg( DbgMsgPart::TAIL, "[Done]" );

	doReload();
};

void NBFolderView::sortByName() {

	if ( fsModel->isCategorizationEnabled() )
		fsModel->sort( 0, Qt::CaseInsensitive, true );

	else
		fsModel->sort( 0, Qt::CaseInsensitive, false );
};

void NBFolderView::sortBySize() {

	if ( fsModel->isCategorizationEnabled() )
		fsModel->sort( 1, Qt::CaseInsensitive, true );

	else
		fsModel->sort( 1, Qt::CaseInsensitive, false );
};

void NBFolderView::sortByType() {

	if ( fsModel->isCategorizationEnabled() )
		fsModel->sort( 2, Qt::CaseInsensitive, true );

	else
		fsModel->sort( 2, Qt::CaseInsensitive, false );
};

void NBFolderView::sortByDate() {

	if ( fsModel->isCategorizationEnabled() )
		fsModel->sort( 4, Qt::CaseInsensitive, true );

	else
		fsModel->sort( 4, Qt::CaseInsensitive, false );
};

void NBFolderView::selectAll() {

	IconView->selectAll();
};

void NBFolderView::openTerminal() {

	NBDebugMsg( DbgMsgPart::HEAD, "Opening the console at %s... ", qPrintable( fsModel->currentDir() ) );
	QStringList commandList = getTerminal();
	QString command = commandList.takeFirst();

	if ( command == QString( "xterm" ) )
		commandList[ 1 ] = QString( "cd %1 && /bin/bash" ).arg( termFormatString( fsModel->currentDir() ) );

	else {
		commandList[ 1 ] = fsModel->currentDir();
		commandList[ 3 ] = "/bin/bash";
	}

	NBDebugMsg( DbgMsgPart::TAIL, ( QProcess::startDetached( command, commandList ) ? "[DONE]" : "[FAILED]" ) );
};

void NBFolderView::setFocus() {

	currentWidget()->setFocus();
};

void NBFolderView::handleWatchDogBark( QString path ) {

	QFileInfo info( path );
	if ( !info.exists() ) {
		QString title = QString( "Error" );
		QString text = QString( "This directory has been deleted by an external process. Loading home dir." );

		NBMessageDialog::error( title, text, QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Ok );
		fsModel->goHome();
	}

	else {
		doReload();
	}
};

void NBFolderView::extract( QString archive ) {

	NBArchive::Type type;
	if ( archive.endsWith( ".zip" ) )
		type = NBArchive::ZIP;

	else if ( archive.endsWith( ".tar" ) )
		type = NBArchive::TAR;

	else if ( archive.endsWith( ".tzip" ) )
		type = NBArchive::TZIP;

	else if ( archive.endsWith( ".tar.bz2" ) )
		type = NBArchive::TBZ2;

	else if ( archive.endsWith( ".tar.gz" ) )
		type = NBArchive::TGZ;

	else if ( archive.endsWith( ".tar.xz" ) )
		type = NBArchive::TXZ;

	else if ( archive.endsWith( ".tbz2" ) )
		type = NBArchive::TBZ2;

	else if ( archive.endsWith( ".tgz" ) )
		type = NBArchive::TGZ;

	else if ( archive.endsWith( ".txz" ) )
		type = NBArchive::TXZ;

	else if ( archive.endsWith( ".bz2" ) )
		type = NBArchive::BZ2;

	else if ( archive.endsWith( ".gz" ) )
		type = NBArchive::GZ;

	else if ( archive.endsWith( ".xz" ) )
		type = NBArchive::XZ;

	else
		// We cannot process this type
		return;

	NBArchive *arc = new NBArchive( archive, NBArchive::READ, type );
	arc->setDestination( fsModel->nodePath( QFileInfo( archive ).baseName() ) );
	arc->extract();
};

void NBFolderView::compress( QStringList archiveList ) {

	/*
		*
		* Use custom getSaveFileName from NBFVDialogs.hpp to get the archive name
		*
	*/

	QString archiveName = NBFileDialog::getSaveFileName(
			QString( ":/icons/newbreeze2.png" ),
			tr( "NewBreeze - Save Archive As" ),
			fsModel->currentDir(),
			QStringList(
				QStringList() << "XZ Compressed Tar (*.txz)"
								<< "BZ2 Compressed Tar (*.tbz2)"
								<< "GZ Compressed Tar (*.tgz)"
								<< "Zip Compressed Tar (*.tzip)"
								<< "XZ File (*.xz)"
								<< "GZip File (*.gz)"
								<< "BZip2 File (*.bz2)"
								<< "Zip File (*.zip)" ),
			QString( "BZ2 Compressed Tar (*.tbz2)" )
	);

	if ( not archiveName.isEmpty() ) {

		NBArchive *arc;
		if ( archiveName.endsWith( ".txz" ) )
			arc = new NBArchive( archiveName, NBArchive::WRITE, NBArchive::TXZ );

		else if ( archiveName.endsWith( ".tbz2" ) )
			arc = new NBArchive( archiveName, NBArchive::WRITE, NBArchive::TBZ2 );

		else if ( archiveName.endsWith( ".tgz" ) )
			arc = new NBArchive( archiveName, NBArchive::WRITE, NBArchive::TGZ );

		else if ( archiveName.endsWith( ".tzip" ) )
			arc = new NBArchive( archiveName, NBArchive::WRITE, NBArchive::TZIP );

		else if ( archiveName.endsWith( ".xz" ) )
			arc = new NBArchive( archiveName, NBArchive::WRITE, NBArchive::XZ );

		else if ( archiveName.endsWith( ".bz2" ) )
			arc = new NBArchive( archiveName, NBArchive::WRITE, NBArchive::BZ2 );

		else if ( archiveName.endsWith( ".gz" ) )
			arc = new NBArchive( archiveName, NBArchive::WRITE, NBArchive::GZ );

		else if ( archiveName.endsWith( ".zip" ) )
			arc = new NBArchive( archiveName, NBArchive::WRITE, NBArchive::ZIP );

		else
			arc = new NBArchive( archiveName, NBArchive::WRITE, NBArchive::TAR );

		arc->updateInputFiles( archiveList );
		arc->setWorkingDir( fsModel->currentDir() );
		arc->create();
	}
};

void NBFolderView::updateProgress( QString nodePath, float fileCopied, float totalCopied ) {

	Q_UNUSED( fileCopied );
	Q_UNUSED( totalCopied );

	if ( dirName( nodePath ) == fsModel->currentDir() ) {
		fsModel->updateNode( baseName( nodePath ) );
	}
};

void NBFolderView::handleDeleteFailure( QStringList files, QStringList dirs ) {

	if ( ( not files.count() ) and ( not dirs.count() ) )
		return;

	QTableWidget *table = new QTableWidget( 0, 2 );
	table->setFocusPolicy( Qt::NoFocus );

	table->verticalHeader()->hide();
	table->setHorizontalHeaderLabels( QStringList() << "File Name" << "Size" );

	table->setShowGrid( false );
	table->setSelectionBehavior( QAbstractItemView::SelectRows );

	QHeaderView *headerView = new QHeaderView( Qt::Horizontal, table );
	table->setHorizontalHeader( headerView );
	#if QT_VERSION >= 0x050000
		headerView->setSectionResizeMode( 0, QHeaderView::Stretch );
		headerView->setSectionResizeMode( 1, QHeaderView::Fixed );
		table->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
	#else
		headerView->setResizeMode( 0, QHeaderView::Stretch );
		headerView->setResizeMode( 1, QHeaderView::Fixed );
		table->verticalHeader()->setResizeMode( QHeaderView::Fixed );
	#endif

	table->setColumnWidth( 1, 100 );

	foreach( QString path, dirs ) {
		QString iconName = NBIconProvider::icon( path );
		QTableWidgetItem *itm1 = new QTableWidgetItem( QIcon::fromTheme( iconName, QIcon( iconName ) ), path );
		QTableWidgetItem *itm2 = new QTableWidgetItem( formatSize( getSize( path ) ) );

		itm1->setFlags( itm1->flags() & ~Qt::ItemIsEditable );
		itm2->setFlags( itm2->flags() & ~Qt::ItemIsEditable );

		table->insertRow( table->rowCount() );

		table->setItem( table->rowCount() - 1, 0, itm1 );
		table->setItem( table->rowCount() - 1, 1, itm2 );
	}
	foreach( QString path, files ) {
		QString iconName = NBIconProvider::icon( path );
		QTableWidgetItem *itm1 = new QTableWidgetItem( QIcon::fromTheme( iconName, QIcon( iconName ) ), path );
		QTableWidgetItem *itm2 = new QTableWidgetItem( formatSize( getSize( path ) ) );

		itm1->setFlags( itm1->flags() & ~Qt::ItemIsEditable );
		itm2->setFlags( itm2->flags() & ~Qt::ItemIsEditable );

		table->insertRow( table->rowCount() );

		table->setItem( table->rowCount() - 1, 0, itm1 );
		table->setItem( table->rowCount() - 1, 1, itm2 );
	}

	NBMessageDialog::error(
		"NewBreeze - Error while deleting",
		"Some errors were encountered while deleting the files and folders you requested. "			\
		"As a result, some of the files and folders may not have been deleted. For the "			\
		"list for files and folder not deleted click <u>M</u>ore",
		QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Ok, table
	);
};

void NBFolderView::updateModel( QString ) {};
