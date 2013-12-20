/*
	*
	* NBIconViewRestricted.hpp - NewBreeze Restricted IconViewer Class Header
	*
*/

#ifndef NBICONVIEWRESTRICTED_HPP
#define NBICONVIEWRESTRICTED_HPP

#include <Global.hpp>
#include <NBFileSystemModel.hpp>
#include <NBIconDelegate.hpp>
#include <NBTools.hpp>
#include <NBIconView.hpp>

class NBIconViewRestricted : public NBIconView {
	Q_OBJECT

	public:
		enum class ViewType {
			TilesView             = 0x01,
			IconsView             = 0x02,
			DetailsView           = 0x03
		};

		NBIconViewRestricted( NBFileSystemModel* );

		NBFileSystemModel* mdl;

	protected:
		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );
		void mouseReleaseEvent( QMouseEvent * );
		void mouseDoubleClickEvent( QMouseEvent * );

		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent* );
		void dropEvent( QDropEvent* );
};

#endif
