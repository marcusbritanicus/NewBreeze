/*
	*
	* NBAddressBar.hpp - NBAddressBar.cpp Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBGuiWidgets.hpp"
#include "NBCrumbsBar.hpp"
#include "NBButton.hpp"
#include "NBButtons.hpp"
#include "NBMenuButton.hpp"
#include "NBProcessManagerUI.hpp"

class NBToggleButton : public QWidget {
	Q_OBJECT

	public:
		NBToggleButton( QWidget *parent = NULL );

		bool isChecked();
		void setChecked( bool );

		void setShortcut( QString );

	private:
		bool checked;
		QAction *shortcut;

	public slots:
		void toggle();

	protected:
		void mousePressEvent( QMouseEvent* );
		void paintEvent( QPaintEvent* );

	Q_SIGNALS:
		void clicked();
};

class NBAddressEdit : public QLineEdit {
	Q_OBJECT

	public:
		NBAddressEdit( QWidget* );
};

class NBAddressWidget : public QWidget {
	Q_OBJECT

	public:
		NBAddressWidget( QWidget *parent = NULL );
		void setShowHidden( bool );

		NBAddressEdit *addressEdit;
		NBCrumbsBar *crumbsBar;

	public slots:
		void setFocus();

	private :
		void setWidgetProperties();

		NBToggleButton *toggleBtn;
		QDirModel *dModel;

	private slots:
		void toggleCrumbsBarAndEdit();
		void revertToCrumbsBar();

	Q_SIGNALS:
		void openLocation( QString );
};

class NBAddressBar : public QWidget {
	Q_OBJECT

	public:
		NBAddressBar( QWidget *parent = NULL );

		QString address();
		void setAddress( QString );

		void updateNavigationButtons( bool, bool );

	private:
		NBButton *backBtn, *forwardBtn, *homeBtn, *ssBtn;
		NBButton *filterBtn;
		NBAddressWidget *addressWidget;
		NBProcessManagerMini *mProcWidget;
		NBMenuButton *menuBtn;

	public slots:
		void focusAddressEdit();

		void showSearchButton();
		void hideSearchButton();

	Q_SIGNALS:
		void openLocation( QString );
		void openSearch();

		void goBack();
		void goHome();
		void openSuperStart();
		void goForward();

		void newWindow();

		void zoomIn();
		void zoomOut();

		void cut();
		void copy();
		void paste();

		void openVTE();

		void changeViewMode( int );

		void sortByName();
		void sortByType();
		void sortBySize();
		void sortByDate();
		void toggleHidden();
		void toggleGrouping();

		void showSettings();

		void closeWindow();
		void quit();
};
