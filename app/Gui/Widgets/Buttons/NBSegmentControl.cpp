#include "NBSegmentControl.hpp"

class NBStyleOptionSegmentControlSegment : public QStyleOption {

	public:
		enum StyleOptionType { Type = 100000  };
		enum StyleOptionVersion { Version = 1 };

		enum SegmentPosition { Beginning, Middle, End, OnlyOneSegment };
		enum SelectedPosition { NotAdjacent, NextIsSelected, PreviousIsSelected };

		QString text;
		QIcon icon;
		QSize iconSize;
		SegmentPosition position;
		SelectedPosition selectedPosition;

		NBStyleOptionSegmentControlSegment()
		   : position(OnlyOneSegment), selectedPosition(NotAdjacent) { }
		NBStyleOptionSegmentControlSegment(const NBStyleOptionSegmentControlSegment &other)
			: QStyleOption(Version, Type) { *this = other; }

	protected:
		NBStyleOptionSegmentControlSegment(int version);
};


static void drawSegmentControlSegmentSegment(const QStyleOption *option, QPainter *painter, QWidget *widget) {

	// ### Change to qstyleoption_cast!
	if ( const NBStyleOptionSegmentControlSegment *segment = static_cast<const NBStyleOptionSegmentControlSegment *>( option ) ) {
		Q_UNUSED(widget);
		painter->save();

		bool selected = (segment->state & QStyle::State_Selected);

		QPixmap pm;

		QSize buttonSize = widget->rect().size();
		QString key = QString("qt_segment %0 %1 %2").arg(option->state).arg(buttonSize.width()).arg(buttonSize.height());

		if (!QPixmapCache::find(key, pm)) {
			pm = QPixmap(buttonSize);
			pm.fill(Qt::transparent);
			QPainter pmPainter(&pm);
			QStyleOptionButton btnOpt;
			btnOpt.QStyleOption::operator =(*option);
			btnOpt.state &= ~QStyle::State_HasFocus;
			btnOpt.rect = QRect(QPoint(0, 0), buttonSize);;
			btnOpt.state = option->state;

			if (selected)
				btnOpt.state |= QStyle::State_Sunken;
			else
				btnOpt.state |= QStyle::State_Raised;
			widget->style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &btnOpt, &pmPainter, widget);
			pmPainter.end();
			QPixmapCache::insert(key, pm);
		}
		int margin = widget->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, widget);
		switch (segment->position) {
			case NBStyleOptionSegmentControlSegment::Beginning:
				painter->setClipRect(option->rect);
				painter->drawPixmap(0, 0, pm);
				painter->setOpacity(0.6);
				painter->setPen(option->palette.dark().color());
				painter->drawLine(option->rect.topRight() + QPoint(-1, margin), option->rect.bottomRight() + QPoint(-1, -margin));
				break;
			case NBStyleOptionSegmentControlSegment::Middle:
				painter->setClipRect(option->rect);
				painter->drawPixmap(0, 0, pm);
				painter->setPen(option->palette.dark().color());
				painter->drawLine(option->rect.topRight() + QPoint(-1, margin), option->rect.bottomRight() + QPoint(-1, -margin));
				break;
			case QStyleOptionTab::End:
				painter->setClipRect(option->rect);
				painter->drawPixmap(0, 0, pm);
				break;
			case QStyleOptionTab::OnlyOneTab:
				painter->setClipRect(option->rect);
				painter->drawPixmap(0, 0, pm);
				break;
		}
		painter->restore();
	}
};

static QSize segmentSizeFromContents(const QStyleOption *option, const QSize &contentSize) {

	QSize ret = contentSize;
	if (const NBStyleOptionSegmentControlSegment *segment
			= static_cast<const NBStyleOptionSegmentControlSegment *>(option)) {
		ret.rwidth() += 20;
		ret.rheight() += 10;
		if (!segment->icon.isNull())
			ret.rwidth() += 5;
	}
	return ret;
};

