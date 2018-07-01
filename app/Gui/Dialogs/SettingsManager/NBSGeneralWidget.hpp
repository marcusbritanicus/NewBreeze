/*
	*
	* NBSGeneralWidget.hpp - NBSGeneralWidget.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDialog.hpp"
#include "NBButtons.hpp"
#include "NBButton.hpp"

class NBSGeneralWidget : public QWidget {
	Q_OBJECT

	public:
		NBSGeneralWidget( QWidget* );

	private:
		/* Grouping */
		QGroupBox *perFolderEnabled;
		QComboBox *defaultViewModeCB;			// Icon, Tiles or Details view
		QSlider *defaultIconSizeS;				// 16 - 128
		QLabel *iconSizeL;						// Show the current icon size
		QCheckBox *defaultGroupingCB;			// Show in groups enabled?
		QCheckBox *defaultSortCaseCB;			// Case sensitive sort?
		QComboBox *defaultSortClmnCB;			// Sort column: Name, Size, Type or Date?

		/* SidePanel */
		QGroupBox *showSidePanelGB;				// Show the SidePanel/SideBar?
		QRadioButton *sidePanelRB;				// Show the Modern Side Panel
		QRadioButton *sideBarRB;				// Show the Classic SideBar

		/* File previews */
		QGroupBox *filePreviewGB;				// Show image previews?
		QCheckBox *imagePreviewCB;				// Show image thumbnails?
		QCheckBox *pdfPreviewCB;				// Show pdf thumbnails?
		QCheckBox *djvuPreviewCB;				// Show djvu thumbnails?
		QCheckBox *odfPreviewCB;				// Show ODF thumbnails?
		QCheckBox *videoPreviewCB;				// Show videos thumbnails?

		/* Others */
		QCheckBox *filterFoldersCB;				// Should we filter folders while searching?
		QCheckBox *showTrayIconCB;				// Show tray icon?
		QCheckBox *openWithCB;					// Open with SuperStart?
		QCheckBox *extendedIOCB;				// Direct Copy/Move?
		QCheckBox *paintOverlayCB;				// Paint Icon Overlays?
		QCheckBox *logDebugCB;					// Enable Debug Logging?

		/* Terminal */
		QGroupBox *termGB;
		QCheckBox *pluginCB;					// Use Inbuilt Plugin?
		QListWidget *termsLW;					// List of terminals detected/added
		QPushButton *addBtn;					// Add a terminal
		QPushButton *defaultBtn;				// Make this terminal default

	private slots:
		/* View */
		void handlePerFolderChanged( bool );
		void handleViewModeChanged( int );
		void handleIconSizeChanged( int );
		void handleGroupingChanged( bool );
		void handleSortCaseChanged( bool );
		void handleSortColumnChanged( int );

		/* SidePanel */
		void handleShowSidePanelToggled( bool );
		void handleSidePanelChoice();

		/* File Previews */
		void handlePreviewsChanged();

		/* Others */
		void handleFilterDirsChanged( bool );
		void handleTrayIconChanged( bool );
		void handleOpenWithToggled();
		void handleExtendedIOChanged( bool );
		void handlePaintOverlayChanged( bool );
		void handleLogDebugChanged( bool );

		/* Terminal */
		void handleDefaultClicked();
		void handleAddTermClicked();
};

class NBAddVTEDialog : public NBDialog {
	Q_OBJECT

	public :
		NBAddVTEDialog( QWidget *parent );

	private:
		NBSegmentButton *btns;
		QCheckBox *wDirCB;
		QLineEdit *execLE, *workLE, *cmdLE;

	private Q_SLOTS:
		void addTerminal();
		void handleCBPressed();
};
