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

		int checkedAction();

	private:
		NBButton *backBtn, *forwardBtn, *homeBtn, *ssBtn;
		NBButton *settingsBtn, *filterBtn;
		NBSegmentButton *viewModes;
		NBAddressWidget *addressWidget;
		NBProcessManagerMini *mProcWidget;

	public slots:
		void focusAddressEdit();

		void showSearchButton();
		void hideSearchButton();

		void updateViewMode( QString );

	Q_SIGNALS:
		void openLocation( QString );
		void openSearch();
		void changeViewMode( int );

		void goBack();
		void goHome();
		void openSuperStart();
		void goForward();
};
