/*
	*
	* NBArchiveExamine.hpp - NewBreeze ArchiveExamine Classes
	*
*/

#include "NBArchiveExamine.hpp"

NBArchiveExamine::NBArchiveExamine( QString pth, QWidget *parent ) : QDialog( parent ) {

	path = QString( pth );

	createGUI();
	setWindowProperties();
};

void NBArchiveExamine::createGUI() {

	QHBoxLayout *lblBtnLyt = new QHBoxLayout();
	QVBoxLayout *widgetLyt = new QVBoxLayout();
	QVBoxLayout *baseLyt = new QVBoxLayout();

	QLabel *lbl = new QLabel( "<tt><b>" + baseName( path ) + "</b></tt>" );

	QToolButton *openBtn = new QToolButton();
	openBtn->setIcon( QIcon( ":/icons/maximize.png" ) );
	openBtn->setAutoRaise( true );
	openBtn->setFocusPolicy( Qt::NoFocus );

	QToolButton *extractBtn = new QToolButton();
	extractBtn->setIcon( QIcon( ":/icons/extract.png" ) );
	extractBtn->setToolTip( "Extract selected members" );
	extractBtn->setAutoRaise( true );
	extractBtn->setFocusPolicy( Qt::NoFocus );

	QWidget *baseWidget = new QWidget( this );
	baseWidget->setObjectName( tr( "guiBase" ) );

	connect( openBtn, SIGNAL( clicked() ), this, SLOT( openInExternal() ) );
	connect( extractBtn, SIGNAL( clicked() ), this, SLOT( extractSelection() ) );

	peekWidgetBase = new QTreeView( this );
	peekWidgetBase->setObjectName( tr( "previewBase" ) );
	peekWidgetBase->setSelectionBehavior( QAbstractItemView::SelectRows );
	peekWidgetBase->setSelectionMode( QAbstractItemView::ExtendedSelection );

	lblBtnLyt->addWidget( lbl );
	lblBtnLyt->addStretch( 0 );
	lblBtnLyt->addWidget( extractBtn );
	lblBtnLyt->addWidget( openBtn );

	widgetLyt->addLayout( lblBtnLyt );
	widgetLyt->addWidget( peekWidgetBase );

	baseWidget->setLayout( widgetLyt );
	baseLyt->addWidget( baseWidget );
	baseLyt->setContentsMargins( 0, 0, 0, 0 );

	setLayout( baseLyt );
	peekWidgetBase->setFocus();
};

void NBArchiveExamine::setWindowProperties() {

	setFixedSize( 720, 540 );
	setWindowFlags( Qt::FramelessWindowHint );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 540 ) / 2 );

	setGeometry( hpos, vpos, 720, 540 );
};

int NBArchiveExamine::exec() {

	/* Let the archive loading start later */
	QTimer::singleShot( 100, this, SLOT( loadArchive() ) );

	return QDialog::exec();
};

void NBArchiveExamine::openInExternal() {

	QProcess::startDetached( "xdg-open " + path );
	close();
};

void NBArchiveExamine::loadArchive() {

	mdl = new NBArchiveTreeModel( path );
	peekWidgetBase->setModel( mdl );
};

void NBArchiveExamine::keyPressEvent( QKeyEvent *keyEvent ) {

	if ( keyEvent->key() == Qt::Key_Escape )
		close();

	else
		QWidget::keyPressEvent( keyEvent );
};

void NBArchiveExamine::changeEvent( QEvent *event ) {

	if ( ( event->type() == QEvent::ActivationChange ) and ( !isActiveWindow() ) ) {
		hide();
		event->accept();
	}

	else {
		QWidget::changeEvent( event );
		event->accept();
	}
};

void NBArchiveExamine::paintEvent( QPaintEvent *pEvent ) {

	QWidget::paintEvent( pEvent );
	QPainter *painter = new QPainter( this );

	painter->setPen( QPen( palette().color( QPalette::Window ).darker(), 2.0 ) );
	painter->drawRect( rect().adjusted( +1, +1, -2, -2 ) );

	painter->end();
	pEvent->accept();
};

void NBArchiveExamine::extractSelection() {

	QModelIndexList indexes = peekWidgetBase->selectionModel()->selectedIndexes();

	if ( indexes.count() ) {
		Q_FOREACH( QModelIndex idx,  )
			mdl->extract( mdl->nodePath( idx ) );
	}

	else {
		mdl->extractAll();
	}
};