static void drawSegmentControlSegmentLabel(const QStyleOption *option, QPainter *painter, QWidget *widget) {

	if (const NBStyleOptionSegmentControlSegment *segment = static_cast<const NBStyleOptionSegmentControlSegment *>(option)) {
		QStyleOptionButton button;
		button.QStyleOption::operator=(*option);
		button.text = segment->text;
		button.icon = segment->icon;
		button.iconSize = QSize( 16, 16 );
		widget->style()->drawControl(QStyle::CE_PushButtonLabel, &button, painter, widget);
	}

};

static void drawSegmentControlFocusRect(const QStyleOption *option, QPainter *painter, QWidget *widget) {

	QStyleOptionFocusRect focusOpt;
	focusOpt.QStyleOption::operator =(*option);
	focusOpt.rect.adjust(2, 2, -2, -2); //use subcontrolrect for this
	widget->style()->drawPrimitive(QStyle::PE_FrameFocusRect, &focusOpt, painter, widget);
};

static void drawSegmentControlSegment(const QStyleOption *option, QPainter *painter, QWidget *widget) {

	drawSegmentControlSegmentSegment(option, painter, widget);
	drawSegmentControlSegmentLabel(option, painter, widget);
	if (option->state & QStyle::State_HasFocus)
		drawSegmentControlFocusRect(option, painter, widget);
};


struct SegmentInfo {

	SegmentInfo() : menu(0), selected(false), enabled(true) {}
	~SegmentInfo() { delete menu; }
	QString text;
	QString toolTip;
	QString whatsThis;
	QIcon icon;
	QMenu *menu;
	bool selected;
	bool enabled;
	QRect rect;
	QKeySequence shortcut;
	int shortcutId;
};

class NBSegmentControlPrivate {

	public:
		NBSegmentControlPrivate(NBSegmentControl *myQ)
			: q(myQ), lastSelected(-1), layoutDirty(true), pressedIndex(-1), wasPressed(-1), focusIndex(-1) {};
		~NBSegmentControlPrivate() {};

		void layoutSegments();
		void postUpdate(int index = -1, bool geoToo = false);

		NBSegmentControl *q;
		NBSegmentControl::SelectionBehavior selectionBehavior;
		QSize iconSize;
		QVector<SegmentInfo> segments;
		int lastSelected;
		bool layoutDirty;
		int pressedIndex;
		int wasPressed;
		int focusIndex;
		inline bool validIndex(int index) { return index >= 0 && index < segments.count(); }

		QBasicTimer animateTimer;
};

void NBSegmentControlPrivate::layoutSegments() {

	if (!layoutDirty)
		return;
	const int segmentCount = segments.count();
	QRect rect;
	for (int i = 0; i < segmentCount; ++i) {
		QSize ssh = q->segmentSizeHint(i);
		rect.setSize(ssh);
		segments[i].rect = rect;
		rect.setLeft(rect.left() + ssh.width());
	}
	layoutDirty = false;
};

void NBSegmentControlPrivate::postUpdate(int /*index*/, bool geoToo) {

	if (geoToo) {
		layoutDirty = true;
		q->updateGeometry();
	}
	q->update();
};

NBSegmentControl::NBSegmentControl(QWidget *parent) : QWidget(parent), d(new NBSegmentControlPrivate(this)) {

	setFocusPolicy(Qt::TabFocus);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_WState_OwnSizePolicy, false);
};

NBSegmentControl::~NBSegmentControl() {

	delete d;
};

int NBSegmentControl::count() const {

	return d->segments.count();
};

void NBSegmentControl::setCount(int newCount) {

	d->segments.resize(newCount);

	// If current index is not valid, make it the first valid index
	if (!d->validIndex(d->focusIndex)) {
		for (int i = 0; i < newCount; ++i) {
			if (d->validIndex(i) && d->segments[i].enabled) {
				d->focusIndex = i;
				break;
			}
		}
	}
};

bool NBSegmentControl::isSegmentSelected(int index) const {

	if (!d->validIndex(index))
		return false;

	return d->segments.at(index).selected;
};

int NBSegmentControl::selectedSegment() const {

	return d->lastSelected;
};

