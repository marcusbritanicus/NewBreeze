/*
	*
	* NBTabsBase.cpp - NewBreeze Tabs Class
	*
*/

#include <NBTabsBase.hpp>

NBTabsBase::NBTabsBase() : QWidget() {

	__currentIndex = 0;

	setContentsMargins( QMargins() );
	setFocusPolicy( Qt::NoFocus );

	stackLyt = new QHBoxLayout();
	setLayout( stackLyt );
};

void NBTabsBase::addWidget( NewBreezeUI *widget ) {

	widgetList << widget;
	stackLyt->addWidget( widget );
	widget->hide();
};

void NBTabsBase::removeWidget( NewBreezeUI *widget ) {

	widgetList.removeAt( widgetList.indexOf( widget ) );
	stackLyt->removeWidget( widget );
};

NewBreezeUI* NBTabsBase::currentWidget() {

	return widgetList.value( __currentIndex );
};

NewBreezeUI* NBTabsBase::widget( int idx ) {

	return widgetList.value( idx );
};

int NBTabsBase::count() {

	return widgetList.count();
};

void NBTabsBase::setCurrentIndex( int idx ) {

	widgetList.value( __currentIndex )->hide();
	widgetList.value( idx )->show();
	__currentIndex = idx;
};

int NBTabsBase::currentIndex() {

	return __currentIndex;
};

int NBTabsBase::indexOf( NewBreezeUI *wdgt ) {

	return widgetList.indexOf( wdgt );
};
