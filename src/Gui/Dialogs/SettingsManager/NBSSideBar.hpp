/*
	*
	* NBSSideBar.hpp - NBSSideBar.cpp Header
	*
*/

#pragma once

#include <Global.hpp>

class NBSSideBar : public QWidget {
	Q_OBJECT

	public:
		NBSSideBar( QWidget *parent  = NULL );

	Q_SIGNALS:
		void loadSettingsCategory( int );
};
