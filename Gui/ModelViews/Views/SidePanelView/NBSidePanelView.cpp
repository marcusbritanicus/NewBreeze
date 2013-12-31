/*
	*
	* NBSidePanel.cpp - The side panel showing My Computer ( All the mounted drives )
	*
*/

#include <NBSidePanelView.hpp>

NBSidePanel::NBSidePanel() : QWidget() {

	setupView();
};

void NBSidePanel::setupView() {

	setFixedWidth( 48 );
	setStyleSheet( getStyleSheet( "NBSidePanel", Settings->General.Style ) );

	QStringList icons;
	icons << ":/icons/folder.png" << ":/icons/applications.png"  << ":/icons/catalogs.png" << ":/icons/comp.png" << ":/icons/bookmark.png";

	QVBoxLayout *iconsLyt = new QVBoxLayout();
	iconsLyt->setContentsMargins( QMargins() );
	iconsLyt->setSpacing( 0 );

	Q_FOREACH( QString icon, icons ) {
		NBClickLabel *lbl = new NBClickLabel( QPixmap( icon ).scaled( QSize( 32, 32 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
		lbl->setFixedSize( QSize( 48, 48 ) );

		iconsLyt->addWidget( lbl );
	}
	iconsLyt->addStretch( 0 );

	QVBoxLayout *lyt = new QVBoxLayout();
	lyt->setContentsMargins( QMargins() );
	lyt->setSpacing( 0 );

	QWidget *base = new QWidget();
	base->setObjectName( "guiBase" );
	base->setLayout( iconsLyt );
	lyt->addWidget( base );

	setLayout( lyt );
};

void NBSidePanel::updateBookmarks() {

};

void NBSidePanel::updateDevices() {

};

void NBSidePanel::handleClick( const QModelIndex ) {

};

void NBSidePanel::dragEnterEvent( QDragEnterEvent* ) {

};

void NBSidePanel::dragMoveEvent( QDragMoveEvent* ) {

};

void NBSidePanel::dropEvent( QDropEvent* ) {

};
