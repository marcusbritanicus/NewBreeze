/*
	*
	* NBDefaultsWidget.hpp - NBDefaultsWidget.cpp header
	*
*/

#pragma once

#include "Global.hpp"

class NBPreviewWidget : public QWidget {
	Q_OBJECT

	public:
		NBPreviewWidget( QStringList paths, QWidget* );

	private:
		QMovie *movie;

	protected:
		void resizeEvent( QResizeEvent *rEvent );
};
