/*
	*
	* NBProgressBar.hpp - NBProgressBar.cpp header
	*
*/

#pragma once

#include "Global.hpp"

class NBProgressBar: public QWidget {
	Q_OBJECT

	public:
		NBProgressBar( QWidget* );

		/* Set a value for the progress in the range [0, 1] */
		void setValue( qreal );

		/* Set the oscillating progressbar */
		void setSway( bool );

		void setProgressText( QString );

	private:
		qreal mMin, mMax;
		qreal mFraction;

		bool mSway;
		qreal xpos;
		bool fwd;

		QString progressText;

		QColor uncoveredBorder;
		QColor coveredBorder;

		QLinearGradient uncoveredGradient;

		QBasicTimer swayTimer;

	protected:
		void paintEvent( QPaintEvent* );
		void timerEvent( QTimerEvent* );
};
