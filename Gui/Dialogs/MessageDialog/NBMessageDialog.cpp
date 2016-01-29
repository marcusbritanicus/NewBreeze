/*
	*
	* NBMessageDialog.cpp - NBMessageDialog class
	*
*/

#include <NBMessageDialog.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>

NBMessageDialog::NBMessageDialog( QString icon, QString title, QString text, QList<StandardButton> btns, QWidget *info, QWidget *parent ) : NBDialog( parent ) {

	QVBoxLayout *dlgLyt = new QVBoxLayout();
	dlgLyt->setContentsMargins( QMargins( 3, 3, 3, 3 ) );
	dlgLyt->setSpacing( 8 );

	QHBoxLayout *msgLyt = new QHBoxLayout();
	msgLyt->setContentsMargins( QMargins() );
	msgLyt->setSpacing( 5 );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->setContentsMargins( QMargins() );
	dlgLyt->setSpacing( 0 );

	iconLbl = new QLabel();
	iconLbl->setAlignment( Qt::AlignCenter );
	iconLbl->setFixedSize( QSize( 64, 64 ) );
	iconLbl->setPixmap( QPixmap( icon ).scaled( 56, 56, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	textLbl = new QLabel( text );
	textLbl->setWordWrap( true );

	setTitle( title );

	msgLyt->addWidget( iconLbl );
	msgLyt->addWidget( textLbl );

	if ( info ) {
		infoWidget = qobject_cast<QWidget *>( info );
		infoWidget->hide();

		detailsBtn = new NBSegmentControl( this );
		detailsBtn->setSegmentIcon( 0, QIcon( ":/icons/info.png" ) );
		detailsBtn->setSegmentText( 0, tr( "&More..." ) );
		detailsBtn->resize( 100, 0 );
		connect( detailsBtn, SIGNAL( clicked( int ) ), this, SLOT( toggleInfoWidget() ) );
		btnLyt->addWidget( detailsBtn );
	}

	btnLyt->addStretch( 0 );

	segBtns = new NBSegmentControl( this );
	segBtns->setCount( btns.count() );
	connect( segBtns, SIGNAL( clicked( int ) ), this, SLOT(buttonClickHandler( int ) ) );

	for( int i = 0; i < btns.count(); i++ ) {
		switch( btns[ i ] ) {
			case NBMessageDialog::Ok :
				segBtns->setSegmentIcon( i, QIcon( ":/icons/ok.png" ) );
				segBtns->setSegmentText( i, tr( "&Ok" ) );
				break;

			case NBMessageDialog::Yes :
				segBtns->setSegmentIcon( i, QIcon( ":/icons/ok.png" ) );
				segBtns->setSegmentText( i, tr( "&Yes" ) );
				break;

			case NBMessageDialog::YesToAll :
				segBtns->setSegmentIcon( i, QIcon( ":/icons/ok.png" ) );
				segBtns->setSegmentText( i, tr( "Yes to &All" ) );
				break;

			case NBMessageDialog::No :
				segBtns->setSegmentIcon( i, QIcon( ":/icons/delete.png" ) );
				segBtns->setSegmentText( i, tr( "&No" ) );
				break;

			case NBMessageDialog::NoToAll :
				segBtns->setSegmentIcon( i, QIcon( ":/icons/delete.png" ) );
				segBtns->setSegmentText( i, tr( "N&o to All" ) );
				break;

			case NBMessageDialog::Abort :
				segBtns->setSegmentIcon( i, QIcon( ":/icons/abort.png" ) );
				segBtns->setSegmentText( i, tr( "A&bort" ) );
				break;

			case NBMessageDialog::Retry :
				segBtns->setSegmentIcon( i, QIcon( ":/icons/refresh.png" ) );
				segBtns->setSegmentText( i, tr( "&Retry" ) );
				break;

			case NBMessageDialog::Ignore :
				segBtns->setSegmentIcon( i, QIcon( ":/icons/ok.png" ) );
				segBtns->setSegmentText( i, tr( "&Ignore" ) );
				break;

			case NBMessageDialog::Cancel :
				segBtns->setSegmentIcon( i, QIcon::fromTheme( "dialog-cancel" ) );
				segBtns->setSegmentText( i, tr( "&Cancel" ) );
				break;

			default:
				break;
		}
	}

	btnLyt->addWidget( segBtns );
	if ( not btns.count() )
		segBtns->hide();

	dlgLyt->addLayout( msgLyt );
	if ( btns.count() or info ) {
		dlgLyt->addWidget( Separator::horizontal() );
		dlgLyt->addLayout( btnLyt );
	}

	if ( info )
		dlgLyt->addWidget( infoWidget );

	NBDialog::setLayout( dlgLyt );

	updateGeometry();
};

void NBMessageDialog::setIcon( QString icon ) {

	iconLbl->setPixmap( QPixmap( icon ).scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
};

void NBMessageDialog::setTitle( QString title ) {

	NBDialog::setDialogTitle( title );
};

void NBMessageDialog::setText( QString text ) {

	textLbl->setText( text );
};

void NBMessageDialog::setButtons( QList<StandardButton> btns ) {

	if ( btns.count() ) {
		segBtns->show();
		updateGeometry();
		segBtns->setCount( segBtns->count() + btns.count() );
	}

	for( int i = 0; i < btns.count(); i++ ) {
		switch( btns[ i ] ) {
			case NBMessageDialog::Ok :
				segBtns->setSegmentIcon( segBtns->count() + i, QIcon( ":/icons/ok.png" ) );
				segBtns->setSegmentText( segBtns->count() + i, tr( "&Ok" ) );
				break;

			case NBMessageDialog::Yes :
				segBtns->setSegmentIcon( segBtns->count() + i, QIcon( ":/icons/ok.png" ) );
				segBtns->setSegmentText( segBtns->count() + i, tr( "&Yes" ) );
				break;

			case NBMessageDialog::YesToAll :
				segBtns->setSegmentIcon( segBtns->count() + i, QIcon( ":/icons/ok.png" ) );
				segBtns->setSegmentText( segBtns->count() + i, tr( "Yes to &All" ) );
				break;

			case NBMessageDialog::No :
				segBtns->setSegmentIcon( segBtns->count() + i, QIcon( ":/icons/delete.png" ) );
				segBtns->setSegmentText( segBtns->count() + i, tr( "&No" ) );
				break;

			case NBMessageDialog::NoToAll :
				segBtns->setSegmentIcon( segBtns->count() + i, QIcon( ":/icons/delete.png" ) );
				segBtns->setSegmentText( segBtns->count() + i, tr( "N&o to All" ) );
				break;

			case NBMessageDialog::Abort :
				segBtns->setSegmentIcon( segBtns->count() + i, QIcon( ":/icons/abort.png" ) );
				segBtns->setSegmentText( segBtns->count() + i, tr( "A&bort" ) );
				break;

			case NBMessageDialog::Retry :
				segBtns->setSegmentIcon( segBtns->count() + i, QIcon( ":/icons/refresh.png" ) );
				segBtns->setSegmentText( segBtns->count() + i, tr( "&Retry" ) );
				break;

			case NBMessageDialog::Ignore :
				segBtns->setSegmentIcon( segBtns->count() + i, QIcon( ":/icons/ok.png" ) );
				segBtns->setSegmentText( segBtns->count() + i, tr( "&Ignore" ) );
				break;

			case NBMessageDialog::Cancel :
				segBtns->setSegmentIcon( segBtns->count() + i, QIcon::fromTheme( "dialog-cancel" ) );
				segBtns->setSegmentText( segBtns->count() + i, tr( "&Cancel" ) );
				break;

			default:
				break;
		}
	}
};

void NBMessageDialog::keyPressEvent( QKeyEvent *keyEvent ) {

	if ( keyEvent->key() == Qt::Key_Escape ) {
		retValue = NBMessageDialog::Cancel;
		close();
	}

	NBDialog::keyPressEvent( keyEvent );
};

void NBMessageDialog::close() {

	NBDialog::close();
	emit closed();
};

void NBMessageDialog::toggleInfoWidget() {

	if ( infoWidget->isVisible() ) {
		detailsBtn->setSegmentText( 0, "&More..." );
		infoWidget->hide();

		restoreGeometry( origGeometry );
	}

	else {
		origGeometry = saveGeometry();

		detailsBtn->setSegmentText( 0, "&Less..." );
		infoWidget->show();

		adjustSize();
	}
};

void NBMessageDialog::buttonClickHandler( int segment ) {

	QString text = segBtns->segmentText( segment );

	if ( text == QString( "&Ok" ) ) {
		retValue = NBMessageDialog::Ok;
		close();
	}

	else if ( text == QString( "&Yes" ) ) {
		retValue = NBMessageDialog::Yes;
		close();
	}

	else if ( text == QString( "Yes to &All" ) ) {
		retValue = NBMessageDialog::YesToAll;
		close();
	}

	else if ( text == QString( "&No" ) ) {
		retValue = NBMessageDialog::No;
		close();
	}

	else if ( text == QString( "N&o to All" ) ) {
		retValue = NBMessageDialog::NoToAll;
		close();
	}

	else if ( text == QString( "A&bort" ) ) {
		retValue = NBMessageDialog::Abort;
		close();
	}

	else if ( text == QString( "&Retry" ) ) {
		retValue = NBMessageDialog::Retry;
		close();
	}

	else if ( text == QString( "&Ignore" ) ) {
		retValue = NBMessageDialog::Ignore;
		close();
	}

	else if ( text == QString( "&Cancel" ) ) {
		retValue = NBMessageDialog::Cancel;
		close();
	}
};

int NBMessageDialog::information( QWidget *parent, QString title, QString text, QList<StandardButton> btns, QWidget *infoWidget ) {

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/info.png", title, text, btns, infoWidget, parent );
	dlg->exec();

	return dlg->retValue;
};

int NBMessageDialog::question( QWidget *parent, QString title, QString text, QList<StandardButton> btns, QWidget *infoWidget ) {

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/question.png", title, text, btns, infoWidget, parent );
	dlg->exec();

	return dlg->retValue;
};

int NBMessageDialog::warning( QWidget *parent, QString title, QString text, QList<StandardButton> btns, QWidget *infoWidget ) {

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/warning.png", title, text, btns, infoWidget, parent );
	dlg->exec();

	return dlg->retValue;
};

int NBMessageDialog::error( QWidget *parent, QString title, QString text, QList<StandardButton> btns, QWidget *infoWidget ) {

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/error.png", title, text, btns, infoWidget, parent );
	dlg->exec();

	return dlg->retValue;
};

int NBMessageDialog::critical( QWidget *parent, QString title, QString text, QList<StandardButton> btns, QWidget *infoWidget ) {

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/stop.png", title, text, btns, infoWidget, parent );
	dlg->exec();

	return dlg->retValue;
};
