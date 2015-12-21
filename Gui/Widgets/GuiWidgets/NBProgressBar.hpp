/*
	*
	* NBProgressBar.hpp - NBProgressBar.cpp header
	*
*/

#pragma once

#include <Global.hpp>

class NBProgressBar: public QWidget {
	Q_OBJECT

	public:
		NBProgressBar( QWidget* );

		void setValue( qreal );
		void setProgressText( QString );

	private:
		qreal mMin, mMax;
		qreal mFraction;

		QString progressText;

		QColor uncoveredBorder;
		QColor coveredBorder;

		QLinearGradient uncoveredGradient;

	protected:
		void paintEvent( QPaintEvent* );
};
