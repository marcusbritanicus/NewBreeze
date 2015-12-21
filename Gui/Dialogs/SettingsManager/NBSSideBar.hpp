/*
	*
	* NBSSideBar.hpp - NBSSideBar.cpp Header
	*
*/

#pragma once
#ifndef NBSSIDEBAR_HPP
#define NBSSIDEBAR_HPP

#include <Global.hpp>

class NBSSideBar : public QWidget {
	Q_OBJECT

	public:
		NBSSideBar( QWidget *parent  = NULL );

	Q_SIGNALS:
		void loadSettingsCategory( int );
};

#endif
