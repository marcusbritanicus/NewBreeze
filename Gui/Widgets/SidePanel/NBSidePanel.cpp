/*
	*
	* NBSidePanel.cpp - The side panel showing My Computer ( All the mounted drives )
	*
*/

#include <NBSidePanel.hpp>

/* Side Panel init */
NBSidePanel::NBSidePanel() : QWidget() {

	setFixedWidth( 48 );

	setupPanel();
	setMouseTracking( true );

	/* Only way to expand this panel when dragging and dropping */
	setAcceptDrops( true );

	/* Update the devices list to set the maxWidth of @v spView */
	vSpacer->setFixedHeight( 192 );
	spView->updateDevices();
	maxWidth = 48 + spView->idealWidth();
};

/* Forcibly expand and show the devices/bookmarks */
void NBSidePanel::expand() {

	if ( not contractingTimer->isActive() )
		expandingTimer->start();
};

/* Forcibly expand and show the devices/bookmarks */
void NBSidePanel::contract() {

	if ( not expandingTimer->isActive() )
		contractingTimer->start();
};

void NBSidePanel::setupPanel() {

	setStyleSheet( getStyleSheet( "NBSidePanel", Settings->General.Style ) );

	/* The 'Folders' Label */
	dirLbl = new NBFlashLabel();
	dirLbl->setPixmap( QPixmap( ":/icons/folder.png" ).scaled( QSize( 32, 32 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	dirLbl->setFixedSize( QSize( 48, 48 ) );
	dirLbl->setToolTip( "Show folders" );
	connect( dirLbl, SIGNAL( clicked() ), this, SIGNAL( showFolders() ) );

	/* The 'Applications' Label */
	appLbl = new NBFlashLabel();
	appLbl->setPixmap( QPixmap( ":/icons/applications.png" ).scaled( QSize( 32, 32 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	appLbl->setFixedSize( QSize( 48, 48 ) );
	appLbl->setToolTip( "Show Applications" );
	connect( appLbl, SIGNAL( clicked() ), this, SIGNAL( showApplications() ) );

	/* The 'Catalogs' Label */
	ctlLbl = new NBFlashLabel();
	ctlLbl->setPixmap( QPixmap( ":/icons/catalogs.png" ).scaled( QSize( 32, 32 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	ctlLbl->setFixedSize( QSize( 48, 48 ) );
	ctlLbl->setToolTip( "Show Catalogs" );
	connect( ctlLbl, SIGNAL( clicked() ), this, SIGNAL( showCatalogs() ) );

	/* The 'Devices' Label */
	devLbl = new NBFlashLabel();
	devLbl->setPixmap( QPixmap( ":/icons/comp.png" ).scaled( QSize( 32, 32 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	devLbl->setFixedSize( QSize( 48, 48 ) );
	devLbl->setObjectName( "special" );
	connect( devLbl, SIGNAL( clicked() ), this, SLOT( toggleExpandContract() ) );
	connect( devLbl, SIGNAL( entered() ), this, SLOT( updateDevices() ) );

	/* The 'BookMarks' Label */
	bmkLbl = new NBFlashLabel();
	bmkLbl->setPixmap( QPixmap( ":/icons/bookmark.png" ).scaled( QSize( 32, 32 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	bmkLbl->setFixedSize( QSize( 48, 48 ) );
	bmkLbl->setObjectName( "special" );
	connect( bmkLbl, SIGNAL( clicked() ), this, SLOT( toggleExpandContract() ) );
	connect( bmkLbl, SIGNAL( entered() ), this, SLOT( updateBookmarks() ) );

	/* The 'Delete!' Label */
	deleteLbl = new NBDeleteLabel();
	deleteLbl->setFixedSize( QSize( 48, 48 ) );
	deleteLbl->setToolTip( "Show TrashCan" );
	connect( deleteLbl, SIGNAL( clicked() ), this, SIGNAL( showTrash() ) );

	/* Icons Layout */
	QVBoxLayout *iconsLyt = new QVBoxLayout();
	iconsLyt->setContentsMargins( QMargins() );
	iconsLyt->setSpacing( 0 );

	iconsLyt->addWidget( dirLbl );
	iconsLyt->addWidget( appLbl );
	iconsLyt->addWidget( ctlLbl );
	iconsLyt->addWidget( devLbl );
	iconsLyt->addWidget( bmkLbl );

	iconsLyt->addStretch( 0 );
	iconsLyt->addWidget( deleteLbl );

	/* View Layout */
	vSpacer = NBSpacer::vertical( 144 );
	spView = new NBSidePanelView();
	spView->resize( 0, spView->height() );

	connect( spView, SIGNAL( driveClicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
	connect( spView, SIGNAL( copy( QStringList, QString, NBIOMode::Mode ) ), this, SIGNAL( copy( QStringList, QString, NBIOMode::Mode ) ) );
	connect( spView, SIGNAL( move( QStringList, QString, NBIOMode::Mode ) ), this, SIGNAL( move( QStringList, QString, NBIOMode::Mode ) ) );
	connect( spView, SIGNAL( showingMenu() ), this, SLOT( toggleExpandContract() ) );
	connect( spView, SIGNAL( hidingMenu() ), this, SLOT( toggleExpandContract() ) );
	connect( spView, SIGNAL( bookmarkRemoved() ), this, SLOT( updateBookmarks() ) );

	QVBoxLayout *viewLyt = new QVBoxLayout();
	viewLyt->setContentsMargins( QMargins() );
	viewLyt->setSpacing( 0 );

	viewLyt->addWidget( vSpacer );
	viewLyt->addWidget( spView );
	viewLyt->addStretch( 0 );

	/* Layout for iconsLyt and viewLyt */
	QHBoxLayout *multiLyt = new QHBoxLayout();
	multiLyt->setContentsMargins( QMargins() );
	multiLyt->setSpacing( 0 );
	multiLyt->addLayout( iconsLyt );
	multiLyt->addLayout( viewLyt );

	/* Base Widget */
	QWidget *base = new QWidget();
	base->setObjectName( "guiBase" );
	base->setLayout( multiLyt );

	/* Base Layout */
	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setContentsMargins( QMargins() );
	lyt->setSpacing( 0 );
	lyt->addWidget( base );

	setLayout( lyt );
	setFixedWidth( 48 );

	/* Expanding Animation Timer */
	expandingTimer = new QTimer();
	expandingTimer->setInterval( 1 );
	connect( expandingTimer, SIGNAL( timeout() ), this, SLOT( increaseWidth() ) );

	/* Contracting Animation Timer */
	contractingTimer = new QTimer();
	contractingTimer->setInterval( 1 );
	connect( contractingTimer, SIGNAL( timeout() ), this, SLOT( decreaseWidth() ) );
};

void NBSidePanel::flashDevices() {

	devLbl->flashLabel();
	spView->updateDevices();
};

void NBSidePanel::flashBookmarks() {

	bmkLbl->flashLabel();
	spView->updateBookmarks();
};

void NBSidePanel::flashFolders() {

	dirLbl->flashLabel();
};

void NBSidePanel::flashCatalogs() {

	ctlLbl->flashLabel();
};

void NBSidePanel::flashApplications() {

	appLbl->flashLabel();
};

void NBSidePanel::updateDevices() {

	showingBookMarks = false;
	showingDevices = true;

	vSpacer->setFixedHeight( 192 );
	spView->updateDevices();

	maxWidth = 48 + spView->idealWidth();
	repaint();

	if ( not expandingTimer->isActive() )
		expand();
};

void NBSidePanel::updateBookmarks() {

	showingBookMarks = true;
	showingDevices = false;

	vSpacer->setFixedHeight( 241 );
	spView->updateBookmarks();

	maxWidth = 48 + spView->idealWidth();
	repaint();

	if ( not expandingTimer->isActive() )
		expand();
};

void NBSidePanel::toggleExpandContract() {

	NBFlashLabel *lbl = qobject_cast<NBFlashLabel*>( sender() );
	Q_UNUSED( lbl );

	forcedOpen = not forcedOpen;
	forcedOpen ? expand() : contract();

	if ( forcedOpen ) {
		if ( lbl == devLbl )
			updateDevices();

		else
			updateBookmarks();
	}
};

void NBSidePanel::increaseWidth() {

	int addWidth = ( spView->idealWidth() - spView->width() ) / 2;

	if ( addWidth <= 0 ) {
		setFixedWidth( maxWidth );
		spView->setFixedWidth( spView->idealWidth() );
		expandingTimer->stop();
		return;
	}

	spView->setFixedWidth( spView->width() + addWidth );
	setFixedWidth( width() + addWidth );
	repaint();
};

void NBSidePanel::decreaseWidth() {

	int delWidth = ( spView->width() ) / 2;

	if ( delWidth <= 0 ) {
		setFixedWidth( 48 );
		spView->setFixedWidth( 0 );
		contractingTimer->stop();
		return;
	}

	setFixedWidth( width() - delWidth );
	spView->setFixedWidth( spView->width() - delWidth );
};

bool NBSidePanel::event( QEvent *evnt ) {

	if ( evnt->type() == QEvent::MouseMove ) {
		if ( forcedOpen )
			return true;

		QPoint curPos = mapFromGlobal( QCursor::pos() );

		if ( curPos.y() < 144 )
			contract();

		else if ( curPos.y() > 240 ) {
			if ( childAt( curPos ) != spView )
				contract();
		}

		evnt->accept();
		return true;
	}

	return QWidget::event( evnt );
};

void NBSidePanel::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->accept();

	if ( forcedOpen )
		return;

	spView->setFixedWidth( spView->idealWidth() );
	setFixedWidth( maxWidth );
	forcedOpen = true;
};

void NBSidePanel::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	dmEvent->ignore();

	if ( qobject_cast<NBFlashLabel*>( childAt( dmEvent->pos() ) ) == bmkLbl )
		updateBookmarks();

	if ( qobject_cast<NBFlashLabel*>( childAt( dmEvent->pos() ) ) == devLbl )
		updateDevices();

	spView->setFixedWidth( spView->idealWidth() );
	setFixedWidth( maxWidth );
};

void NBSidePanel::dragLeaveEvent( QDragLeaveEvent *dlEvent ) {

	dlEvent->accept();

	forcedOpen = false;

	if ( QCursor::pos().x() > maxWidth )
		contract();
};

void NBSidePanel::enterEvent( QEvent* eEvent ) {

	if ( forcedOpen )
		return;

	if ( qobject_cast<NBFlashLabel*>( childAt( mapFromGlobal( QCursor::pos() ) ) ) == bmkLbl ) {
		updateBookmarks();
		expand();
	}

	else if ( qobject_cast<NBFlashLabel*>( childAt( mapFromGlobal( QCursor::pos() ) ) ) == devLbl ) {
		updateDevices();
		expand();
	}

	eEvent->accept();
};

void NBSidePanel::leaveEvent( QEvent *lEvent ) {

	if ( forcedOpen )
		return;

	contract();
	lEvent->accept();
};

void NBSidePanel::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );
	painter->save();

	if ( showingDevices ) {
		QLinearGradient hLine( QPoint( 0, 144 ), QPoint( width(), 144 ) );
		hLine.setColorAt( 0, QColor( Qt::gray ) );
		hLine.setColorAt( 1, Qt::transparent );

		QBrush brush( hLine );
		QPen hPen( brush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );

		painter->setPen( hPen );
		painter->drawLine( QPoint( 48, 144 ), QPoint( width(), 144 ) );
		painter->drawLine( QPoint( 48, 191 ), QPoint( width(), 191 ) );

		painter->setPen( Qt::white );
		painter->drawText( 48, 144, spView->idealWidth(), 48, Qt::AlignCenter, tr( "Devices" ) );
	}

	if ( showingBookMarks ) {
		QLinearGradient hLine( QPoint( 0, 192 ), QPoint( width(), 192 ) );
		hLine.setColorAt( 0, QColor( Qt::gray ) );
		hLine.setColorAt( 1, Qt::transparent );

		QBrush brush( hLine );
		QPen hPen( brush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );

		painter->setPen( hPen );
		painter->drawLine( QPoint( 48, 192 ), QPoint( width(), 192 ) );
		painter->drawLine( QPoint( 48, 239 ), QPoint( width(), 239 ) );

		painter->setPen( Qt::white );
		painter->drawText( 48, 192, spView->idealWidth(), 48, Qt::AlignCenter, tr( "Bookmarks" ) );
	}

	painter->restore();
	painter->end();

	QWidget::paintEvent( pEvent);

	pEvent->accept();
};
