/*
	*
	* NBProgressBar.hpp - ProgressBar Class Header
	*
*/

#pragma once
#ifndef NBPROGRESSBAR_HPP
#define NBPROGRESSBAR_HPP

#include <Global.hpp>

class NBProgressBar: public QWidget {
	Q_OBJECT

	public:
		NBProgressBar();

		void setProgressFraction( qreal );
		void setValue( qreal );

		qreal minimum();
		qreal maximum();

		void setMinimum( qreal );
		void setMaximum( qreal );
		void setRange( qreal, qreal );

	private:
		qreal mMin, mMax;
		qreal mFraction;

		QColor uncoveredBorder;
		QColor coveredBorder;

		QLinearGradient uncoveredGradient;
		QLinearGradient coveredGradient;

	protected:
		void paintEvent( QPaintEvent* );
};

#endif
