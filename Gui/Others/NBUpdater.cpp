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

	Settings->General.FolderView = viewModes.at( AddressBar->viewModeBtn->checkedAction() );

	Settings->setValue( "FolderView", Settings->General.FolderView );
	Settings->reload();

	FolderView->updateViewMode();
};

void NewBreeze::switchToNextView() {

	QStringList viewModes = QStringList() << "SmallListView" << "NormalListView"<< "TilesView";
	viewModes << "SmallIconsView" << "NormalIconsView" << "LargeIconsView" << "HugeIconsView";
	viewModes << "SDetailsView" << "NDetailsView";

	QString currentMode = Settings->General.FolderView;

	int curIndex = viewModes.indexOf( currentMode );
	int newIndex = ( curIndex == ( viewModes.size() - 1 ) ) ? 0 : curIndex + 1;

	Settings->General.FolderView = viewModes.at( newIndex );
	Settings->setValue( "FolderView", viewModes.at( newIndex ) );
	Settings->reload();

	FolderView->updateViewMode();
};

void NewBreeze::toggleGrouping() {

	if ( Settings->Session.SortCategory ) {
		Settings->setValue( "Session/SortCategory", false );
		Settings->reload();
	}

	else {
		Settings->setValue( "Session/SortCategory", true );
		Settings->reload();
	}

	FolderView->fsModel->setCategorizationEnabled( Settings->Session.SortCategory );
	FolderView->groupsAct->setChecked( Settings->Session.SortCategory );
};
