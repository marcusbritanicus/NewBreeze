/*
	*
	* NBUpdater.cpp - NewBreeze Update Various
	*
*/

#include <NewBreeze.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>

void NewBreeze::updateVarious( QString url ) {

	QString folder = ( ( url == "/" ) ? QString() : url.split( "/", QString::SkipEmptyParts ).takeLast() );
	setWindowTitle( ( folder.isEmpty() ? "/ - NewBreeze" : QString( "%1 - NewBreeze" ).arg( folder ) ) );

	AddressBar->addressWidget->addressEdit->setText( url );
	AddressBar->addressWidget->crumbsBar->setCurrentDirectory( url );

	Terminal->changeDir( url );
};

void NewBreeze::updateQuickMenuBar() {

	if ( FolderView->hasSelection() ) {
		QuickMenuBar->openWithBtn->setEnabled( true );
		QuickMenuBar->moveToBtn->setEnabled( true );
		QuickMenuBar->deleteBtn->setEnabled( true );
		QuickMenuBar->permsBtn->setEnabled( true );
	}
	else {
		QuickMenuBar->openWithBtn->setDisabled( true );
		QuickMenuBar->moveToBtn->setDisabled( true );
		QuickMenuBar->deleteBtn->setDisabled( true );
		QuickMenuBar->permsBtn->setDisabled( true );
	}
};

void NewBreeze::updateInfoBar() {

	InfoBar->clear();
	if ( qobject_cast<NBFolderView*>( sender() ) == FolderView ) {
		QModelIndexList selectedIndexes = FolderView->getSelection();
		if ( selectedIndexes.count() == 0 )
			InfoBar->updateInfoBarCF( FolderView->fsModel->currentDir() );

		else if ( selectedIndexes.count() == 1 )
			InfoBar->updateInfoBarSingle( FolderView->fsModel->nodePath( selectedIndexes.at( 0 ) ) );

		else
			InfoBar->updateInfoBarSelection( FolderView->fsModel->currentDir(), selectedIndexes );
	}

	else if ( qobject_cast<NBFileSystemModel*>( sender() ) == FolderView->fsModel )
		InfoBar->updateInfoBarCF( FolderView->fsModel->currentDir() );
};

void NewBreeze::changeViewMode() {

	QStringList viewModes = QStringList() << "SmallListView" << "NormalListView"<< "TilesView";
	viewModes << "SmallIconsView" << "NormalIconsView" << "LargeIconsView" << "HugeIconsView";
	viewModes << "SDetailsView" << "NDetailsView";

	if ( AddressBar->viewModeBtn->smallListBtn->isChecked() )
		Settings.General.FolderView = viewModes.at( 0 );

	else if ( AddressBar->viewModeBtn->mediumListBtn->isChecked() )
		Settings.General.FolderView = viewModes.at( 1 );

	else if ( AddressBar->viewModeBtn->tilesBtn->isChecked() )
		Settings.General.FolderView = viewModes.at( 2 );

	else if ( AddressBar->viewModeBtn->smallIconsBtn->isChecked() )
		Settings.General.FolderView = viewModes.at( 3 );

	else if ( AddressBar->viewModeBtn->normalIconsBtn->isChecked() )
		Settings.General.FolderView = viewModes.at( 4 );

	else if ( AddressBar->viewModeBtn->largeIconsBtn->isChecked() )
		Settings.General.FolderView = viewModes.at( 5 );

	else if ( AddressBar->viewModeBtn->hugeIconsBtn->isChecked() )
		Settings.General.FolderView = viewModes.at( 6 );

	else if ( AddressBar->viewModeBtn->smallDetailsBtn->isChecked() )
		Settings.General.FolderView = viewModes.at( 7 );

	else if ( AddressBar->viewModeBtn->normalDetailsBtn->isChecked() )
		Settings.General.FolderView = viewModes.at( 8 );

	settings.setValue( "FolderView", Settings.General.FolderView );
	settings.sync();

	int fvcWidth = FolderView->size().width();

	if ( FolderView->currentIndex() ) {
		fvcWidth -= FolderView->TreeView->contentsMargins().left();
		fvcWidth -= FolderView->TreeView->contentsMargins().right();
		fvcWidth -= FolderView->TreeView->autoScrollMargin();
		fvcWidth -= FolderView->TreeView->verticalScrollBar()->width();
	}
	else {
		fvcWidth -= FolderView->IconView->contentsMargins().left();
		fvcWidth -= FolderView->IconView->contentsMargins().right();
		fvcWidth -= FolderView->IconView->autoScrollMargin();
		fvcWidth -= FolderView->IconView->verticalScrollBar()->width();
	}

	FolderView->setContentsWidth( fvcWidth );
	FolderView->updateViewMode();

	FolderView->updateViewMode();
};

void NewBreeze::switchToNextView() {

	QStringList viewModes = QStringList() << "SmallListView" << "NormalListView"<< "TilesView";
	viewModes << "SmallIconsView" << "NormalIconsView" << "LargeIconsView" << "HugeIconsView";
	viewModes << "SDetailsView" << "NDetailsView";

	QString currentMode = Settings.General.FolderView;

	int curIndex = viewModes.indexOf( currentMode );
	int newIndex = ( curIndex == ( viewModes.size() - 1 ) ) ? 0 : curIndex + 1;

	Settings.General.FolderView = viewModes.at( newIndex );
	settings.setValue( "FolderView", viewModes.at( newIndex ) );
	settings.sync();

	int fvcWidth = FolderView->size().width();

	if ( FolderView->currentIndex() ) {
		fvcWidth -= FolderView->TreeView->contentsMargins().left();
		fvcWidth -= FolderView->TreeView->contentsMargins().right();
		fvcWidth -= FolderView->TreeView->autoScrollMargin();
		fvcWidth -= FolderView->TreeView->verticalScrollBar()->width();
	}
	else {
		fvcWidth -= FolderView->IconView->contentsMargins().left();
		fvcWidth -= FolderView->IconView->contentsMargins().right();
		fvcWidth -= FolderView->IconView->autoScrollMargin();
		fvcWidth -= FolderView->IconView->verticalScrollBar()->width();
	}

	FolderView->setContentsWidth( fvcWidth );
	FolderView->updateViewMode();

	FolderView->updateViewMode();
};
