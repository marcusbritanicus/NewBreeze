/*
	*
	* NBOpenWithView.hpp - NewBreeze Application Viewer Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBItemViewModel.hpp"
#include "NBOpenWithDelegate.hpp"
#include "NBGuiFunctions.hpp"

class NBOpenWithView : public QAbstractItemView {
	Q_OBJECT

	public:
		NBOpenWithView();

		/* Set the item model */
		void setModel( QAbstractItemModel *model );

		/* Given the index, return the visual rect */
		QRect visualRect( const QModelIndex &index ) const;

		/* Given the category index get the rectangle for it */
		QRect categoryRect( int ) const;

		/* Given the index scroll the view */
		void scrollTo( const QModelIndex &index, QAbstractItemView::ScrollHint );

		/* Get the index at a given point */
		QModelIndex indexAt( const QPoint &position ) const;

		/* Get the category at a given point */
		QString categoryAt( const QPoint &position ) const;

		/* Return the selected indexes */
		QModelIndexList selection();

		/* Is the index visible in the viewport? */
		bool isIndexVisible( QModelIndex ) const;

	protected slots:
		void dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );
		void rowsInserted( const QModelIndex &parent, int start, int end );
		void rowsAboutToBeRemoved( const QModelIndex &parent, int start, int end );
		void updateGeometries();
		void reload();

		void currentChanged( const QModelIndex &, const QModelIndex & );

	protected:
		QModelIndex moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers );
		bool isIndexHidden( const QModelIndex& ) const { return false; }
		int horizontalOffset() const;
		int verticalOffset() const;
		void scrollContentsBy( int dx, int dy );
		void setSelection( const QRect &rect, QFlags<QItemSelectionModel::SelectionFlag> flags );
		QRegion visualRegionForSelection( const QItemSelection &selection ) const;

		QModelIndexList selectedIndexes();

		void paintEvent( QPaintEvent* );
		void resizeEvent( QResizeEvent* );

		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );
		void mouseReleaseEvent( QMouseEvent * );
		void mouseDoubleClickEvent( QMouseEvent * );

	private:
		/* Cursor Movement Helpers */
		QModelIndex nextIndex();
		QModelIndex prevIndex();
		QModelIndex aboveIndex();
		QModelIndex belowIndex();

		QModelIndex firstIndex();
		QModelIndex lastIndex();

		QModelIndex indexPageBelow();
		QModelIndex indexPageAbove();

		void calculateRectsIfNecessary() const;

		void calculateCategorizedRects() const;

		QRect viewportRectForRow( int row ) const;

		void paintCategory( QPainter *painter, const QRect &rectangle, const QString &text ) const;
		void paintSelection( QPainter *painter, const QModelIndexList ) const;

		/* Categorized Item Model */
		NBItemViewModel *appModel;

		/* ViewMode */
		mutable QString currentViewMode;

		/* Selection */
		mutable QModelIndexList mSelectedIndexes;
		mutable QStringList mSelectedCategories;
		mutable QModelIndex mSelStartIdx;

		/* Icon rects */
		mutable int idealHeight;
		mutable QHash<int, QRect> rectForRow;
		mutable QHash<int, QRect> rectForCategory;
		mutable bool hashIsDirty;

		/* Category Height */
		mutable int myCategoryHeight;

		/* Category Spacing */
		mutable int myCategorySpacing;

		/* Contents Margins */
		mutable QMargins myContentsMargins;

		/* Inlay Margins */
		mutable QMargins myInlayMargins;

		/* View Width */
		mutable int viewWidth;

		/* Icon Size */
		mutable QSize myIconSize;

		/* Persistent vertical column */
		mutable int persistentVCol;
		mutable QPair<int, int> curIdx;

		/* Items per visual row */
		mutable int numberOfRows;
		mutable int padding;

		/* Category List */
		mutable QStringList categoryList;

		/* Hidden Categories */
		mutable QStringList hiddenCategories;

	private slots:
		void showHideCategory( QString );
		void toggleFoldCategory( QString );

		void showCategory( QString );
		void foldCategory( QString );
		void hideCategory( QString );

	Q_SIGNALS :
		void open( QModelIndex );
		void open( QString );

		void peek( QModelIndex );

		void contextMenuRequested( QPoint );
		void actionsMenuRequested( QPoint );

		void copy( QStringList, QString );
		void move( QStringList, QString );
		void link( QStringList, QString );

		void selectionChanged();
};
