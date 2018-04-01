/*
	*
	* NBSystemMenu.hpp - NewBreeze Menu Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBBugReporter.hpp"

class NBMenuItem : public QLabel {
	Q_OBJECT

	public:
		NBMenuItem( QString name, QString icon, QWidget *parent );

		QString name();
		QIcon icon();

		void setCheckable( bool );

		inline bool isChecked() {
			return mChecked;
		};

		inline void setChecked( bool check ) {
			mChecked = check;
			repaint();
		};

	private:
		QString mName;
		QString mIcon;

		bool mPressed;
		bool mHover;

		bool mChecked;
		bool mCheckable;

	protected:
		void enterEvent( QEvent* );
		void leaveEvent( QEvent* );

		void mouseReleaseEvent( QMouseEvent *mrEvent );
		void mousePressEvent( QMouseEvent *mpEvent );
		void paintEvent( QPaintEvent *pEvent );

	Q_SIGNALS:
		void clicked();
};

class NBMenuItemGroup : public QObject {
	Q_OBJECT

	public:
		NBMenuItemGroup();
		void addMenuItem( NBMenuItem *itm );

	private:
		QList<NBMenuItem*>mItems;
		int mCurrent;

	private Q_SLOTS:
		void makeExclusive();
};

class NBMenuItemButton : public QToolButton {
	Q_OBJECT

	public:
		NBMenuItemButton( QString icon, QWidget *parent );
};

class NBMenuItemCheck : public QCheckBox {
	Q_OBJECT

	public:
		NBMenuItemCheck( QString text, QWidget *parent );
};

class NBSystemMenu : public QWidget {
	Q_OBJECT

	public:
		NBSystemMenu( QWidget *parent );

		inline void updateAddress( QString addr ) {

			mAddress = addr;
		};

		void populateMenu();

	public Q_SLOTS:
		void exec( QPoint );
		void viewModesClicked();

		void reportBug();
		void updateZoomIn();
		void updateZoomOut();

	private:
		QVBoxLayout *itemsLayout;

		NBMenuItemButton *iconsBtn, *tilesBtn, *detailsBtn;
		NBMenuItem *nSortBtn, *tSortBtn, *sSortBtn, *dSortBtn;
		NBMenuItemCheck *groupCheck;

		QLabel *zoomLbl;

		QString mAddress;

	protected:
		void closeEvent( QCloseEvent *cEvent );

	Q_SIGNALS:
		void clicked();
		void closed();

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
		void toggleGrouping();

		void showSettings();

		void closeWindow();
		void quit();
};
