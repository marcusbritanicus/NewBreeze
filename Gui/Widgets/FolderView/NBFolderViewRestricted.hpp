/*
	*
	* NBFolderView.hpp - NewBreeze FolderView Class Header
	*
*/

#ifndef NBFOLDERVIEWRESTRICTED_HPP
#define NBFOLDERVIEWRESTRICTED_HPP

#include <Global.hpp>
#include <NBFileSystemModel.hpp>

class NBFolderViewRestricted : public QListView {
	Q_OBJECT

	public :
		NBFolderViewRestricted( QString path = QString() );

		bool showHidden;

	private :
		void updateViewMode();

		QString currentPath;
		NBFileSystemModel *fsModel;
};

#endif
