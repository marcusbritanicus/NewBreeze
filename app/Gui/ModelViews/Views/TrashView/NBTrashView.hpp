/*
	*
	* NBTrashView.hpp - NewBreeze Icon Viewer Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBTrashModel.hpp"
#include "NBTrashDelegate.hpp"
#include "NBGuiWidgets.hpp"
#include "NBGuiFunctions.hpp"

class NBTrashView : public QTreeView {
	Q_OBJECT

	public:
		NBTrashView( NBTrashModel *model, QWidget *parent );

	protected:
		int sizeHintForColumn( int c );
};
