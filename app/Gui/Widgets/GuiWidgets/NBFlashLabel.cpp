/*
	*
	* NBFlashLabel.cpp - A highly customized QLabel for NewBreeze
	*
*/

#include "NBFlashLabel.hpp"

NBFlashLabel::NBFlashLabel( QWidget *parent ) : QWidget( parent ) {


	mPixmap = QPixmap();
	mText = QString();

	alpha = 0;
	mAlphaDelta = 30;
	color = QColor( 255, 255, 255, 0 );

	currentStep = 0;
	flashSteps = 10;
	mFlashDuration = 200;

	flash = false;
	flashesCompleted = 0;
	maxFlashes = 2;

	timer.setInterval( mFlashDuration / flashSteps );
	timer.setSingleShot( false );

	connect( &timer, SIGNAL( timeout() ), this, SLOT( repaint() ) );
};

/* Override the QLabel pixmap property handlers */
QPixmap NBFlashLabel::pixmap() {

	return mPixmap;
};

void NBFlashLabel::setPixmap( QPixmap newPixmap ) {

	mPixmap = newPixmap;
	mText = QString();

	repaint();
};

/* Color property handlers */
QColor NBFlashLabel::flashColor() {

	return color;
};

void NBFlashLabel::setFlashColor( QColor newColor ) {

	color = newColor;
	color.setAlpha( alpha );

	repaint();
};

/* Alpha property handlers */
qint64 NBFlashLabel::alphaDelta() {

	return mAlphaDelta;
};

void NBFlashLabel::setAlphaDelta( qint64 newAlphaDelta ) {

	if ( ( newAlphaDelta * 10 ) > 255 )
		mAlphaDelta = 25;

	else
		mAlphaDelta = newAlphaDelta;
};

/* Flash duration property handlers */
int NBFlashLabel::flashDuration() {

	return mFlashDuration;
};

void NBFlashLabel::setFlashDuration( int newFlashDuration ) {

	mFlashDuration = ( newFlashDuration >= 10 ? newFlashDuration : 10 );
	timer.setInterval( mFlashDuration / flashSteps );
};

/* Number of Flashes property handlers */
int NBFlashLabel::numberOfFlashes() {

	return maxFlashes;
};

void NBFlashLabel::setNumberOfFlashes( int newNumOfFlashes ) {

	maxFlashes = newNumOfFlashes;
};

/* flashFrames property handlers - Number of frames per flash */
int NBFlashLabel::flashFrames() {

	return flashSteps;
};

void NBFlashLabel::setFlashFrames( int newFlashSteps ) {

	/* Persistence of vision is 62.5 (~63) ms */

	flashSteps = ( newFlashSteps > mFlashDuration ? mFlashDuration / 63 : newFlashSteps );
	timer.setInterval( mFlashDuration / flashSteps );
};

/* Overriding of paint event for showing flashes */
void NBFlashLabel::paintEvent( QPaintEvent *pEvent ) {

	if ( flash ) {
		if ( flashesCompleted == maxFlashes ) {
			timer.stop();
			flash = false;
			currentStep = 0;
			alpha = 0;
			flashesCompleted = 0;
		}

		else if ( currentStep == flashSteps ) {
			flashesCompleted += 1;
			currentStep = 0;
		}

		else if ( currentStep < flashSteps / 2 ) {
			currentStep += 1;
			alpha += mAlphaDelta;
			color.setAlpha( alpha );
		}

		else if ( currentStep >= ( flashSteps / 2 ) ) {
			currentStep += 1;
			alpha -= mAlphaDelta;
			color.setAlpha( alpha );
		}
	}

	QPainter *painter = new QPainter( this );
	painter->save();

	painter->setPen( Qt::transparent );
	painter->setBrush( color );
	painter->drawRect( 0, 0, width(), height() );

	if ( not mPixmap.isNull() ) {
		painter->setBrush( Qt::transparent );
		painter->drawPixmap( 8, 8, 32, 32, mPixmap );
	}

	else if ( not ( mText.isNull() or  mText.isEmpty() ) ) {
		painter->setBrush( Qt::transparent );
		painter->drawText( 0, 0, width(), height(), Qt::AlignCenter | Qt::TextWordWrap, mText );
	}

	painter->restore();
	painter->end();

	pEvent->accept();
};

/* Overriding QLabel::mousePressEvent to emit clicked signal */
void NBFlashLabel::mousePressEvent( QMouseEvent *mEvent ) {

	emit clicked();
	mEvent->accept();
};

/* Overriding QLabel::enterEvent to emit entered signal */
void NBFlashLabel::enterEvent( QEvent *mEvent ) {

	emit entered();
	mEvent->accept();
};

/* Overriding QLabel::leaveEvent to emit exited signal */
void NBFlashLabel::leaveEvent( QEvent *lEvent ) {

	emit exited();
	lEvent->accept();
};

/* Slot to access the flashing */
void NBFlashLabel::flashLabel() {

	if ( colorFlash ) {
		colorFlash = false;
		color = QColor( 255, 255, 255, 0 );
	}

	if ( timer.isActive() )
		timer.stop();

	timer.start();
	flash = true;
};

