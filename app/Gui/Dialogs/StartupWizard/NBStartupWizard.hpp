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
		void createPages();
		void setupConnections();
		void setWindowProperties();

		QGroupBox *p2gb1;
		QCommandLinkButton *p2btn1, *p2btn2, *p2btn3, *p2btn4;
		QButtonGroup *p2btnGrp1;

		QComboBox *p3cb1;
		QSlider *p3sl1;
		QGroupBox *p3gb1;

		QCheckBox *p4cb1;
		QGroupBox *p4gb1;

		QCheckBox *p5cb1;
		QGroupBox *p5gb1;

		QCheckBox *p6cb1;
		QGroupBox *p6gb1;

	protected:
		void paintEvent( QPaintEvent *pEvent );

	private Q_SLOTS:
		void saveSettings();
};
