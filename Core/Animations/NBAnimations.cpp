/*
	*
	* NBAnimations.cpp - Some QVariantAnimations for NewBreeze
	*
*/

#include <NBAnimations.hpp>

NBWidthAnimation::NBWidthAnimation( QWidget *target ) : QVariantAnimation(), targetWidget( target ) {

	connect( this, SIGNAL( valueChanged( const QVariant & ) ), this, SLOT( setTargetWidth( const QVariant & ) ) );
};

void NBWidthAnimation::updateCurrentValue( const QVariant &value ) {

	emit valueChanged( value );
};

void NBWidthAnimation::setTargetWidth( const QVariant &value ) {

	targetWidget->setFixedWidth( value.toInt() );

	if ( value.toInt() == 0 )
		targetWidget->hide();

	else if ( value.toInt() == endValue().toInt() )
		targetWidget->setFocus();
};

NBHeightAnimation::NBHeightAnimation( QWidget *target ) : QVariantAnimation(), targetWidget( target ) {

	connect( this, SIGNAL( valueChanged( const QVariant & ) ), this, SLOT( setTargetHeight( const QVariant & ) ) );
};

void NBHeightAnimation::updateCurrentValue( const QVariant &value ) {

	emit valueChanged( value );
};

void NBHeightAnimation::setTargetHeight( const QVariant &value ) {

	targetWidget->setFixedHeight( value.toInt() );
	if ( value.toInt() == 0 )
		targetWidget->hide();

	else if ( value.toInt() == endValue().toInt() )
		targetWidget->setFocus();
};
