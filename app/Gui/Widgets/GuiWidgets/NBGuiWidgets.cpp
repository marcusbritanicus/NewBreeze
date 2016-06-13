/*
	*
	* NBGuiWidgets.cpp - The GUI classes for NewBreeze
	*
*/

#include "NBGuiWidgets.hpp"

NBLineEdit::NBLineEdit( QWidget *parent ) : QLineEdit( parent ) {

	setStyleSheet( "background: transparent; border: none;" );
};

NBDriveLabel::NBDriveLabel( const QString path, QWidget *parent ) : QWidget( parent ) {

	painter = new QPainter();
	label = QString( path );

	setFixedHeight( 32 );
	QFontMetrics fm( font() );
	setMinimumWidth( 42 + fm.maxWidth() * path.count() );
};

NBDriveLabel::~NBDriveLabel() {

	if ( painter->isActive() )
		painter->end();

	delete painter;
};

void NBDriveLabel::paintEvent( QPaintEvent *pEvent ) {

	painter->begin( this );
	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	QPixmap pix = QIcon::fromTheme( "drive-harddisk" ).pixmap( 28, 28 );

	painter->setPen( palette().color( QPalette::WindowText ) );

	// 10 px padding along X
	int pixX = 7;

	// Centering along Y
	int pixY = ( height() - 28 ) / 2;

	// icon padding-left + icons size + text padding-left
	int textX = 7 + 28 + 7;
	// Text is painted using Qt Alignent Flags
	int textY = 0;
	// Total width - text start - text-padding-right - right icon size - icon padding-right
	int textW = width() - textX - 7 - 28 - 7;
	// Text height is full label height
	int textH = height();

	// painter->begin( this );
	painter->drawPixmap( pixX, pixY, 28, 28, pix );
	painter->drawText( textX, textY, textW, textH, Qt::AlignVCenter + Qt::TextWordWrap, label );
	painter->end();

	pEvent->accept();
};

NBDriveInfo::NBDriveInfo( qint64 used, qint64 total, QWidget *parent ) : QFrame( parent ) {

	setFrameStyle( QFrame::StyledPanel | QFrame::Plain );
	setMaximumHeight( 32 );

	painter = new QPainter();

	percent = 1.0 * used / total;
	QString freeStr = formatSize( total - used );
	QString totalStr = formatSize( total );

	QFont xFont( font() );
	xFont.setBold( true );

	QLabel *label = new QLabel( QString( "%1 free of %2 (%3% used)" ).arg( freeStr ).arg( totalStr ).arg( 100 * percent, 0, 'f', 2 ) );
	label->setFont( xFont );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->addWidget( label );
	lyt->setAlignment( Qt::AlignCenter );

	setLayout( lyt );
};

NBDriveInfo::~NBDriveInfo() {

	if ( painter->isActive() )
		painter->end();

	delete painter;
};

void NBDriveInfo::paintEvent( QPaintEvent *pEvent ) {

	painter->begin( this );
	painter->setRenderHints( QPainter::Antialiasing );

	painter->setPen( Qt::NoPen );

	// Draw the percentage bar with the correct color
	if ( percent > 0.9 )
		painter->setBrush( QBrush( Qt::darkRed ) );

	else
		painter->setBrush( QBrush( Qt::darkGreen ) );

	painter->drawRoundedRect( 1, 1, width() * percent, height() - 2, 5, 5 );

	// Now draw a border
	painter->setPen( QColor( Qt::gray ) );
	painter->setBrush( Qt::transparent );
	painter->drawRoundedRect( 0, 0, width(), height(), 6, 6 );

	painter->end();

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
