/*
	*
	* NBFilterWidget.hpp - NBFilterWidget for NewBreeze
	*
*/

#include "NBFilterWidget.hpp"

NBFilterWidget::NBFilterWidget( QWidget *parent ) : QWidget( parent ) {

	setupUI();
};

void NBFilterWidget::setupUI() {

	// Modality
	setWindowFlags( Qt::WindowStaysOnTopHint );

	// Minimum Width
	setFixedHeight( 48 );
	setFixedWidth( 360 );

	// Widgets - Label and LineEdit

	findLbl = new QLabel();
	findLbl->setPixmap( QIcon( ":/icons/search.png" ).pixmap( 16 ) );

	findLE = new QLineEdit();
	findLE->setFocusPolicy( Qt::StrongFocus );
	connect( findLE, SIGNAL( textChanged( QString ) ), this, SIGNAL( search( QString ) ) );

	// Layouts
	QHBoxLayout *srchLyt = new QHBoxLayout();
	srchLyt->addWidget( findLbl );
	srchLyt->addWidget( findLE );

	QWidget *base = new QWidget( this );
	base->setObjectName( "guiBase" );
	base->setLayout( srchLyt );

	QVBoxLayout *widgetLyt = new QVBoxLayout();
	widgetLyt->addWidget( base );

	setLayout( widgetLyt );

	setStyleSheet( "QWidget#guiBase { border: 1px solid gray; background: rgba(255, 255, 255, 180); } QLineEdit{ background: transparent; }" );
};

void NBFilterWidget::clear() {

	findLE->clear();
};

void NBFilterWidget::setFocus() {

	findLE->setFocus();
};

void NBFilterWidget::show() {

	move( qobject_cast<QWidget*>( parent() )->width() - 360, 0 );

	QWidget::show();
	emit shown();

	setFocus();
};

void NBFilterWidget::hide() {

	QWidget::hide();
	emit hidden();
};

void NBFilterWidget::showHide() {

	if ( isVisible() )
		hide();

	else
		show();
};

void NBFilterWidget::keyPressEvent( QKeyEvent *kEvent ) {

	switch( kEvent->key() ) {
		case Qt::Key_Escape : {
			hide();
			break;
		}

		default: {
			QWidget::keyPressEvent( kEvent );
			break;
		}
	};

	kEvent->accept();
};
