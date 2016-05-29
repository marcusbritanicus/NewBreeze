/*
	*
	* NBLabels.cpp - Different types of customized labels for NewBreeze
	*
*/

#include <NBLabels.hpp>

NBClickLabel::NBClickLabel( QPixmap icon, QWidget *parent ) : QLabel( parent ) {

	clickEnabled = true;

	setPixmap( icon );
	setAlignment( Qt::AlignCenter );
};

NBClickLabel::NBClickLabel( QString text, QWidget *parent ) : QLabel( parent ) {

	clickEnabled = true;

	setText( text );
	setAlignment( Qt::AlignCenter );
};

void NBClickLabel::mousePressEvent( QMouseEvent *mEvent ) {

	if ( clickEnabled )
		emit pressed();

	mEvent->accept();
};

void NBClickLabel::mouseReleaseEvent( QMouseEvent *mEvent ) {

	if ( clickEnabled and rect().contains( mEvent->pos() ) ) {
		emit clicked();
		emit released();
	}

	mEvent->accept();
};

void NBClickLabel::setClickable( bool canClick ) {

	clickEnabled = canClick;
};

NBInfoLabel::NBInfoLabel() : QLabel() {

	setAlignment( Qt::AlignCenter );
	setFont( QFont( "Envy Code R", 9 ) );
};

void NBInfoLabel::setText( QString name, QString size, QString type, QString perm ) {

	QString text = QString(
		"<table style='width: 700px; height: 56px;'>"			\
		"	<tr>"												\
		"		<td colspan=3 align = 'left'>%1</td>"			\
		"	</tr>"												\
		"	<tr>"												\
		"		<td width=300 align='left'>%2</td>"				\
		"		<td width=300 align='left'>%3</td>"				\
		"		<td width=100 align='center'>%4</td>"			\
		"	</tr>"												\
		"</table>"
	);

	QLabel::setText( text.arg( name ).arg( size ).arg( type ).arg( perm ) );
};
