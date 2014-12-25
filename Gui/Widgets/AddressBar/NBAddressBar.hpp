/*
	*
	* NBAddressBar.hpp - NBAddressBar.cpp Header
	*
*/

#pragma once
#ifndef NBADDRESSBAR_HPP
#define NBADDRESSBAR_HPP

#include <Global.hpp>
#include <NBGuiWidgets.hpp>
#include <NBBreadCrumbsBar.hpp>
#include <NBTools.hpp>
#include <NBButtons.hpp>

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

class NBAddressWidget : public QWidget {
	Q_OBJECT

	public:
		NBAddressWidget( QWidget *parent = NULL );
		void setShowHidden( bool );

		QLineEdit *addressEdit;
		NBBreadCrumbsBar *crumbsBar;

	private :
		void setWidgetProperties();

		NBToggleButton *toggleBtn;
		QDirModel *dModel;

	private slots:
		void toggleCrumbsBarAndEdit();
};

class NBAddressBar : public QFrame {
	Q_OBJECT

	public:
		NBAddressBar( QWidget *parent = NULL );

		NBButtons *addressButtons;
		QPushButton *reloadBtn, *openVTEBtn;
		NBViewModeButton *viewModeBtn;
		NBAddressWidget *addressWidget;
		NBSearchBar *searchBar;
};

#endif
