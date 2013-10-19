/*
	*
	* NBContextmenu.cpp - ContextMenu class for NewBreeze
	*
*/

#include <NBContextMenu.hpp>
#include <NBFolderView.hpp>
#include <NBDesktopFile.hpp>
#include <NBTools.hpp>

NBCustomActionsMenu::NBCustomActionsMenu( QList<QModelIndex> selectedIndexes, QString dir ) : QMenu() {
	/*
		*
		* We set the custom actions based on the indexes
		*
	*/

	selection.clear();

	selection << selectedIndexes;
	workingDir = QString( dir );

	setTitle( tr( "&Actions" ) );
	setIcon( QIcon( ":/icons/archive.png" ) );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

    setStyleSheet( getStyleSheet( "NBMenu", Settings->General.Style ) );

	// Actions
	if ( selectedIndexes.count() == 1 ) {
		QFileInfo info( QDir( dir ).filePath( selectedIndexes.at( 0 ).data().toString() ) );
		if ( isArchive( info.absoluteFilePath() ) ) {
			QAction *uncompressAct = new QAction( QIcon( ":/icons/extract.png" ), tr( "E&xtract Archive" ), this );
			connect( uncompressAct, SIGNAL( triggered() ), this, SLOT( extract() ) );
			addAction( uncompressAct );
		}
		else {
			QAction *compressAct = new QAction( QIcon( ":/icons/archive.png" ), tr( "&Create Archive" ), this );
			connect( compressAct, SIGNAL( triggered() ), this, SLOT( compress() ) );
			addAction( compressAct );
		}
	}

	else {
		QAction *compressAct = new QAction( QIcon( ":/icons/archive.png" ), tr( "&Create Archive" ), this );
		connect( compressAct, SIGNAL( triggered() ), this, SLOT( compress() ) );
		addAction( compressAct );
	}

	addSeparator();
	buildCustomActionsMenu();

	QAction *addActionAct = new QAction( QIcon( ":/icons/list-add.png" ), tr( "Add custo&m action" ), this );
	connect( addActionAct, SIGNAL( triggered() ), this, SLOT( showCustomActionsDialog() ) );
	addSeparator();
	addAction( addActionAct );
};

void NBCustomActionsMenu::buildCustomActionsMenu() {

	foreach( QString group, actionSettings.childGroups() ) {
		actionSettings.beginGroup( group );

		QString name = actionSettings.value( "Name" ).toString();
		QStringList glob = actionSettings.value( "Glob" ).toStringList();
		QString exec = actionSettings.value( "Exec" ).toString();

		if ( exec.contains( "%term" ) ) {
			QStringList terminal = getTerminal();
			QString file = QDir( workingDir ).filePath( selection.at( 0 ).data().toString() );
			if ( terminal.at( 0 ) == QString( "xterm" ) )
				terminal[ 1 ] = QString( "cd %1 && /bin/bash" ).arg( termFormatString( file ) );

			else {
				terminal[ 2 ] = file;
				terminal[ 4 ] = "/bin/bash";
			}
			exec.replace( "%term", terminal.join( " " ) );
		}

		QAction *customAction = new QAction( QIcon( ":/icons/openWith.png" ), name, this );
		if ( ( exec.endsWith( "%f" ) or exec.endsWith( "%d" ) ) and selection.count() == 1 ) {
			QString file = QDir( workingDir ).filePath( selection.at( 0 ).data().toString() );

			if ( glob.at( 0 ) == QString( "*" ) ) {
				exec.replace( "%f", file ).replace( "%d", file );
				customAction->setData( exec.split( " ", QString::SkipEmptyParts ) );

				connect( customAction, SIGNAL( triggered() ), this, SLOT( doCustomAction() ) );
				addAction( customAction );
			}

			else if ( glob.at( 0 ) == QString( "file" ) and QFileInfo( file ).isFile() ) {
				exec.replace( "%f", file ).replace( "%d", file );
				customAction->setData( exec.split( " ", QString::SkipEmptyParts ) );

				connect( customAction, SIGNAL( triggered() ), this, SLOT( doCustomAction() ) );
				addAction( customAction );
			}

			else if ( glob.at( 0 ) == QString( "folder" ) and QFileInfo( file ).isDir() ) {
				exec.replace( "%f", file ).replace( "%d", file );
				customAction->setData( exec.split( " ", QString::SkipEmptyParts ) );

				connect( customAction, SIGNAL( triggered() ), this, SLOT( doCustomAction() ) );
				addAction( customAction );
			}
		}

		else if ( exec.endsWith( "%F" ) or exec.endsWith( "%D" ) ) {
			QStringList execList = exec.split( " ", QString::SkipEmptyParts );
			execList.removeLast();
			foreach( QModelIndex idx, selection )
				execList << QDir( workingDir ).filePath( idx.data().toString() );
			customAction->setData( execList );

			connect( customAction, SIGNAL( triggered() ), this, SLOT( doCustomAction() ) );
			addAction( customAction );
		}

		else if ( selection.count() == 1 ) {
			QString file = QDir( workingDir ).filePath( selection.at( 0 ).data().toString() );
			if ( glob.at( 0 ) == QString( "*" ) ) {
				exec.replace( "%f", file ).replace( "%d", file );
				customAction->setData( exec.split( " ", QString::SkipEmptyParts ) );

				connect( customAction, SIGNAL( triggered() ), this, SLOT( doCustomAction() ) );
				addAction( customAction );
			}

			else if ( glob.at( 0 ) == QString( "file" ) and QFileInfo( file ).isFile() ) {
				exec.replace( "%f", file ).replace( "%d", file );
				customAction->setData( exec.split( " ", QString::SkipEmptyParts ) );

				connect( customAction, SIGNAL( triggered() ), this, SLOT( doCustomAction() ) );
				addAction( customAction );
			}

			else if ( glob.at( 0 ) == QString( "folder" ) and QFileInfo( file ).isDir() ) {
				exec.replace( "%f", file ).replace( "%d", file );
				customAction->setData( exec.split( " ", QString::SkipEmptyParts ) );

				connect( customAction, SIGNAL( triggered() ), this, SLOT( doCustomAction() ) );
				addAction( customAction );
			}
		}

		actionSettings.endGroup();
	}
};

