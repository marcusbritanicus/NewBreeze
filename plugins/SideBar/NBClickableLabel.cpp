/*
	*
	* NBClickableLabel.cpp - Clickable Label for SideBar Plugin
	*
	* The original file was a part of Quartica, released under GPL v3
	* Copyright ( c ) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
	*
	* Modified: Marcus Britanicus
	*
*/

#include <QMouseEvent>

#include <NBClickableLabel.hpp>

NBClickableLabel::NBClickableLabel( QWidget *parent, Qt::WindowFlags f ) : QLabel( parent ) {

	setWindowFlags( f );
}

NBClickableLabel::NBClickableLabel( const QString& text, QWidget *parent, Qt::WindowFlags f ) : QLabel( parent, f ) {

	setText( text );
}

void NBClickableLabel::mousePressEvent( QMouseEvent *mEvent ) {

	QLabel::mousePressEvent( mEvent );

	emit pressed();
	mEvent->accept();
};

void NBClickableLabel::mouseReleaseEvent ( QMouseEvent *mEvent ) {

	QLabel::mouseReleaseEvent( mEvent );

	emit released();

	emit clicked();
	emit clicked( this );

	mEvent->setAccepted( true );
}
