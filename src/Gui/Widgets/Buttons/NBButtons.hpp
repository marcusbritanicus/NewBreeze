/*
	*
	* NBSegmentButton.hpp - The Segmented Buttons Class Header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBButton.hpp>
#include <NBGuiWidgets.hpp>

class NBSegmentButton : public QWidget {
	Q_OBJECT

	enum SelectionBehavior { SelectOne, SelectAll, SelectNone };

	public:
		NBSegmentButton( QWidget* );
		NBSegmentButton( QList<QIcon>, QWidget* );
		NBSegmentButton( QStringList, QWidget* );
		NBSegmentButton( QStringList, QList<QIcon>, QWidget* );

		int count();
		void setCount( int );

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

		void setSelectionBehavior(SelectionBehavior behavior);
		SelectionBehavior selectionBehavior() const;

		QSize size();
		QSize sizeHint();

	private:
		void resetStyleSheets();
		void autoResize();

		QHBoxLayout *btnsLyt;
		QList<NBButton*> segments;

		int myButtonHeight;
		int myWidgetWidth;
		int mSelectionBehavior;

	private slots:
		void handleSegmentClick();

	Q_SIGNALS:
		void clicked( int );
};
