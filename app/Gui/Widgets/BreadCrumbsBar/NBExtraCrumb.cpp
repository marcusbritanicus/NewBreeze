/*
	*
	* NBExtraCrumb.cpp - ExtraCrumb class fro NewBreeze
	*
	* This button is used to show the full path if the path runs out of space in the NBCrumbsBar
	*
*/

#include "NBExtraCrumb.hpp"

NBExtraCrumb::NBExtraCrumb( QWidget *parent ) : QWidget( parent ) {

	/* Track mouse movements */
	setMouseTracking( true );

	/* Button Press */
	m_Pressed = false;

	/* Size Policy */
	setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
};

QSize NBExtraCrumb::sizeHint() {

	return QSize( 24, 24 );
};

QSize NBExtraCrumb::minimumSizeHint() {

	return QSize( 24, 24 );
};

void NBExtraCrumb::setRequired( bool truth ) {

	m_Required = truth;
};

void NBExtraCrumb::mousePressEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() != Qt::LeftButton )
		return;

	if ( not rect().contains( mEvent->pos() ) )
		return;

	m_Pressed = true;
	repaint();

	mEvent->accept();
};

void NBExtraCrumb::mouseReleaseEvent( QMouseEvent *mEvent ) {

	m_Pressed = false;
	repaint();

	emit clicked();

	mEvent->accept();
};

void NBExtraCrumb::enterEvent( QEvent *eEvent ) {

	repaint();
	eEvent->accept();
};

void NBExtraCrumb::leaveEvent( QEvent *lEvent ) {

	repaint();
	lEvent->accept();
};

void NBExtraCrumb::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );

	// Background
	if ( m_Pressed ) {
		painter->save();
		painter->setPen( Qt::NoPen );
		painter->setRenderHint( QPainter::Antialiasing );
		painter->setBrush( palette().color( QPalette::Active, QPalette::Highlight ) );
		painter->drawRoundedRect( rect(), 2.0, 2.0 );
		painter->restore();
	}

	else if ( rect().contains( mapFromGlobal( QCursor::pos() ) ) ) {
		painter->save();
		painter->setPen( Qt::NoPen );
		painter->setRenderHint( QPainter::Antialiasing );
		painter->setBrush( palette().color( QPalette::Active, QPalette::Highlight ).lighter( 160 ) );
		painter->drawRoundedRect( rect(), 2.0, 2.0 );
		painter->restore();
	}

	else {
		/* Make sure the previous paintings are undone */
		painter->fillRect( rect(), Qt::transparent );
	}

	// Colors
	if ( m_Pressed ) {
		painter->setPen( palette().color( QPalette::Active, QPalette::HighlightedText ) );
		painter->setBrush( palette().color( QPalette::Active, QPalette::HighlightedText ) );
	}

	else if ( not m_Required ) {
		painter->setPen( palette().color( QPalette::Active, QPalette::Window ).darker() );
		painter->setBrush( palette().color( QPalette::Active, QPalette::Window ).darker() );
	}

	else {
		painter->setPen( palette().color( QPalette::Active, QPalette::Highlight ) );
		painter->setBrush( palette().color( QPalette::Active, QPalette::Highlight ) );
	}

	painter->save();

	painter->drawLine( QPoint( 3, 3 ), QPoint( 7, 3 ) );
	painter->drawLine( QPoint( 3, 3 ), QPoint( 3, 7 ) );

	painter->drawLine( QPoint( 16, 3 ), QPoint( 20, 3 ) );
	painter->drawLine( QPoint( 20, 3 ), QPoint( 20, 7 ) );

	painter->drawLine( QPoint( 3, 20 ), QPoint( 7, 20 ) );
	painter->drawLine( QPoint( 3, 16 ), QPoint( 3, 20 ) );

	painter->drawLine( QPoint( 20, 16 ), QPoint( 20, 20 ) );
	painter->drawLine( QPoint( 16, 20 ), QPoint( 20, 20 ) );

	painter->drawRect( QRect( QPoint( 7, 7 ), QPoint( 15, 15 ) ) );
	painter->restore();

	painter->end();
};
