/*
	*
	* NBAnimations.hpp - NBAnimations.cpp header
	*
*/

#pragma once

#include <Global.hpp>

class NBWidthAnimation : public QVariantAnimation {
	Q_OBJECT

	public:
		NBWidthAnimation( QWidget *target );

	private:
		QWidget *targetWidget;

	protected:
		void updateCurrentValue( const QVariant& );

	private slots:
		void setTargetWidth( const QVariant & );
};

class NBHeightAnimation : public QVariantAnimation {
	Q_OBJECT

	public:
		NBHeightAnimation( QWidget *target );

	private:
		QWidget *targetWidget;

	protected:
		void updateCurrentValue( const QVariant& );

	private slots:
		void setTargetHeight( const QVariant & );
};
