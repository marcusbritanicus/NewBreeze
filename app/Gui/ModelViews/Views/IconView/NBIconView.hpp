/*
	*
	* NBIconView.hpp - NewBreeze Icon Viewer Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBItemViewModel.hpp"
#include "NBIconDelegate.hpp"
#include "NBGuiWidgets.hpp"
#include "NBGuiFunctions.hpp"
#include "NBIOProcess.hpp"
#include "NBProcessManager.hpp"
#include "NBErrorsDialog.hpp"

class NBIconView : public QAbstractItemView {
	Q_OBJECT

	public:
		NBIconView( NBItemViewModel*, QWidget *parent );

		// Set the item model
		void setModel( QAbstractItemModel *model );

		// Update the view mode
		void updateViewMode();

		// Category drawing height : myCategoryHeight
		int categoryHeight() const;
		void setCategoryHeight( int );

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

		// Given the index, return the visual rect
		QRect visualRect( const QModelIndex &index ) const;

		// Given the category index get the rectangle for it
		QRect categoryRect( int ) const;

		// Given the index scroll the view
		void scrollTo( const QModelIndex &index, QAbstractItemView::ScrollHint );

		// Get the index at a given point
		QModelIndex indexAt( const QPoint &position ) const;

		// Get the category at a given point
		QString categoryAt( const QPoint &position ) const;

		// Return the selected indexes
		QModelIndexList selection();

		// Is the index visible in the viewport?
		bool isIndexVisible( QModelIndex ) const;

	protected slots:
		void dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );
		void rowsInserted( const QModelIndex &parent, int start, int end );
		void rowsAboutToBeRemoved( const QModelIndex &parent, int start, int end );
		void updateGeometries();
		void reload();

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
		void mouseDoubleClickEvent( QMouseEvent * );

		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent* );
		void dropEvent( QDropEvent* );

		void keyPressEvent( QKeyEvent* );

	private:
		// Grid size for the indexes: myGridSizeMin, myGridSize
		void computeGridSize( QSize );

		QModelIndex moveCursorCategorized( QAbstractItemView::CursorAction cursorAction );
		QModelIndex moveCursorNonCategorized( QAbstractItemView::CursorAction cursorAction );

		void setCategorySelected( QString, bool );

		// Cursor Movement Helpers
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
		void calculateNonCategorizedRects() const;

		void calculateCategorizedIconsRects() const;
		void calculateCategorizedTilesRects() const;
		void calculateCategorizedDetailsRects() const;

		void calculateNonCategorizedIconsRects() const;
		void calculateNonCategorizedTilesRects() const;
		void calculateNonCategorizedDetailsRects() const;

		void computeRowsAndColumns() const;

		QRect viewportRectForRow( int row ) const;

		void paintCategory( QPainter *painter, const QRect &rectangle, const QString &text ) const;
		void paintSelection( QPainter *painter, const QModelIndexList ) const;

		void showHideCategory( QString );
		void toggleFoldCategory( QString );
		bool canShowIndex( QModelIndex );

		// FileSystem Model
		NBItemViewModel *cModel;

		// ViewMode
		mutable QString currentViewMode;

		// Selection
		mutable QModelIndexList mSelectedIndexes;
		mutable QStringList mSelectedCategories;
		mutable QModelIndex mSelStartIdx;

		// Icon rects
		mutable int idealHeight;
		mutable QHash<int, QPoint> rectForRow;
		mutable QHash<int, QRect> rectForCategory;
		mutable bool hashIsDirty;

		// Category Height
		mutable int myCategoryHeight;

		// Category Spacing
		mutable int myCategorySpacing;

		// Contents Margins
		mutable QMargins myContentsMargins;

		// Inlay Margins
		mutable QMargins myInlayMargins;

		// Grid Size
		mutable QSize myGridSizeMin;
		mutable QSize myGridSize;
		mutable QSize myItemSize;

		// Icon Size
		mutable QSize myIconSize;

		// Persistent vertical column
		mutable int persistentVCol;

		// Items per visual row
		mutable int itemsPerRow;
		mutable int numberOfRows;
		mutable int padding;

		// Category List
		mutable QStringList categoryList;

		// Hidden Categories
		mutable QStringList hiddenCategories;

		// Folded Categories
		mutable QStringList foldedCategories;

		QPoint dragStartPosition;

	private slots:
		void zoomIn();
		void zoomOut();

		void emitCML();

	Q_SIGNALS :
		void open( QModelIndex );
		void open( QString );

		void contextMenuRequested( QPoint );

		void copy( QStringList, QString );
		void move( QStringList, QString );
		void link( QStringList, QString );

		void updateViewMode( QString );

		void selectionChanged();
};
