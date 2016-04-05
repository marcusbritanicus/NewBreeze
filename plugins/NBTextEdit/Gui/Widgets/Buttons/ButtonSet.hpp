/*
	*
	* ButtonSet.hpp - The Segmented Buttons Class Header
	*
*/

#pragma once

#include <Global.hpp>
#include <GuiWidgets.hpp>

class ButtonSet : public QWidget {
	Q_OBJECT

	public:
		ButtonSet();
		ButtonSet( QList<QIcon> );
		ButtonSet( QStringList );
		ButtonSet( QStringList, QList<QIcon> );

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

		int myButtonHeight;
		int myButtonWidth;

	private slots:
		void handleSegmentClick();

	Q_SIGNALS:
		void segmentClicked( int );
};

class ButtonSet2 : public QWidget {
	Q_OBJECT

	public:
		ButtonSet2();
		ButtonSet2( QList<QIcon> );

		QList<QToolButton*> buttons();

		QToolButton* segment( int );

		void setSegmentIcon( int, QIcon );

		void setSegmentWidth( int );
		void setSegmentHeight( int );

		void setSegmentShortcut( int, QKeySequence );

		void setSegmentDisabled( int );
		void setSegmentEnabled( int );

		void insertSegment( QToolButton *button, int logicalPos );
		void addSegment( QToolButton *button );

	private:
		QHBoxLayout *btnsLyt;
		QList<QToolButton*> segments;

		int myButtonHeight;
		int myButtonWidth;

	private slots:
		void handleSegmentClick();

	Q_SIGNALS:
		void segmentClicked( int );
};
