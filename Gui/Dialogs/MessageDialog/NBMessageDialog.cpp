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

		detailsBtn = new NBButton( QIcon( ":/icons/info.png" ), "&More", this );
		detailsBtn->resize( 100, 0 );
		connect( detailsBtn, SIGNAL( clicked() ), this, SLOT( toggleInfoWidget() ) );
		btnLyt->addWidget( detailsBtn );
	}

	btnLyt->addStretch( 0 );
	segBtns = new NBButtons( this );

	foreach( StandardButton btn, btns ) {
		NBButton *button;
		switch( btn ) {
			case NBMessageDialog::Ok :
				button = new NBButton( QIcon( ":/icons/ok.png" ), "&Ok", this );
				button->setObjectName( tr( "okBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::Yes :
				button = new NBButton( QIcon( ":/icons/ok.png" ), "&Yes", this );
				button->setObjectName( tr( "okBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::YesToAll :
				button = new NBButton( QIcon( ":/icons/ok.png" ), "Yes to &All", this );
				button->setObjectName( tr( "okBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::No :
				button = new NBButton( QIcon( ":/icons/delete.png" ), "&No", this );
				button->setObjectName( tr( "cancelBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::NoToAll :
				button = new NBButton( QIcon( ":/icons/delete.png" ), "N&o to All", this );
				button->setObjectName( tr( "cancelBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::Abort :
				button = new NBButton( QIcon( ":/icons/abort.png" ), "A&bort", this );
				button->setObjectName( tr( "abortBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::Retry :
				button = new NBButton( QIcon( ":/icons/refresh.png" ), "&Retry", this );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::Ignore :
				button = new NBButton( QIcon( ":/icons/ok.png" ), "&Ignore", this );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::Cancel :
				button = new NBButton( QIcon::fromTheme( "dialog-cancel" ), "&Cancel", this );
				button->setObjectName( tr( "cancelBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
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
	}

	foreach( StandardButton btn, btns ) {
		NBButton *button;
		switch( btn ) {
			case NBMessageDialog::Ok :
				button = new NBButton( QIcon( ":/icons/ok.png" ), "&Ok", this );
				button->setObjectName( tr( "okBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::Yes :
				button = new NBButton( QIcon( ":/icons/ok.png" ), "&Yes", this );
				button->setObjectName( tr( "okBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::YesToAll :
				button = new NBButton( QIcon( ":/icons/ok.png" ), "Yes to &All", this );
				button->setObjectName( tr( "okBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::No :
				button = new NBButton( QIcon( ":/icons/delete.png" ), "&No", this );
				button->setObjectName( tr( "cancelBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::NoToAll :
				button = new NBButton( QIcon( ":/icons/delete.png" ), "N&o to All", this );
				button->setObjectName( tr( "cancelBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::Abort :
				button = new NBButton( QIcon( ":/icons/abort.png" ), "A&bort", this );
				button->setObjectName( tr( "abortBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::Retry :
				button = new NBButton( QIcon( ":/icons/refresh.png" ), "&Retry", this );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::Ignore :
				button = new NBButton( QIcon( ":/icons/ok.png" ), "&Ignore", this );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			case NBMessageDialog::Cancel :
				button = new NBButton( QIcon::fromTheme( "dialog-cancel" ), "&Cancel", this );
				button->setObjectName( tr( "cancelBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				segBtns->addSegment( button );
				break;

			default:
				qDebug() << "No Button added!!";
				break;
		}
	}

	segBtns->setSegmentWidth( 128 );
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
		detailsBtn->setText( "&More..." );
		infoWidget->hide();

		restoreGeometry( origGeometry );
	}

	else {
		origGeometry = saveGeometry();

		detailsBtn->setText( "&Less..." );
		infoWidget->show();

		adjustSize();
	}
};

void NBMessageDialog::buttonClickHandler() {

	QString text = qobject_cast<NBButton*>( sender() )->text();

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
