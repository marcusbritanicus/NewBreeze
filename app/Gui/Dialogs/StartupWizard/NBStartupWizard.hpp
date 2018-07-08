/*
	*
	* NBStartupWizard.cpp - Startup wizard header
	*
*/

#include "Global.hpp"
#include "NBSGeneralWidget.hpp"

class NBStartupWizard : public QWizard {
	Q_OBJECT

	public:
		NBStartupWizard();

	private:
		void createGui();
		void createPages();
		void setupConnections();
		void setWindowProperties();

		void loadTerminals();

		QGroupBox *p2gb1;
		QCommandLinkButton *p2btn1, *p2btn2, *p2btn3, *p2btn4;
		QButtonGroup *p2btnGrp1;
		QCheckBox *p2cb1;

		QComboBox *p3cb1;
		QSlider *p3sl1;
		QGroupBox *p3gb1;

		QCheckBox *p4cb1;
		QGroupBox *p4gb1;

		QCheckBox *p5cb1;
		QGroupBox *p5gb1;

		QCheckBox *p6cb1;
		QGroupBox *p6gb1;

		QGroupBox *p7gb1;
		QCommandLinkButton *p7btn1, *p7btn2;
		QButtonGroup *p7btnGrp1;

		QGroupBox *p8gb1;
		QCommandLinkButton *p8btn1, *p8btn2;
		QButtonGroup *p8btnGrp1;

		QGroupBox *p9gb1;
		QCommandLinkButton *p9btn1, *p9btn2;
		QButtonGroup *p9btnGrp1;

		QGroupBox *p10gb1;
		QCheckBox *p10cb1;

		QGroupBox *p11gb1;
		QListWidget *p11lw1;
		QToolButton *p11btn1;

		QGroupBox *p12gb1;
		QCheckBox *p12cb1;
		QCheckBox *p12cb2;

	protected:
		void paintEvent( QPaintEvent *pEvent );

	private Q_SLOTS:
		void saveSettings();
		void disableCancelOnLastPage( int );

		void addNewTerminal();
};
