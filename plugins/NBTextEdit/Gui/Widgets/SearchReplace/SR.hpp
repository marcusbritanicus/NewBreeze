/*
	*
	* SR.hpp - Wordy's SearchReplace Class Header
	*
*/

#pragma once

#include <Global.hpp>
#include <ButtonSet.hpp>
#include <GuiWidgets.hpp>

class SearchWidget : public QWidget {
	Q_OBJECT

	public:
		SearchWidget( QWidget *parent );

	private:
		void setupUI();

		QLineEdit *findLE;
		QLabel *findLbl;

		QToolButton *findNextBtn, *findPrevBtn, *settingsBtn;
		QCheckBox *useRegexCB, *caseSensitiveCB, *wholeWordsCB;
		QGroupBox *optsGB;

	private slots:
		void updateUI();
		void toggleOpts( bool );

		void autoFind();
		void findNextClicked();
		void findPrevClicked();

	public slots:
		void setFocus();
		void show();
		void hide();

	protected:
		void keyPressEvent( QKeyEvent* );

	Q_SIGNALS:
		void autofind( QString, bool, bool, bool );
		void find( QString, bool, bool, bool, bool );
		void shown();
		void hidden();
};

class ReplaceWidget : public QWidget {
	Q_OBJECT

	public:
		ReplaceWidget( QWidget *parent );

	Q_SIGNALS:
		void replace( QString, QString, bool, bool, bool, bool );
		void replaceAndFind( QString, QString, bool, bool, bool, bool );
		void replaceInDoc( QString, QString, bool, bool, bool );
		void replaceInSel( QString, QString, bool, bool, bool, bool );
};
