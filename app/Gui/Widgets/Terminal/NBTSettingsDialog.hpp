/*
    *
    * * NBTSettingsDialog.cpp - QTermWidget reimplementation for NBTerminal
    *
*/

#ifndef NBTSETTINGSDIALOG_HPP
#define NBTSETTINGSDIALOG_HPP

#include <Global.hpp>

class NBTSettingsDialog : public QDialog {
	Q_OBJECT

	public:
		NBTSettingsDialog();

		QLineEdit *shellEdit;
		QComboBox *colorSchemesCombo;
		QFontComboBox *fontCombo;
		QSpinBox *fontSizeSpin;

	private:
		void createGUI();

	private slots:
		void saveShell( QString );
		void setColorScheme();
		void setFont( QFont font );
		void setFont( int size );
};

#endif
