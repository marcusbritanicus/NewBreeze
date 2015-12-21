/*
	*
	* NBButtons.hpp - The Segmented Buttons Class Header
	*
*/

#pragma once
#ifndef NBBUTTONS_HPP
#define NBBUTTONS_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBButton.hpp>
#include <NBGuiWidgets.hpp>

class NBButtons : public QWidget {
	Q_OBJECT

	public:
		NBButtons( QWidget* );
		NBButtons( QList<QIcon>, QWidget* );
		NBButtons( QStringList, QWidget* );
		NBButtons( QStringList, QList<QIcon>, QWidget* );

		QList<NBButton*> buttons();

		NBButton* segment( int );

		void setSegmentIcon( int, QIcon );
		void setSegmentText( int, QString );

		void setSegmentWidth( int );

		void setSegmentShortcut( int, QKeySequence );

		void setSegmentDisabled( int );
		void setSegmentEnabled( int );

		void insertSegment( NBButton *button, int logicalPos );
		void addSegment( NBButton *button );

	private:
		QHBoxLayout *btnsLyt;
		QList<NBButton*> segments;

		int myButtonHeight;

	private slots:
		void handleSegmentClick();

	Q_SIGNALS:
		void segmentClicked( int );
};

#endif