void NBSegmentControl::setSegmentSelected(int index, bool selected) {

	if (!d->validIndex(index))
		return;

	if (d->segments[index].selected != selected) {
		d->segments[index].selected = selected;
		d->lastSelected = index;
		if (d->selectionBehavior == SelectOne) {
			const int segmentCount = d->segments.count();
			for (int i = 0; i < segmentCount; ++i) {
				SegmentInfo &info = d->segments[i];
				if (i != index && info.selected) {
					info.selected = false;
					d->postUpdate(i);
				}
			}
		}
		d->postUpdate(index);
		emit segmentSelected(index);
	}
};

void NBSegmentControl::setSegmentEnabled(int index, bool enabled) {

	if (!d->validIndex(index))
		return;

	if (d->segments[index].enabled != enabled) {
		d->segments[index].enabled = enabled;
		d->postUpdate(index);
	}
};

void NBSegmentControl::setSelectionBehavior(SelectionBehavior behavior) {

	if (d->selectionBehavior == behavior)
		return;
	d->selectionBehavior = behavior;
	if (behavior == SelectOne) {
		// This call will do the right thing.
		setSegmentSelected(d->lastSelected, true);
	} else if (behavior == SelectNone) {
		d->lastSelected = -1;
		const int segmentCount = d->segments.count();
		for (int i = 0; i < segmentCount; ++i) {
			d->segments[i].selected = false;
		}
		d->postUpdate(-1);
	}
};

NBSegmentControl::SelectionBehavior NBSegmentControl::selectionBehavior() const {

	return d->selectionBehavior;
};

void NBSegmentControl::setSegmentText(int index, const QString &text) {

	if (!d->validIndex(index))
		return;

	if (d->segments[index].text != text) {
		d->segments[index].text = text;
		d->postUpdate(index, true);
		QKeySequence newMnemonic = QKeySequence::mnemonic(text);
		setShortcut(index,newMnemonic);
	}
};

bool NBSegmentControl::segmentEnabled(int index) const {

	if (d->validIndex(index))
		return d->segments[index].enabled;
	return false;
};

QString NBSegmentControl::segmentText(int index) const {

	return d->validIndex(index) ? d->segments.at(index).text : QString();
};

void NBSegmentControl::setSegmentIcon(int index, const QIcon &icon) {

	if (!d->validIndex(index))
		return;

	d->segments[index].icon = icon;
	d->postUpdate(index, true);
};

QIcon NBSegmentControl::segmentIcon(int index) const {

	return d->validIndex(index) ? d->segments.at(index).icon : QIcon();
};

void NBSegmentControl::setIconSize(const QSize &size) {

	if (d->iconSize == size)
		return;

	d->iconSize = size;
	d->postUpdate(-1, true);
};

QSize NBSegmentControl::iconSize() const {

	return d->iconSize;
};

void NBSegmentControl::setSegmentMenu(int index, QMenu *menu) {

	if (!d->validIndex(index))
		return;

	if (menu != d->segments[index].menu) {
		QMenu *oldMenu = d->segments[index].menu;
		d->segments[index].menu = menu;
		delete oldMenu;
		d->postUpdate(index, true);
	}
};

QMenu *NBSegmentControl::segmentMenu(int index) const {

	return d->validIndex(index) ? d->segments.at(index).menu : 0;
};

void NBSegmentControl::setSegmentToolTip(int segment, const QString &tipText) {

	if (!d->validIndex(segment))
		return;

	d->segments[segment].toolTip = tipText;
};

QString NBSegmentControl::segmentToolTip(int segment) const {

	return d->validIndex(segment) ? d->segments.at(segment).toolTip : QString();
};

void NBSegmentControl::setSegmentWhatsThis(int segment, const QString &whatsThisText) {

	if (!d->validIndex(segment))
		return;

	d->segments[segment].whatsThis = whatsThisText;
};

QString NBSegmentControl::segmentWhatsThis(int segment) const {

	return d->validIndex(segment) ? d->segments.at(segment).whatsThis : QString();
};

