/*
	*
	* NBButtons.hpp - The Segmented Buttons Class Header
	*
*/

#ifndef NBBUTTONS_HPP
#define NBBUTTONS_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>

class NBButtons : public QWidget {
	Q_OBJECT

	public:
		NBButtons();
		NBButtons( QList<QIcon> );
		NBButtons( QStringList );
		NBButtons( QStringList, QList<QIcon> );

		QList<QPushButton*> buttons();

		QPushButton* segment( int );

		void setSegmentIcon( int, QIcon );
		void setSegmentText( int, QString );

		void setSegmentWidth( int );
		void setSegmentHeight( int );

		void setSegmentShortcut( int, QKeySequence );

		void setSegmentDisabled( int );
		void setSegmentEnabled( int );

		void insertSegment( QPushButton *button, int logicalPos );
		void addSegment( QPushButton *button );

	private:
		QHBoxLayout *btnsLyt;
		QList<QPushButton*> segments;

		int myButtonHeight = 24;

	private slots:
		void handleSegmentClick();

	Q_SIGNALS:
		void segmentClicked( int );
};

#endif
