/*
	*
	* NBIOManager.cpp - The NBIOManager class for NewBreeze
	*
*/

#include <NBIOManager.hpp>

NBIOManager::NBIOManager() : QFrame() {

	checked = false;
	jobList.clear();

	totalF = 0;

	painter = new QPainter();
	setFixedSize( QSize( 128, 52 ) );

	timer = new QTimer();
	connect( timer, SIGNAL( timeout() ), this, SLOT( manageTimer() ) );
	connect( timer, SIGNAL( timeout() ), this, SLOT( updateProgress() ) );
};

NBIOManager::~NBIOManager() {

	if ( painter->isActive() )
		painter->end();

	delete painter;
};

void NBIOManager::addJob( QStringList sourceList, QString target, NBIOMode::Mode iomode ) {

	NBPasteDialog *pasteDialog = new NBPasteDialog();
	pasteDialog->setSources( sourceList );
	pasteDialog->setTarget( target );
	pasteDialog->setIOMode( iomode );

	connect( pasteDialog, SIGNAL( IOComplete() ), this, SLOT( handleJobComplete() ) );

	jobList << pasteDialog;

	QTimer::singleShot( 100, pasteDialog, SLOT( startWork() ) );

	manageTimer();
};

quint64 NBIOManager::activeJobs() {

	return jobList.count();
};

void NBIOManager::showAllIODialogs() {

	foreach( NBPasteDialog *pDialog, jobList )
		pDialog->show();
};

void NBIOManager::paintEvent( QPaintEvent *pEvent ) {

	painter->begin( this );
	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	painter->setFont( QFont( "Courier 10 Pitch", 10 ) );

	// Draw the frame border
	painter->setPen( Qt::gray );
	painter->setBrush( checked ? QColor( 100, 100, 100, 72 ) : QColor( 0, 0, 0, 72 ) );
	painter->drawRoundedRect( 0, 0, 128, 52, 9, 9 );

	quint64 jobs = jobList.count();

	if ( not jobs ) {
		painter->setBrush( Qt::transparent );
		if ( Settings->General.Style == QString( "TransDark" ) or Settings->General.Style == QString( "DullBlack" ) )
			painter->setPen( Qt::white );

		else
			painter->setPen( Qt::black );

		painter->drawText( 0, 0, 128, 52, Qt::AlignCenter, QString( "No active Jobs" ) );
	}

	else {
		// Draw text indicating total active jobs
		painter->setBrush( Qt::transparent );
		if ( Settings->General.Style == QString( "TransDark" ) or Settings->General.Style == QString( "DullBlack" ) )
			painter->setPen( Qt::white );

		else
			painter->setPen( Qt::black );

		painter->drawText( 0, 6, 128, 17, Qt::AlignCenter, QString( "%1 active Job%2" ).arg( jobs ).arg( ( jobs == 1 ) ? "" : "s" ) );

		// Draw the total progress indicator
		painter->setPen( Qt::gray );
		painter->setBrush( Qt::darkGreen );
		painter->drawRoundedRect( 5, 29, 118 * totalF, 17, 5, 5 );
	}

	painter->end();
	pEvent->accept();
};

void NBIOManager::mousePressEvent( QMouseEvent *mEvent ) {

	if ( checked ) {
		foreach( NBPasteDialog *pDialog, jobList )
			pDialog->hide();

		checked = false;
		repaint();
	}

	else {
		if ( jobList.count() ) {
			foreach( NBPasteDialog *pDialog, jobList )
				pDialog->show();

			checked = true;
			repaint();
		}
	}

	mEvent->accept();
};

void NBIOManager::uncheck() {

	checked = false;
	repaint();
};

void NBIOManager::manageTimer() {

	// If there are jobs still running,
	if ( activeJobs() ) {
		// And timer is not active, activate it
		if ( not timer->isActive() )
			timer->start( 1000 );
	}

	// If there are no active jobs,
	else {
		// And timer is active, halt it
		if ( timer->isActive() )
			timer->stop();
	}
};

void NBIOManager::updateProgress() {

	qreal totalP = 0;
	if ( activeJobs() ) {
		foreach( NBPasteDialog *pDialog, jobList ) {
			totalP += pDialog->progress();
		}
		totalF = ( totalP / 100.0 ) / jobList.count();
	}

	manageTimer();
	repaint();
};

void NBIOManager::handleJobComplete() {

	NBPasteDialog *pDialog = qobject_cast<NBPasteDialog*>( sender() );
	pDialog->close();
	QStringList errors = pDialog->errorNodes();
	NBIOMode::Mode mode = pDialog->ioMode();

	if ( errors.count() ) {
		QString title = QString( "Error %1 files" ).arg( ( mode == NBIOMode::Copy ) ? "copying" : "moving" );
		QString text = QString(
			"<p>Some errors were encountered while %1 the files. Please check the copied data.</p>"		\
			"<p>These errors were mostly caused due to insufficient permissions or invalid characters "	\
			"in files names. You can rectify these problems and try again.</p>"
		).arg( ( mode == NBIOMode::Copy ) ? "copying" : "moving" );

		QListWidget *errorList = new QListWidget();
		errorList->setFocusPolicy( Qt::NoFocus );
		errorList->setIconSize( QSize( 32, 32 ) );

		foreach( QString errorNode, errors ) {
			QString iconName = NBIconProvider::icon( errorNode );
			QListWidgetItem *item = new QListWidgetItem( QIcon::fromTheme( iconName, QIcon( iconName ) ), errorNode );
			errorList->addItem( item );
		}

		NBMessageDialog::error( title, text, QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Ok, errorList );
	}

	pDialog->deleteLater();
	jobList.removeOne( pDialog );

	manageTimer();
	repaint();
};