QSize NBSegmentControl::segmentSizeHint(int segment) const {

	QSize size;
	const SegmentInfo &segmentInfo = d->segments[segment];
	QFontMetrics fm(font());
	size = fm.size(0, segmentInfo.text);
	if (!segmentInfo.icon.isNull()) {
		QSize size2 = segmentInfo.icon.actualSize(iconSize());
		size.rwidth() += size2.width();
		size.rheight() = qMax(size.height(), size2.height());
	}
	NBStyleOptionSegmentControlSegment opt;
	opt.initFrom(this);
	opt.text = segmentInfo.text;
	opt.icon = segmentInfo.icon;
	opt.iconSize = d->iconSize;
	size = segmentSizeFromContents(&opt, size);
	return size;
};

QSize NBSegmentControl::sizeHint() const {

	d->layoutSegments();
	QRect rect;
	const int segmentCount = d->segments.count();
	for (int i = 0; i < segmentCount; ++i) {
		rect = rect.united(segmentRect(i));
	}
	return rect.size();
};

QRect NBSegmentControl::segmentRect(int index) const {

	return d->validIndex(index) ? d->segments[index].rect : QRect();
};

int NBSegmentControl::segmentAt(const QPoint &pos) const {

	const int segmentCount = d->segments.count();
	for (int i = 0; i < segmentCount; ++i) {
		QRect rect = segmentRect(i);
		if (rect.contains(pos))
			return i;
	}
	return -1;
};

void NBSegmentControl::setShortcut( int idx, const QKeySequence &key) {

	if (d->segments[idx].shortcutId != 0)
		releaseShortcut(d->segments[idx].shortcutId);
	d->segments[idx].shortcut = key;
	d->segments[idx].shortcutId = grabShortcut(key);
};

QKeySequence NBSegmentControl::shortcut( int index ) const {

	return d->segments[index].shortcut;
};

void NBSegmentControl::keyPressEvent(QKeyEvent *event) {

	if (event->key() != Qt::Key_Left && event->key() != Qt::Key_Right && event->key() != Qt::Key_Space && event->key() != Qt::Key_Tab) {
		event->ignore();
		return;
	}

	if (event->key() == Qt::Key_Space) {
		d->pressedIndex = d->focusIndex = d->focusIndex;
		d->postUpdate(d->wasPressed);
		animateClick( d->focusIndex );
	}

	else if (event->key() == Qt::Key_Tab) {
		int dx = 1;
		for (int index = d->focusIndex + dx; d->validIndex(index); index += dx) {
			if (d->segments[index].enabled) {
				d->focusIndex = index;
				update();
				break;
			}
		}
	}

	else {
		int dx = event->key() == (isRightToLeft() ? Qt::Key_Right : Qt::Key_Left) ? -1 : 1;
		for (int index = d->focusIndex + dx; d->validIndex(index); index += dx) {
			if (d->segments[index].enabled) {
				d->focusIndex = index;
				update();
				break;
			}
		}
	}
};

void NBSegmentControl::keyReleaseEvent(QKeyEvent *event) {

	if (event->key() == Qt::Key_Space) {
		int index = d->pressedIndex;
		if (d->selectionBehavior != SelectNone) {
			if (d->selectionBehavior == SelectAll) {
				setSegmentSelected(index, !d->segments[index].selected);
			} else {
				setSegmentSelected(index, true);
			}
		}
		d->postUpdate(index);
		d->pressedIndex = -1;
	}
	QWidget::keyReleaseEvent(event);
};


void NBSegmentControl::paintEvent(QPaintEvent *) {

	d->layoutSegments();
	QPainter p(this);
	NBStyleOptionSegmentControlSegment segmentInfo;
	const int segmentCount = d->segments.count();
	for (int i = 0; i < segmentCount; ++i) {
		initStyleOption(i, &segmentInfo);
		drawSegmentControlSegment(&segmentInfo, &p, this);
	}
};

void NBSegmentControl::mousePressEvent(QMouseEvent *event) {

	int index = segmentAt(event->pos());
	if (segmentEnabled(index)) {
		d->wasPressed = d->focusIndex = d->pressedIndex = segmentAt(event->pos());
		d->postUpdate(d->pressedIndex);

		emit pressed( index );
	}
};

