/*
	*
	* NBStartUpWizard.cpp - Startup wizard header
	*
*/

#include "Global.hpp"

class NBStartupWizard : public QWizard {
	Q_OBJECT

	public:
		NBStartupWizard( QWidget *parent = 0 );

	private:
		void createGui();
		void setupConnections();
		void createPages();
		void setWindowProperties();

	protected:
		void paintEvent( QPaintEvent *pEvent );
};