void NBCustomActionsMenu::extract() {

	emit extractArchive( QDir( workingDir ).filePath( selection.at( 0 ).data().toString() ) );
};

void NBCustomActionsMenu::compress() {

	QStringList archiveList;
	foreach( QModelIndex idx, selection )
		archiveList << QDir( workingDir ).filePath( idx.data().toString() );

	emit addToArchive( archiveList );
};

void NBCustomActionsMenu::showCustomActionsDialog() {

	NBCustomActions *customActions = new NBCustomActions();
	customActions->exec();
};

void NBCustomActionsMenu::doCustomAction() {

	QAction *customAct = qobject_cast<QAction*>( sender() );

	QStringList cmdList = customAct->data().toStringList();
	if ( cmdList.count() == 1 )
		QProcess::startDetached( cmdList.at( 0 ) );

	else
		QProcess::startDetached( cmdList.takeFirst(), cmdList );
};

NBOpenWithMenu::NBOpenWithMenu( QString icon, QString name, QWidget *parent ) : QMenu( parent ) {

	setIcon( QIcon( icon ) );
	setTitle( name );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

    setStyleSheet( getStyleSheet( "NBMenu", Settings->General.Style ) );
};

void NBOpenWithMenu::setWorkingDirectory( QString wDir ) {

	workingDir = QString( wDir );
};