void NBSegmentControl::mouseMoveEvent(QMouseEvent *event) {
	int index = segmentAt(event->pos());
	if (index != d->wasPressed) {
		d->pressedIndex = -1;
		d->postUpdate(d->wasPressed);
	} else if (index == d->wasPressed && d->pressedIndex == -1) {
		d->pressedIndex = d->wasPressed;
		d->postUpdate(d->wasPressed);
	}
};

void NBSegmentControl::mouseReleaseEvent(QMouseEvent *event) {
	int index = segmentAt(event->pos());
	// This order of reset is important.
	d->pressedIndex = -1;
	if (index == d->wasPressed && d->selectionBehavior != SelectNone) {
		if (d->selectionBehavior == SelectAll) {
			setSegmentSelected(index, !d->segments[index].selected);
		} else {
			setSegmentSelected(index, true);
		}
	}

	emit released( index );
	emit clicked( index );

	d->postUpdate(index);
	d->wasPressed = -1;
};

bool NBSegmentControl::event(QEvent *event) {

	if (event->type() == QEvent::Shortcut) {
		QShortcutEvent *se = static_cast<QShortcutEvent *>(event);

		for ( int i = 0; i < d->segments.count(); i++ ) {
			if ( d->segments[ i ].shortcutId == se->shortcutId() ) {
				if ( not se->isAmbiguous() ) {
					if (!d->animateTimer.isActive())
						animateClick( i );
				}
				return true;
			}
		}
		return false;
	}

	return QWidget::event(event);
};

void NBSegmentControl::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == d->animateTimer.timerId() ) {
		d->animateTimer.stop();
		int idx = d->pressedIndex;
		d->pressedIndex = -1;
		d->postUpdate( idx );
		emit released( idx );
		emit clicked( idx );
	}
};

void NBSegmentControl::initStyleOption(int segment, QStyleOption *option) const {

	if (!option || !d->validIndex(segment))
		return;
	option->initFrom(this);
	if (segment == d->pressedIndex)
		option->state |= QStyle::State_Sunken;

	// ## Change to qstyleoption_cast
	if (NBStyleOptionSegmentControlSegment *sgi = static_cast<NBStyleOptionSegmentControlSegment *>(option)) {
		sgi->iconSize = d->iconSize;
		const SegmentInfo &segmentInfo = d->segments[segment];
		if (d->segments.count() == 1) {
			sgi->position = NBStyleOptionSegmentControlSegment::OnlyOneSegment;
		} else if (segment == 0) {
			sgi->position = NBStyleOptionSegmentControlSegment::Beginning;
		} else if (segment == d->segments.count() - 1) {
			sgi->position = NBStyleOptionSegmentControlSegment::End;
		} else {
			sgi->position = NBStyleOptionSegmentControlSegment::Middle;
		}

		if (hasFocus() && segment == d->focusIndex)
			sgi->state |= QStyle::State_HasFocus;
		else
			sgi->state &= ~QStyle::State_HasFocus;

		if (segmentInfo.enabled && isEnabled())
			sgi->state |= QStyle::State_Enabled;
		else
			sgi->state &= ~QStyle::State_Enabled;

		if (segmentInfo.selected) {
			sgi->state |= QStyle::State_Selected;
		} else {
			if (d->validIndex(segment - 1) && d->segments[segment - 1].selected) {
				sgi->selectedPosition = NBStyleOptionSegmentControlSegment::PreviousIsSelected;
			} else if (d->validIndex(segment + 1) && d->segments[segment + 1].selected) {
				sgi->selectedPosition = NBStyleOptionSegmentControlSegment::NextIsSelected;
			} else {
				sgi->selectedPosition = NBStyleOptionSegmentControlSegment::NotAdjacent;
			}
		}
		sgi->rect = segmentInfo.rect;
		sgi->text = segmentInfo.text;
		sgi->icon = segmentInfo.icon;
	}
};

void NBSegmentControl::animateClick(int index, int msec) {

	if (!isEnabled())
		return;

	d->wasPressed = d->focusIndex = d->pressedIndex = index;
	d->postUpdate(d->pressedIndex);

	repaint(); //flush paint event before invoking potentially expensive operation
	QApplication::flush();

	if (!d->animateTimer.isActive())
		emit pressed( index );

	d->animateTimer.start(msec, this);
};
