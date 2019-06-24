/*
	*
	* NBGuiWidgets.cpp - The GUI classes for NewBreeze
	*
*/

#include "NBGuiWidgets.hpp"

NBLineEdit::NBLineEdit( QWidget *parent ) : QLineEdit( parent ) {

	setMinimumHeight( 32 );
};

void NBLineEdit::paintEvent( QPaintEvent *pEvent ) {

	QLineEdit::paintEvent( pEvent );
	pEvent->accept();
};

QWidget* Separator::vertical( QWidget *parent ) {

	return new Separator( Separator::Vertical, parent );
};

QWidget* Separator::horizontal( QWidget *parent ) {

	return new Separator( Separator::Horizontal, parent );

	QWidget *hSep = new QWidget( parent );
	hSep->setContentsMargins( QMargins() );
	hSep->setFixedHeight( 1 );
	hSep->setStyleSheet( "background-color: gray;" );

	return hSep;
};

Separator::Separator( Separator::Mode mode, QWidget *parent ) : QWidget( parent ) {

	mMode = mode;

	switch( mode ) {
		case Separator::Horizontal: {
			setContentsMargins( QMargins() );
			setFixedHeight( 1 );

			hGrad = QLinearGradient( QPoint( 0, 0 ), QPoint( 2000, 0 ) );
			hGrad.setColorAt( 0.0, Qt::transparent );
			hGrad.setColorAt( 0.4, Qt::gray );
			hGrad.setColorAt( 0.6, Qt::gray );
			hGrad.setColorAt( 1.0, Qt::transparent );

			break;
		};

		case Separator::Vertical: {
			setContentsMargins( QMargins() );
			setFixedWidth( 1 );

			vGrad = QLinearGradient( QPoint( 0, 0 ), QPoint( 0, 2000 ) );
			vGrad.setColorAt( 0.0, Qt::transparent );
			vGrad.setColorAt( 0.4, Qt::gray );
			vGrad.setColorAt( 0.6, Qt::gray );
			vGrad.setColorAt( 1.0, Qt::transparent );

			break;
		}
	}
};

void Separator::resizeEvent( QResizeEvent *rEvent ) {

	QWidget::resizeEvent( rEvent );
	rEvent->accept();

	switch( mMode ) {
		case Separator::Horizontal : {
			hGrad.setFinalStop( 0, rEvent->size().width() );
			break;
		}

		case Separator::Vertical : {
			vGrad.setFinalStop( 0, rEvent->size().height() );
			break;
		}
	}

	repaint();
};

void Separator::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );
	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::HighQualityAntialiasing );

	switch( mMode ) {
		case Separator::Horizontal : {
			hGrad.setFinalStop( 0, width() );

			// painter->setPen( QPen( QBrush( hGrad ), 1.0 ) );
			painter->setPen( QPen( Qt::gray, 1.0 ) );
			painter->drawLine( 0, 0, width(), 0 );
			break;
		}

		case Separator::Vertical : {
			vGrad.setFinalStop( 0, height() );

			painter->setPen( QPen( QBrush( vGrad ), 1.0 ) );
			painter->drawLine( rect().topLeft(), rect().bottomLeft() );
			break;
		}
	}

	painter->end();

	pEvent->accept();
};
