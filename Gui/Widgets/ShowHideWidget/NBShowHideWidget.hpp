/*
	*
	* NBShowHideWidget.hpp - NBShowHideWidget.cpp header
	*
*/

#pragma once
#ifndef NBSHOWHIDEWIDGET_HPP
#define NBSHOWHIDEWIDGET_HPP

#include <Global.hpp>

class NBExpanderLabel: public QToolButton {
	Q_OBJECT

	public:
		NBExpanderLabel( QString text );

	private:
		bool widgetShown;

	protected:
		void mousePressEvent( QMouseEvent *mEvent );
};

class NBShowHideWidget : public QWidget {
	Q_OBJECT

	public:
		NBShowHideWidget( QString, QWidget * );
		bool isWidgetVisible();

	private:
		NBExpanderLabel *clickLabel;
		QWidget *widget;

	private slots:
		void toggleVisibility();

	signals:
		void visibilityChanged();

};

#endif
