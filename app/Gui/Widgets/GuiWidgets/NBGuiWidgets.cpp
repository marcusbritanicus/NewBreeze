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

NBDriveInfo::NBDriveInfo( NBDeviceInfo dInfo, QWidget *parent ) : QWidget ( parent ) {

	quint64 usedSize = dInfo.usedSpace();
	quint64 freeSize = dInfo.freeSpace();
	quint64 totalSize = dInfo.driveSize();

	percent = 1.0 * usedSize / totalSize;

	QString freeStr = formatSize( totalSize - usedSize );

	name = dInfo.driveLabel();
	disk = dInfo.driveName();
	icon = ":/icons/" + dInfo.driveType() + ".png";
	info = QString( "%1 free of %2 (%3% used)" ).arg( freeStr ).arg( formatSize( totalSize ) ).arg( 100 * percent, 0, 'f', 2 );

	setFixedHeight( 64 );
};

void NBDriveInfo::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setRenderHints( QPainter::HighQualityAntialiasing | QPainter::Antialiasing | QPainter::TextAntialiasing );

	painter.save();
	painter.drawPixmap( QRect( 8, 8, 48, 48 ), QPixmap( icon ).scaled( QSize( 64, 64 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	painter.drawText( QRect( 74, 0, width(), height() ), Qt::AlignLeft | Qt::AlignVCenter, QString( "%1 (%2)\n%3" ).arg( name ).arg( disk ).arg( info ) );
	painter.restore();

	painter.save();
	painter.setPen( Qt::darkGray );
	painter.setBrush( Qt::gray );
	painter.drawEllipse( QPointF( width() - 32.0, 32.0 ), 32.0, 32.0 );
	painter.restore();

	painter.save();
	qreal radius = 32 * sqrt( percent );
	painter.setPen( Qt::NoPen );
	painter.setBrush( percent < .9 ? Qt::darkGreen : Qt::darkRed );
	painter.drawEllipse( QPointF( width() - 32.0, 32.0 ), radius, radius );
	painter.restore();

	painter.save();
	painter.setPen( Qt::white );
	painter.setFont( QFont( font().family(), font().pointSize(), QFont::Bold ) );
	painter.drawText( QRect( width() - 64, 0, 64, 64 ), Qt::AlignCenter, QString( "%1%" ).arg( percent * 100, 0, 'f', 2 ) );
	painter.restore();

	painter.end();
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
