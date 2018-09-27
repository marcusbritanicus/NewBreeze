/*
	*
	* PdfView - PdfView class header
	*
*/

#pragma once

#include <QtCore>
#include <QtGui>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#endif

#include "PdfDocument.hpp"
#include "MuPdfDocument.hpp"
#include "PopplerDocument.hpp"

class PdfView : public QScrollArea {
	Q_OBJECT

	public:
		enum ViewMode {
			SinglePageView = 0x54FC79,								// One page at once
			DoublePageView,											// Two pages at once
			FitPageToWidth,											// Zoom the page to fit the width of the view
			FitPageToHeight,										// Zoom the page to fit the height of the view
			BookView												// First Page Treated as cover, rest will be layout like a book.
		};

		enum LayoutMode {
			SinglePage = 0x1D83E6,									// Only one page will be displayed at once.
			Continuous,												// All the pages will be displayed in a single shot
			Book													// First page treated as cover, rest will be layout like a book.
		};

		/*
			*
			* SinglePageView, DoublePageView, FitPageToWidth and FitPageToHeight can be layout using only SinglePage and Continuous layout modes.
			* BookView can be Book layout mode and vice versa
			*
		*/

		enum RenderBackend {
			MuPdfRenderBackend = 0xC32C65,
			PopplerRenderBackend
		};

		PdfView( QWidget *parent );

		void loadPdfDocument( QString, RenderBackend );

		int viewMode() const {

			return mViewMode;
		};

		void setViewMode( int viewMode ) {

			mViewMode = viewMode;

			if ( viewMode == BookView )
				mLytMode = Book;

			reshapeView();
		};

		int layoutMode() const {

			return mLytMode;
		};

		void setLayoutMode( int lytMode ) {

			mLytMode = lytMode;

			if ( mLytMode == Book )
				mViewMode = BookView;

			reshapeView();
		};

		qreal zoom();
		void setZoom( qreal );

	private:
		PdfDocument *PdfDoc;
		QHash<int, QImage> renderedImages;
		QHash<int, QLabel*> pages;

		QList<int> visiblePages;

		int currentPage;
		qreal mZoom;

		int mViewMode;
		int mLytMode;

		void reshapeView();

		bool isPageVisible( int pgNo );

	private Q_SLOTS:
		void slotZoomIn() {

			/* Maximum default zoom 400% */
			if ( mZoom >= 4.0 )
				return;

			/* Book view has fixed zoom */
			if ( mViewMode == BookView )
				return;

			if ( mViewMode != DoublePageView )
				mViewMode = SinglePageView;

			setZoom( mZoom + 0.1 );
		};

		void slotZoomOut() {

			/* Minimum default zoom 10% */
			if ( mZoom <= 0.1 )
				return;

			/* Book view has fixed zoom */
			if ( mViewMode == BookView )
				return;

			if ( mViewMode != DoublePageView )
				mViewMode = SinglePageView;

			setZoom( mZoom - 0.1 );
		};

		void renderPages();

	protected:
		void resizeEvent( QResizeEvent *rEvent );
		void wheelEvent( QWheelEvent *wEvent );

		void keyPressEvent( QKeyEvent *kEvent );

	Q_SIGNALS:
		void closePreview();
};