void NBOpenWithMenu::buildMenu( QList<QModelIndex> selection ) {

	NBFolderView *FolderView = qobject_cast<NBFolderView *>( parent() );

	if ( selection.count() > 1 ) {
		QStringList files;
		foreach( QModelIndex idx, selection )
			files << QDir( workingDir ).absoluteFilePath( idx.data().toString() );

		QAction *openWithCmdAct = new QAction( QIcon( ":/icons/openWith.png" ), "&Open with...", this );
		openWithCmdAct->setData( QVariant( files ) );
		connect( openWithCmdAct, SIGNAL( triggered() ), FolderView, SLOT( doOpenWithCmd() ) );
		addAction( openWithCmdAct );
	}

	else {
		QFileInfo fInfo( QDir( workingDir ).absoluteFilePath( selection[ 0 ].data().toString() ) );
		QString file = termFormatString( fInfo.absoluteFilePath() );

		foreach( QString desktopEntry, getDesktopEntries( fInfo.absoluteFilePath() ) ) {
			NBDesktopFile dFile( desktopEntry );

			QString name = dFile.name;
			QStringList exec = dFile.execArgs;
			QString icon = dFile.icon;

			QIcon progIcon = QIcon::fromTheme( icon, QIcon( icon ) );
			if ( progIcon.pixmap( QSize( 16, 16 ) ).isNull() )
				progIcon = QIcon( "/usr/share/pixmaps/" + icon + ".png" );

			if ( progIcon.pixmap( QSize( 16, 16 ) ).isNull() )
				progIcon = QIcon( "/usr/share/pixmaps/" + icon + ".xpm" );

			if ( progIcon.pixmap( QSize( 16, 16 ) ).isNull() )
				progIcon = QIcon( ":/icons/exec.png" );

			// Prepare @v exec
			if ( dFile.takesArgs() )
				if ( dFile.multipleInputFiles ) {
					int idx = dFile.execArgs.indexOf( "<#NEWBREEZE-ARG-FILES#>" );
					exec.removeAt( idx );
					exec.insert( idx, fInfo.absoluteFilePath() );
				}

				else {
					int idx = dFile.execArgs.indexOf( "<#NEWBREEZE-ARG-FILE#>" );
					exec.removeAt( idx );
					exec.insert( idx, fInfo.absoluteFilePath() );
				}
			else
				exec << fInfo.absoluteFilePath();

			QAction *openWithAct = new QAction( progIcon, name, this );
			openWithAct->setData( QVariant( QStringList() << exec ) );
			connect( openWithAct, SIGNAL( triggered() ), FolderView, SLOT( doOpenWith() ) );
			addAction( openWithAct );
		}

		// Open with vlc
		if ( fInfo.isDir() ) {
			QAction *openWithVLCAct = new QAction( QIcon::fromTheme( "vlc" ), "Open with &VLC", this );
			openWithVLCAct->setData( QVariant( QStringList() << "vlc" << fInfo.absoluteFilePath() ) );
			connect( openWithVLCAct, SIGNAL( triggered() ), FolderView, SLOT( doOpenWith() ) );
			addSeparator();
			addAction( openWithVLCAct );
		}

		// Execute and Execute in terminal
		if ( fInfo.isFile() and fInfo.isExecutable() ) {
			QAction *runAct = new QAction( QIcon( ":/icons/exec.png" ), "Execute", this );
			runAct->setData( QVariant( fInfo.absoluteFilePath() ) );
			connect( runAct, SIGNAL( triggered() ), FolderView, SLOT( doOpenWith() ) );

			QAction *runInTermAct = new QAction( QIcon( ":/icons/exec.png" ), "Execute in terminal", this );
			runInTermAct->setData( QVariant( getTerminal().join( " " ).arg( workingDir ).arg( file ) ) );
			connect( runInTermAct, SIGNAL( triggered() ), FolderView, SLOT( doOpenWith() ) );

			addSeparator();
			addAction( runAct );
			addAction( runInTermAct );
		}

		addSeparator();

		QAction *openWithCmdAct = new QAction( QIcon( ":/icons/openWith.png" ), "&Open with...", this );
		openWithCmdAct->setData( QVariant( file ) );
		connect( openWithCmdAct, SIGNAL( triggered() ), FolderView, SLOT( doOpenWithCmd() ) );
		addAction( openWithCmdAct );
	}
};

