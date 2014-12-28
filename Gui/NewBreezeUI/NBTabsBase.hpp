/*
	*
	* NBTabsBase.hpp - NBTabsBase Header
	*
*/

#ifndef NBTABSBASE_HPP
#define NBTABSBASE_HPP

#include <Global.hpp>
#include <NewBreezeUI.hpp>

class NBTabsBase : public QWidget {
	Q_OBJECT

	public :
		NBTabsBase();

		void addWidget( NewBreezeUI* );
		void removeWidget( NewBreezeUI* );

		NewBreezeUI* currentWidget();
		NewBreezeUI* widget( int );

		int count();

		void setCurrentIndex( int );
		int currentIndex();
		int indexOf( NewBreezeUI* );

	private:
		QList<NewBreezeUI*> widgetList;
		int __currentIndex;
		QHBoxLayout *stackLyt;
};

// The code for @f NBStartup() and ReadSettings() is in NBStartup.cpp
void NBStartup();

#endif
