/*
	*
	* NBIconViewRestricted.hpp - NewBreeze Restricted IconViewer Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBItemViewModel.hpp"
#include "NBIconDelegate.hpp"
#include "NBIconView.hpp"

class NBIconViewRestricted : public NBIconView {
	Q_OBJECT

	public:
		NBIconViewRestricted( NBItemViewModel*, QWidget* );

		NBItemViewModel* mdl;

	protected:
		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );
		void mouseReleaseEvent( QMouseEvent * );
		void mouseDoubleClickEvent( QMouseEvent * );

		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent* );
		void dropEvent( QDropEvent* );
};
