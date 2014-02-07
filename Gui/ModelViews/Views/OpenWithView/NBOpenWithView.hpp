/*
	*
	* NBOpenWithView.hpp - NewBreeze Application Viewer Class Header
	*
*/

#pragma once
#ifndef NBOPENWITHVIEW_HPP
#define NBOPENWITHVIEW_HPP

#include <Global.hpp>
#include <NBApplicationsModel.hpp>
#include <NBOpenWithDelegate.hpp>
#include <NBTools.hpp>

class NBOpenWithView : public QAbstractItemView {
	Q_OBJECT

	public:
		NBOpenWithView();

		// Set the item model
		void setModel( QAbstractItemModel *model );

		// Category drawing height : myCategoryHeight
		int categoryHeight() const;
		void setCategoryHeight( int );

		// Item Height for the indexes: myItemHeight
		int itemHeight() const;
		void setItemHeight( int );

		// Icon Size for the indexes: myIconSize
		QSize iconSize() const;
		void setIconSize( QSize );
		void setIconSize( int, int );

		// Contents Margins
		QMargins contentsMargins() const;
		void setContentsMargins( QMargins );
		void setContentsMargins( int, int, int, int );

		// Inlay margins: myInlayMargins
		QMargins inlayMargins() const;
		void setInlayMargins( QMargins );
		void setInlayMargins( int, int, int, int );

		// Space between the categories: myCategorySpacing
		int categorySpacing() const;
		void setCategorySpacing( int );

		// Given the index, return the visual index
		QRect visualRect( const QModelIndex &index ) const;

		// Given the category index get the rectangle for it
		QRect categoryRect( int ) const;

		// Given the index scroll the view
		void scrollTo( const QModelIndex &index, QAbstractItemView::ScrollHint );

		// Get the index at a given point
		QModelIndex indexAt( const QPoint &position ) const;

		// Get the category at a given point
		QString categoryAt( const QPoint &position ) const;

	protected slots:
		void dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );
		void rowsInserted( const QModelIndex &parent, int start, int end );
		void rowsAboutToBeRemoved( const QModelIndex &parent, int start, int end );
		void updateGeometries();

	protected:
		QModelIndex moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers );
		bool isIndexHidden( const QModelIndex& ) const { return false; }
		int horizontalOffset() const;
		int verticalOffset() const;
		void scrollContentsBy( int dx, int dy );
		void setSelection( const QRect &rect, QFlags<QItemSelectionModel::SelectionFlag> flags );
		QRegion visualRegionForSelection( const QItemSelection &selection ) const;

		void paintEvent( QPaintEvent* );
		void resizeEvent( QResizeEvent* );
		void mousePressEvent( QMouseEvent *event );
		void mouseDoubleClickEvent( QMouseEvent *event );

	private:
		void calculateRectsIfNecessary() const;

		QRect viewportRectForRow( int row ) const;

		void paintCategory( QPainter *painter, const QRect &rectangle, const QString &text ) const;
		QPixmap pixmapForCategory( QString ) const;

		NBApplicationsModel *appModel;

		mutable int idealHeight = 0;
		mutable int viewWidth = 0;
		mutable QHash<int, int> yOffsetForRow;
		mutable QHash<int, int> yOffsetForCategory;
		mutable bool hashIsDirty = true;

		// Category Height
		mutable int myCategoryHeight = 32;

		// Category Spacing
		mutable int myCategorySpacing = 10;

		// Contents Margins
		mutable QMargins myContentsMargins = QMargins( 10, 10, 10, 10 );

		// Inlay Margins
		mutable QMargins myInlayMargins = QMargins( 10, 0, 10, 0 );

		// Grid Size
		mutable int myItemHeight = 32;

		// Icon Size
		mutable QSize myIconSize = QSize( 24, 24 );
};

#endif
