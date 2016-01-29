/*
   A whole bunch of stuff here.
*/

#pragma once

#include <Global.hpp>

class NBSegmentControlPrivate;

class NBSegmentControl : public QWidget {

    Q_OBJECT

    Q_ENUMS(SelectionBehavior)
    Q_PROPERTY(SelectionBehavior selectionBehavior READ selectionBehavior WRITE setSelectionBehavior)
    Q_PROPERTY(int selectedSegment READ selectedSegment NOTIFY segmentSelected)
    Q_PROPERTY(int count READ count WRITE setCount)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
    // Q_PROPERTY(QKeySequence shortcut READ shortcut WRITE setShortcut)

	public:
		enum SelectionBehavior { SelectOne, SelectAll, SelectNone };

		NBSegmentControl(QWidget *parent = 0);
		~NBSegmentControl();

		int count() const;
		void setCount(int newConut);

		bool isSegmentSelected(int index) const;
		int selectedSegment() const;
		void setSegmentSelected(int index, bool selected);

		void setSegmentEnabled(int index, bool enable);
		bool segmentEnabled(int index) const;

		void setSelectionBehavior(SelectionBehavior behavior);
		SelectionBehavior selectionBehavior() const;

		void setSegmentText(int index, const QString &text);
		QString segmentText(int index) const;

		void setSegmentIcon(int index, const QIcon &icon);
		QIcon segmentIcon(int index) const;

		void setIconSize(const QSize &size);
		QSize iconSize() const;

		void setSegmentMenu(int segment, QMenu *menu);
		QMenu *segmentMenu(int segment) const;

		void setSegmentToolTip(int segment, const QString &tipText);
		QString segmentToolTip(int segment) const;

		void setSegmentWhatsThis(int segment, const QString &whatsThisText);
		QString segmentWhatsThis(int segment) const;

		virtual QSize segmentSizeHint(int segment) const;
		QSize sizeHint() const;

		QRect segmentRect(int index) const;
		int segmentAt(const QPoint &pos) const;

		void setShortcut( int index, const QKeySequence &key);
		QKeySequence shortcut( int index ) const;

	protected:
		void initStyleOption(int segment, QStyleOption *option) const;

		void paintEvent(QPaintEvent *pe);

		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);

		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);

		bool event(QEvent *event);
		void timerEvent( QTimerEvent *tEvent);

	Q_SIGNALS:
		void segmentSelected(int selected);
		void pressed( int seg );
		void released( int seg );
		void clicked( int seg );

	private:
		NBSegmentControlPrivate *d;

	private slots:
		void animateClick(int index, int msec = 100);
};
