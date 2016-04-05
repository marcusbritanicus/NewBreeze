/*
    *
    * GuiWidgets.cpp - Gui Widgets for Wordy
    *
*/

#include <GuiWidgets.hpp>

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

QWidget* Spacer::vertical( int space, QWidget *parent ) {

	QWidget *spacer = new QWidget( parent );
	if ( space > 0 )
		spacer->setFixedHeight( space );

	return spacer;
};

QWidget* Spacer::horizontal( int space, QWidget *parent ) {

	QWidget *spacer = new QWidget( parent );
	if ( space > 0 )
		spacer->setFixedWidth( space );

	return spacer;
};

LedLight::LedLight( quint64 radius, QWidget *parent ) : QWidget( parent ) {

	mRadius = radius;
	setMinimumSize( radius * 2, radius * 2 );
};

bool LedLight::isSafe() {

	return safety;
};

void LedLight::setSafe( bool sfty ) {

	safety = sfty;

	repaint();
};

void LedLight::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );

	painter.setRenderHints( QPainter::Antialiasing );

	painter.setPen( QPen( Qt::lightGray, 1.0 ) );
	QColor color;
	if ( safety )
		color = Qt::darkGreen;

	else
		color = Qt::red;

	QRadialGradient rg( QPoint( mRadius, mRadius ), mRadius, QPoint( width() / 2, mRadius / 2 ) );
	rg.setColorAt( 0, Qt::white );
	rg.setColorAt( 0.8, color.lighter( 120 ) );
	rg.setColorAt( 1, color );

	painter.setBrush( rg );

	painter.drawEllipse( QRectF( ( width() - 2 * mRadius ) / 2, ( height() - 2 * mRadius ) / 2, 2 * mRadius, 2 * mRadius ) );
	painter.end();

	pEvent->accept();
};
