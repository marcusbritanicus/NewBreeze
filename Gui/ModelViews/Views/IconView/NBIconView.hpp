/*
	*
	* NBIconView.hpp - NewBreeze Icon Viewer Class Header
	*
*/

#ifndef NBIconView_HPP
#define NBIconView_HPP

#include <Global.hpp>
#include <NBFileSystemModel.hpp>
#include <NBIconDelegate.hpp>
#include <NBTools.hpp>

class NBIconView : public QAbstractItemView {
	Q_OBJECT

	public:
		enum class ViewType {
			SmallListView              = 0x01,
			NormalListView             = 0x02,
			TilesView                  = 0x03,
			SmallIconsView             = 0x04,
			NormalIconsView            = 0x05,
			LargeIconsView             = 0x06,
			HugeIconsView              = 0x07,
			SmallTreeView              = 0x08,
			NormalTreeView             = 0x09
		};

		NBIconView( NBFileSystemModel* );

		// Set the item model
		void setModel( QAbstractItemModel *model );

		// Update the view mode
		void updateViewMode();

		// Category drawing height : myCategoryHeight
		int categoryHeight() const;
		void setCategoryHeight( int );

		// Grid size for the indexes: myGridSizeMin, myGridSize
		QSize gridSize() const;
		void setGridSize( QSize );
		void setGridSize( int, int );

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
		void reload();

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

		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );
		void mouseDoubleClickEvent( QMouseEvent * );

		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent* );
		void dropEvent( QDropEvent* );

	private:
		QModelIndex moveCursorCategorized( QAbstractItemView::CursorAction cursorAction );
		QModelIndex moveCursorNonCategorized( QAbstractItemView::CursorAction cursorAction );

		void calculateRectsIfNecessary() const;

		void calculateCategorizedRects() const;
		void calculateNonCategorizedRects() const;

		void computeRowsAndColumns() const;

		QRect viewportRectForRow( int row ) const;

		void paintCategory( QPainter *painter, const QRect &rectangle, const QString &text ) const;
		QPixmap pixmapForCategory( QString ) const;

		NBFileSystemModel *cModel;

		mutable int idealHeight = 0;
		mutable QHash<int, QPoint> rectForRow;
		mutable QHash<int, QRect> rectForCategory;
		mutable bool hashIsDirty = true;

		// Category Height
		mutable int myCategoryHeight = 24;

		// Category Spacing
		mutable int myCategorySpacing = 10;

		// Contents Margins
		mutable QMargins myContentsMargins = QMargins( 10, 10, 10, 10 );

		// Inlay Margins
		mutable QMargins myInlayMargins = QMargins( 10, 0, 10, 0 );

		// Grid Size
		mutable QSize myGridSizeMin = QSize( 120, 80 );
		mutable QSize myGridSize = QSize( 120, 80 );
		mutable QSize myItemSize = QSize( 110, 70 );

		// Icon Size
		mutable QSize myIconSize = QSize( 48, 48 );

		// Items per cisual row
		mutable int itemsPerRow = 1;
		mutable int numberOfRows = 0;

		QPoint dragStartPosition;

	Q_SIGNALS :
		void open( QModelIndex );
		void contextMenuRequested( QPoint );
		void acopy( QStringList, QString );
		void copy( QStringList, QString );
		void move( QStringList, QString );
		void link( QStringList, QString );
};

#endif
