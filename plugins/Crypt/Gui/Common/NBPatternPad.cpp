/*
	*
	* PasswordKeypad.cpp - PasswordKeypad class
	*
*/

#include <NBPatternPad.hpp>

inline static quint64 qHash( const QPoint key ) {

	return qHash( static_cast<qint64>( key.x() ) << 32 | key.y() );
};

PasswordKeypad::PasswordKeypad( QWidget *parent ) : QWidget( parent ) {

	mInvalid = false;
	pressed = false;
	curPos = QPoint();

	setWindowTitle( "NewBreeze - Input the Password" );

	setFixedSize( 300, 300 );

	QRect sSize = QDesktopWidget().screenGeometry();
	int hpos = ( sSize.width() - 300 ) / 2;
	int vpos = ( sSize.height() - 300 ) / 2;

	setGeometry( hpos, vpos, 300, 300 );

	centers.clear();
	for( int y = 30; y <= 270; y += 60 )
		for( int x = 30; x <= 270; x += 60 )
			centers << QPoint( x, y );

	Q_FOREACH( QPoint center, centers )
		circles << Circle( center, 25 );

	for( int i = 0; i <= 25; i++ )
		pointsCharMap[ centers.at( i ) ] = QChar( i + 65 );
};

QString PasswordKeypad::password() {

	/* If ther version is greater than 5.1 */
	#if QT_VERSION >= 0x050100
		return QCryptographicHash::hash( m_password.toUtf8(), QCryptographicHash5::Sha3_512 ).toHex();
	#else
		return QCryptographicHash5::hash( m_password.toUtf8(), QCryptographicHash5::Sha3_512 ).toHex();
	#endif
};

void PasswordKeypad::clear() {

	mInvalid = false;
	points.clear();
	m_password.clear();

	repaint();
};

void PasswordKeypad::setInvalid() {

	mInvalid = true;
	repaint();
};

void PasswordKeypad::mousePressEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() != Qt::LeftButton )  {
		mEvent->ignore();
		return;
	}

	pressed = true;
	mInvalid = false;

	points.clear();
	m_password.clear();

	grabMouse();

	Q_FOREACH( Circle circle, circles ) {
		if ( circle.contains( mEvent->pos() ) ) {
			points << circle.center();
			m_password += pointsCharMap.value( circle.center() );
			break;
		}

		else  {
			continue;
		}
	}

	repaint();

	mEvent->accept();
};

void PasswordKeypad::mouseMoveEvent( QMouseEvent *mEvent )  {

	if ( pressed )  {
		curPos = mEvent->pos();
		Q_FOREACH( Circle circle, circles ) {
			if ( circle.contains( curPos ) ) {
				/* Check if this point is the previous point */
				if ( points.last() != circle.center() ) {
					points << circle.center();
					m_password += pointsCharMap.value( circle.center() );
					break;
				}

				else  {
					continue;
				}
			}

			else  {
				continue;
			}
		}
	}

	else {
		QWidget::mouseMoveEvent( mEvent );
	}

	repaint();

	mEvent->accept();
};

void PasswordKeypad::mouseReleaseEvent( QMouseEvent *mEvent )  {

	pressed = false;
	curPos = QPoint();

	releaseMouse();
	repaint();

	/* We close it as soon as we input the password */
	inputComplete();

	mEvent->accept();
};

void PasswordKeypad::paintEvent( QPaintEvent *pEvent )  {

	QPainter *painter = new QPainter( this );

	painter->setRenderHint( QPainter::Antialiasing );
	painter->setPen( QColor( 0x00, 0x66, 0xFF ) );

	/* Draw dots */
	Q_FOREACH( QPoint center, centers )  {
		painter->drawEllipse( center, 1, 1 );
		painter->drawEllipse( center, 2, 2 );
		painter->drawEllipse( center, 3, 3 );
	}

	if ( points.count() > 1 ) {
		// Draw selected points
		if ( mInvalid )
			painter->setPen( QPen( QColor( 177,  79, 79 ), 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

		else
			painter->setPen( QPen( QColor( 177,  79, 154 ), 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

		// Paint only the last three points
		painter->drawEllipse( points.at( points.count() - 1 ), 20, 20 );
		painter->drawEllipse( points.at( points.count() - 2 ), 20, 20 );
		if ( points.count() >= 3 )
			painter->drawEllipse( points.at( points.count() - 3 ), 20, 20 );

		// Draw lines
		if ( mInvalid )
			painter->setPen( QPen( QColor( 177,  79, 79 ), 3.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

		else
			painter->setPen( QPen( QColor( 177,  79, 154 ), 3.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

		painter->drawLine( points.at( points.count() - 2 ), points.at( points.count() - 1 ) );
		if ( points.count() >= 3 )
			painter->drawLine( points.at( points.count() - 3 ), points.at( points.count() - 2 ) );
	}

	else {
		if ( mInvalid )
			painter->setPen( QPen( QColor( 177,  79, 79 ), 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

		else
			painter->setPen( QPen( QColor( 177,  79, 154 ), 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

		if( pressed and points.count() )
			painter->drawEllipse( points.last(), 20, 20 );
	}

	if ( not curPos.isNull() )
		painter->drawLine( points.last(), curPos );

	painter->end();

	pEvent->accept();
};
