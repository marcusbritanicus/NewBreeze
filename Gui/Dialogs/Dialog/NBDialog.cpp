/*
	*
	* NBDialog.cpp - NewBreeze Dialog Class
	*
*/

#include <NBDialog.hpp>
#include <NBStyleManager.hpp>

NBDialog::NBDialog( QWidget *parent ) : QDialog( parent ) {

	setupGUI();
	setupDialogProperties();
};

void NBDialog::setupGUI() {

	BodyFrame = new QFrame();
	BodyFrame->setContentsMargins( 0, 0, 0, 0 );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->setContentsMargins( 5, 5, 5, 5 );

	QAction *closeAct = new QAction( "&Close", this );
	closeAct->setShortcut( QKeySequence( Qt::Key_Escape ) );
	connect( closeAct, SIGNAL( triggered() ), this, SLOT( reject() ) );
	addAction( closeAct );

	baseLyt->addWidget( BodyFrame );

	QWidget::setLayout( baseLyt );
};

void NBDialog::setupDialogProperties() {

	/* Dialogs without close button */
	setWindowFlags( Qt::CustomizeWindowHint | Qt::Dialog | Qt::WindowSystemMenuHint | Qt::WindowTitleHint );

	/* NewBreeze speciality */
	setDialogIcon( QIcon( ":/icons/newbreeze2.png" ) );

	setWindowModality( Qt::ApplicationModal);

	/* Size and Size policy */
	setMinimumSize( 500, 75 );
	setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
};

void NBDialog::setLayout( QLayout *lyt ) {

	BodyFrame->setLayout( lyt );
};

void NBDialog::setDialogTitle( QString title ) {

	setWindowTitle( title );
};

void NBDialog::setDialogIcon( QIcon icon ) {

	setWindowIcon( icon );
};
