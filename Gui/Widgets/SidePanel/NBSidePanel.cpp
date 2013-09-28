/*
	*
	* NBSidePanel.cpp - The side panel showing My Computer ( All the mounted drives )
	*
*/

#include <NBSidePanel.hpp>
#include <NBSPDeviceView.hpp>
#include <NBSPBookmarksView.hpp>
#include <NBShowHideWidget.hpp>
#include <NBTools.hpp>

NBSidePanel::NBSidePanel() : QWidget() {

	createGUI();
	setPanelProperties();
};

void NBSidePanel::createGUI() {

	// No Focus
	setFocusPolicy( Qt::NoFocus );

	NBSPDeviceView *deviceView = new NBSPDeviceView();
	NBShowHideWidget *deviceBase = new NBShowHideWidget( "Devices", deviceView );

	NBSPBookmarksView *bookmarksView = new NBSPBookmarksView();
	NBShowHideWidget *bookmarksBase = new NBShowHideWidget( "Bookmarks", bookmarksView );

	connect( deviceView, SIGNAL( mountDrive( QString ) ), this, SIGNAL( driveClicked( QString) ) );
	connect( bookmarksView, SIGNAL( mountDrive( QString ) ), this, SIGNAL( driveClicked( QString) ) );

	int devWidth = deviceView->sizeHint().width();
	int bmkWidth = bookmarksView->sizeHint().width();

	if ( devWidth > bmkWidth )
		sidePanelWidth = devWidth > 200 ? 200 : devWidth;

	else
		sidePanelWidth = bmkWidth > 200 ? 200 : bmkWidth;

	// Layout
	QVBoxLayout *scrollLyt = new QVBoxLayout();
	scrollLyt->setContentsMargins( QMargins() );
	scrollLyt->addWidget( deviceBase );
	scrollLyt->addWidget( bookmarksBase );
	scrollLyt->addStretch( 0 );

	QWidget *base = new QWidget();
	base->setContentsMargins( QMargins() );
	base->setLayout( scrollLyt );

	// Static Scroller
	QScrollArea *scroll = new QScrollArea();
	scroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	scroll->setContentsMargins( QMargins() );
	scroll->setWidgetResizable( true );
	scroll->setWidget( base );
	// No Focus
	scroll->setFocusPolicy( Qt::NoFocus );

	QVBoxLayout *sidePanelLyt = new QVBoxLayout();
	sidePanelLyt->addWidget( scroll );

	setLayout( sidePanelLyt );
};

void NBSidePanel::setPanelProperties() {

	setStyleSheet( getStyleSheet( "NBSidePanel", Settings.General.Style ) );
	setFixedWidth( sidePanelWidth + 30 );

	setMouseTracking( true );
};
