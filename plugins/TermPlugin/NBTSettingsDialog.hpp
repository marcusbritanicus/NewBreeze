/*
    *
    * * NBTSettingsDialog.cpp - QTermWidget reimplementation for NBTerminal
    *
*/

#ifndef NBTSETTINGSDIALOG_HPP
#define NBTSETTINGSDIALOG_HPP

#include "Global.hpp"

class NBTSettingsDialog : public QDialog {
	Q_OBJECT

	public:
		NBTSettingsDialog();

		QComboBox *colorSchemesCombo;
		QDoubleSpinBox *transparencySpin;
		QFontComboBox *fontCombo;
		QSpinBox *fontSizeSpin;
		QCheckBox *enableTransparencyCheck;
		QCheckBox *borderlessCheck;

	private:
		void createGUI();

	private slots:
		void setColorScheme();
		void setFont( QFont font );
		void setFont( int size );
		void setEnableTransparency();
		void setTransparency();
		void setBorderless();
};

#endif