void NBFolderView::showContextMenu( QPoint position ) {

	QList<QModelIndex> selectedList = getSelection();

    NBMenu *menu = new NBMenu();
	if ( selectedList.isEmpty() ) {

		// Create a new file/directory
		NBMenu *createNewMenu = new NBMenu( "Create &New" );
		createNewMenu->setIcon( QIcon::fromTheme( "archive-insert" ) );

		createNewMenu->addAction( actNewDir );
		createNewMenu->addAction( actNewFile );

		// File/directory sorting
		NBMenu *sortMenu = new NBMenu( "&Sort by" );
		sortMenu->setIcon( QIcon::fromTheme( "view-sort-ascending" ) );
		if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
			sortMenu->setAttribute( Qt::WA_TranslucentBackground );
		sortMenu->setStyleSheet( getStyleSheet( "NBMenu", Settings->General.Style ) );

		sortMenu->addAction( sortByNameAct );
		sortMenu->addAction( sortBySizeAct );
		sortMenu->addAction( sortByTypeAct );
		sortMenu->addAction( sortByDateAct );

		menu->addMenu( createNewMenu );
		menu->addSeparator();

		menu->addAction( pasteAct );
		menu->addSeparator();

		menu->addAction( reloadAct );
		menu->addSeparator();

		menu->addSeparator();
		menu->addMenu( sortMenu );

		menu->addSeparator();
		menu->addAction( propertiesAct );
	}

	else if ( selectedList.count() == 1 ) {
		menu->addAction( peekAct );

		QFileInfo fInfo( fsModel->nodePath( selectedList[ 0 ].data().toString() ) );
		QString file = termFormatString( fInfo.absoluteFilePath() );

		if ( fInfo.isDir() ) {
			QAction *openInNewWinAct = new QAction( QIcon( ":/icons/newwin.png" ), "Open in New &Window", this );
			openInNewWinAct->setData( QVariant( fInfo.absoluteFilePath() ) );
			connect( openInNewWinAct, SIGNAL( triggered() ), this, SLOT( doOpenInNewWindow() ) );
			menu->addAction( openInNewWinAct );
		}

		NBOpenWithMenu *openWithMenu = new NBOpenWithMenu( ":/icons/openWith.png", "&Open With", this );
		openWithMenu->setWorkingDirectory( fsModel->currentDir() );
		openWithMenu->buildMenu( selectedList );

		customMenu = new NBCustomActionsMenu( selectedList, fsModel->currentDir() );
		connect( customMenu, SIGNAL( extractArchive( QString ) ), this, SLOT( extract( QString ) ) );
		connect( customMenu, SIGNAL( addToArchive( QStringList ) ), this, SLOT( compress( QStringList ) ) );

		// File/directory sorting
		NBMenu *sortMenu = new NBMenu( "&Sort by" );
		sortMenu->setIcon( QIcon::fromTheme( "view-sort-ascending" ) );

		sortMenu->addAction( sortByNameAct );
		sortMenu->addAction( sortBySizeAct );
		sortMenu->addAction( sortByTypeAct );
		sortMenu->addAction( sortByDateAct );

		menu->addMenu( openWithMenu );
		menu->addSeparator();

		menu->addMenu( customMenu );
		menu->addSeparator();

		menu->addAction( moveAct );
		menu->addAction( copyAct );
		menu->addSeparator();

		menu->addAction( renameAct );

		menu->addSeparator();

		// menu->addAction( trash );
		menu->addAction( delAct );

		menu->addSeparator();
		menu->addMenu( sortMenu );

		menu->addSeparator();
		menu->addAction( propertiesAct );
	}

	else {
		NBOpenWithMenu *openWithMenu = new NBOpenWithMenu( ":/icons/openWith.png", "&Open With", this );
		openWithMenu->setWorkingDirectory( fsModel->currentDir() );
		openWithMenu->buildMenu( selectedList );

		customMenu = new NBCustomActionsMenu( selectedList, fsModel->currentDir() );
		connect( customMenu, SIGNAL( extractArchive( QString ) ), this, SLOT( extract( QString ) ) );
		connect( customMenu, SIGNAL( addToArchive( QStringList ) ), this, SLOT( compress( QStringList ) ) );

		// File/directory sorting
		NBMenu *sortMenu = new NBMenu( "&Sort by" );
		sortMenu->setIcon( QIcon::fromTheme( "view-sort-ascending" ) );

		sortMenu->addAction( sortByNameAct );
		sortMenu->addAction( sortBySizeAct );
		sortMenu->addAction( sortByTypeAct );
		sortMenu->addAction( sortByDateAct );

		menu->addMenu( openWithMenu );
		menu->addSeparator();

		menu->addMenu( customMenu );
		menu->addSeparator();

		menu->addAction( moveAct );
		menu->addAction( copyAct );
		menu->addSeparator();

		// menu->addAction( trashAct );
		menu->addAction( delAct );

		menu->addSeparator();
		menu->addMenu( sortMenu );

		menu->addSeparator();
		menu->addAction( propertiesAct );
	}

    menu->exec( mapToGlobal( position ) );
};
