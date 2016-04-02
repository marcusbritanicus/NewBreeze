/*
	*
	* NBSGeneralWidget.hpp - NBSGeneralWidget.cpp header
	*
*/

#pragma once

#include <Global.hpp>

class NBSGeneralWidget : public QWidget {
	Q_OBJECT

	public:
		NBSGeneralWidget( QWidget* );

	private:
		/* Grouping */
		QGroupBox *perFolderEnabled;
		QComboBox *defaultViewModeCB;		// Icon, Tiles or Details view
		QSlider *defaultIconSizeS;			// 16 - 128
		QLabel *iconSizeL;					// Show the current icon size
		QCheckBox *defaultGroupingCB;		// Show in groups enabled?
		QCheckBox *defaultSortCaseCB;		// Case sensitive sort?
		QComboBox *defaultSortClmnCB;		// Sort column: Name, Size, Type or Date?

		/* SidePanel */
		QGroupBox *showSidePanelGB;			// Show the SidePanel/SideBar?
		QRadioButton *sidePanelRB;			// Show the Modern Side Panel
		QRadioButton *sideBarRB;			// Show the Classic SideBar

		/* Others */
		QCheckBox *filterFoldersCB;			// Should we filter folders while searching?
		QCheckBox *nativeTitleBarCB;		// Use native titlebar?
		QCheckBox *showTrayIconCB;			// Show tray icon?
		QCheckBox *openWithCatalogCB;		// Show home folder or catalogs?
		QCheckBox *imagePreviewCB;			// Show image previews?

	private slots:
		/*  */
		void handlePerFolderChanged( bool );
		void handleViewModeChanged( int );
		void handleIconSizeChanged( int );
		void handleGroupingChanged( bool );
		void handleSortCaseChanged( bool );
		void handleSortColumnChanged( int );

		/* SidePanel */
		void handleShowSidePanelToggled( bool );
		void handleSidePanelChoice();

		/*  */
		void handleFilterDirsChanged( bool );
		void handleNativeTitleBarToggle( bool );
		void handleTrayIconChanged( bool );
		void handleOpenWithCatalogToggled( bool );
		void handleCheckStateChanged( int );
};
