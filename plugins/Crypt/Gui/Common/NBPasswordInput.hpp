/*
	*
	* PasswordInput.hpp - PasswordInput class header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBPatternPad.hpp>

class PasswordInput : public QDialog {
	Q_OBJECT

	public:
		PasswordInput( QWidget *parent = NULL );

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

	Q_SIGNALS:
		void closed();
};
