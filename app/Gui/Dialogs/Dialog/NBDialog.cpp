/*
	*
	* NBDialog.cpp - NewBreeze Dialog Class
	*
*/

#include "NBDialog.hpp"

NBDialog::NBDialog( QWidget *parent ) : QDialog( parent ) {

	setupGUI();
	setupDialogProperties();
};

void NBDialog::setupGUI() {

	QAction *closeAct = new QAction( "&Close", this );
	closeAct->setShortcut( QKeySequence( Qt::Key_Escape ) );
	connect( closeAct, SIGNAL( triggered() ), this, SLOT( reject() ) );
	addAction( closeAct );
};

void NBDialog::setupDialogProperties() {

	/* Dialogs without close button */
	setWindowFlags( Qt::Dialog | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );

	/* NewBreeze speciality */
	setDialogIcon( QIcon( ":/icons/newbreeze.png" ) );

	setWindowModality( Qt::ApplicationModal);

	/* Size and Size policy */
	setMinimumSize( 500, 75 );
	setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
};

void NBDialog::setLayout( QLayout *lyt ) {

	lyt->setContentsMargins( QMargins( 5, 5, 5, 5 ) );
	QDialog::setLayout( lyt );
};

void NBDialog::setDialogTitle( QString title ) {

	setWindowTitle( title );
};

void NBDialog::setDialogIcon( QIcon icon ) {

	setWindowIcon( icon );
};

void NBDialog::closeEvent( QCloseEvent *cEvent ) {

	QDialog::closeEvent( cEvent );

	emit closed();
};
