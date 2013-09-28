/*
	*
	* NBAddressBar.hpp - NBAddressBar.cpp Header
	*
*/

#ifndef NBADDRESSBAR_HPP
#define NBADDRESSBAR_HPP

#include <Global.hpp>
#include <NBGuiWidgets.hpp>
#include <NBBreadCrumbsBar.hpp>
#include <NBTools.hpp>

class NBAddressWidget : public QWidget {
	Q_OBJECT

	public:
		NBAddressWidget();
		void setShowHidden( bool );

		QLineEdit *addressEdit;
		NBBreadCrumbsBar *crumbsBar;

	private :
		void setWidgetProperties();

		NBToolButton *toggleBtn;
		QDirModel *dModel;

	private slots:
		void toggleCrumbsBarAndEdit();
};

class NBAddressBar : public QFrame {
	Q_OBJECT

	public:
		NBAddressBar();

		NBToolButton *reloadBtn, *openVTEBtn;
		NBViewModeButton *viewModeBtn;
		NBAddressWidget *addressWidget;
};

#endif
