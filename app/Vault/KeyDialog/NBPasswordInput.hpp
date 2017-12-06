/*
	*
	* NBPasswordInput.hpp - NBPasswordInput class header
	*
*/

#pragma once

#ifdef STANDALONE
	#include "Global2.hpp"
#else
	#include "Global.hpp"
#endif
#include "NBPatternPad.hpp"

class NBPasswordInput : public QDialog {
	Q_OBJECT

	public:
		NBPasswordInput( QWidget *parent = NULL );

		/* Password */
		QString password();

		/* Clear the password */
		void clear();

	private:
		void createGUI();
		void setWindowProperties();

		PasswordKeypad *pKeyPad;
		QLineEdit *passwdLE;

		QString mPassword;
		bool mOkay;

	private Q_SLOTS:
		void setPassword();
};