/* Slot to access the flashing with a given color */
void NBFlashLabel::flashLabel( QColor newColor ) {

	setFlashColor( newColor );

	if ( timer.isActive() )
		timer.stop();

	timer.start();
	flash = true;
};

NBTrashLabel::NBTrashLabel( QWidget *parent ) : QWidget( parent ) {

	mPixmap = QPixmap();
	color = QColor( 255, 0, 0, 150 );

	currentStep = 0;
	flashSteps = 50;
	radius = 0;

	flash = false;

	setAcceptDrops( true );
	mPixmap = QIcon::fromTheme( "user-trash" ).pixmap( 32 );

	timer.setInterval( 5 );
	timer.setSingleShot( false );

	connect( &timer, SIGNAL( timeout() ), this, SLOT( repaint() ) );
};

/* Overriding of paint event for showing flashes */
void NBTrashLabel::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );
	painter->save();

	painter->fillRect( QRect( 0, 0, width(), height() ), Qt::transparent );

	QRadialGradient bgGradient( 24, 24, radius );
	if ( flash ) {
		if ( currentStep == flashSteps ) {
			timer.stop();
			flash = false;
			currentStep = 0;
			radius = 0;

			bgGradient.setColorAt( 0, QColor( Qt::transparent ) );
			bgGradient.setColorAt( 1, QColor( Qt::transparent ) );
		}

		else if ( currentStep < flashSteps ) {
			currentStep += 1;
			radius += 3;

			bgGradient.setRadius( radius );
			bgGradient.setColorAt( 0, QColor( Qt::transparent ) );
			bgGradient.setColorAt( 0.85, color );
			bgGradient.setColorAt( 1, QColor( Qt::transparent ) );
		}

		painter->setPen( Qt::transparent );
		painter->setBrush( bgGradient );
		painter->drawEllipse( QPoint( 24, 24 ), radius, radius );
	}

	painter->setBrush( Qt::transparent );
	painter->drawPixmap( 8, 8, 32, 32, mPixmap );

	painter->restore();
	painter->end();

	pEvent->accept();
};

/* Overriding QLabel::mousePressEvent to emit clicked signal */
void NBTrashLabel::mousePressEvent( QMouseEvent *mEvent ) {

	emit clicked();
	mEvent->accept();
};

void NBTrashLabel::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

/* Drop event handler for the delete label */
void NBTrashLabel::dropEvent( QDropEvent *dpEvent ) {

	const QMimeData *mData = dpEvent->mimeData();
	if ( mData->hasUrls() ) {
		dpEvent->accept();

		/* Check if we have protection set */
		QSettings nbSettings( "NewBreeze", "NewBreeze" );
		QStringList safeNodes = nbSettings.value( "ProtectedNodes" ).toStringList();

		QStringList toBeDeleted;
		Q_FOREACH( QUrl url, mData->urls() ) {
			QString path = url.toLocalFile();
			if ( not safeNodes.contains( path ) )
				toBeDeleted << path.replace( dirName( path ), "" );
		}

		if ( dpEvent->keyboardModifiers() == Qt::NoModifier ) {

			color = QColor( Qt::darkYellow );
			color.setAlpha( 150 );

			NBProcess::Progress *progress = new NBProcess::Progress;
			progress->sourceDir = dirName( mData->urls().at( 0 ).toLocalFile() );
			progress->targetDir = QString();
			progress->type = NBProcess::Trash;

			NBDeleteProcess *proc = new NBDeleteProcess( toBeDeleted, progress );
			NBProcessManager::instance()->addProcess( progress, proc );

			progress->startTime = QTime::currentTime();
			proc->start();

			flashLabel();
		}

		else if ( dpEvent->keyboardModifiers() == Qt::ShiftModifier ) {

			color = QColor( Qt::red );
			color.setAlpha( 150 );

			NBConfirmDeleteDialog *deleteMsg = new NBConfirmDeleteDialog( dirName( toBeDeleted.at( 0 ) ), toBeDeleted, true );
			if ( not deleteMsg->exec() )
				return;

			NBProcess::Progress *progress = new NBProcess::Progress;
			progress->sourceDir = dirName( mData->urls().at( 0 ).toLocalFile() );
			progress->targetDir = QString();
			progress->type = NBProcess::Delete;

			NBDeleteProcess *proc = new NBDeleteProcess( toBeDeleted, progress );
			NBProcessManager::instance()->addProcess( progress, proc );

			progress->startTime = QTime::currentTime();
			proc->start();

			flashLabel();
		}

		else {

			dpEvent->ignore();
			return;
		}
	}

	dpEvent->ignore();
};

/* Slot to handle the deletion failures */
void NBTrashLabel::handleDeleteFailure( QStringList files, QStringList dirs ) {

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

	NBMessageDialog::error( this,
		"NewBreeze - Error while deleting",
		"Some errors were encountered while deleting the files and folders you requested. "			\
		"As a result, some of the files and folders may not have been deleted. For the "			\
		"list for files and folder not deleted click <u>M</u>ore"
	);
};

/* Slot to access the flashing */
void NBTrashLabel::flashLabel() {

	if ( timer.isActive() )
		timer.stop();

	timer.start();
	flash = true;
};
