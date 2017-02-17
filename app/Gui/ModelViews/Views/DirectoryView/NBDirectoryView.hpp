/*
	*
	* NBDirectoryView.hpp - NewBreeze DirectoryView class header
	*
*/

#pragma once

#include "Global.hpp"

#include "NBDirTreeModel.hpp"

class NBDirViewDelegate : public QStyledItemDelegate {
	Q_OBJECT

	public:
		void paint( QPainter*, const QStyleOptionViewItem&, const QModelIndex& ) const;
};

class NBDirectoryView : public QTreeView {
	Q_OBJECT

	public:
		NBDirectoryView( QWidget *parent = 0 );

		QString currentBranch() const;

	public Q_SLOTS:
		void setCurrentBranch( QString );

	private:
		NBDirTreeModel *mdl;
};
