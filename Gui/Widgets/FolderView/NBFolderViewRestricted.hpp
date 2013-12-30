/*
	*
	* NBFolderView.hpp - NewBreeze FolderView Class Header
	*
*/

#ifndef NBFOLDERVIEWRESTRICTED_HPP
#define NBFOLDERVIEWRESTRICTED_HPP

#include <Global.hpp>
#include <NBFileSystemModel.hpp>
#include <NBIconDelegate.hpp>
#include <NBTools.hpp>

class NBFolderViewRestricted : public QListView {
	Q_OBJECT

	public :
		NBFolderViewRestricted( QString path = QString() );

	private :
		void updateViewMode();

		QString currentPath;
		NBFileSystemModel *model;
};

#endif
