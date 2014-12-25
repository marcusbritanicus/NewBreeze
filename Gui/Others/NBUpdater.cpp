/*
	*
	* NBUpdater.cpp - NewBreeze Update Various
	*
*/

#include <NewBreezeUI.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>

void NewBreezeUI::updateVarious( QString url ) {

	QString folder = ( ( url == "/" ) ? QString() : url.split( "/", QString::SkipEmptyParts ).takeLast() );
	setWindowTitle( ( folder.isEmpty() ? "/ - NewBreeze" : QString( "%1 - NewBreeze" ).arg( folder ) ) );

	AddressBar->addressWidget->addressEdit->setText( url );
	AddressBar->addressWidget->crumbsBar->setCurrentDirectory( url );

	if ( not url.startsWith( "NB://" ) )
		Terminal->changeDir( url );
};

void NewBreezeUI::updateQuickMenuBar() {

	if ( FolderView->hasSelection() ) {
		QuickMenuBar->quickBtns->setSegmentEnabled( 0 );
		QuickMenuBar->quickBtns->setSegmentEnabled( 2 );
		QuickMenuBar->quickBtns->setSegmentEnabled( 3 );
		QuickMenuBar->quickBtns->setSegmentEnabled( 4 );
	}

	else {
		QuickMenuBar->quickBtns->setSegmentDisabled( 0 );
		QuickMenuBar->quickBtns->setSegmentDisabled( 1 );
		QuickMenuBar->quickBtns->setSegmentDisabled( 2 );
	}
};

void NewBreezeUI::updateInfoBar() {

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

void NewBreezeUI::changeViewMode() {

	QStringList viewModes = QStringList() << "TilesView" << "IconsView" << "DetailsView";
	Settings->General.FolderView = viewModes.at( AddressBar->viewModeBtn->checkedAction() );

	FolderView->updateViewMode();
};

void NewBreezeUI::switchToNextView() {

	if ( Settings->General.FolderView == QString( "TilesView" ) )
		Settings->General.FolderView = QString( "IconsView" );

	else if ( Settings->General.FolderView == QString( "IconsView" ) )
		Settings->General.FolderView = QString( "DetailsView" );

	else
		Settings->General.FolderView = QString( "TilesView" );

	FolderView->updateViewMode();
};

void NewBreezeUI::toggleGrouping() {

	if ( Settings->Session.SortCategory )
		Settings->Session.SortCategory = false;

	else
		Settings->Session.SortCategory = true;

	FolderView->fsModel->setCategorizationEnabled( Settings->Session.SortCategory );
	FolderView->groupsAct->setChecked( Settings->Session.SortCategory );
};
