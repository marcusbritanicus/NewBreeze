/*
	*
	* NBPasswordDialog.hpp - New EncFS Volume Class header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBPasswordEdit.hpp"
#include "NBPasswordInput.hpp"
#include "NBTools.hpp"

class NBPasswordDialog : public QDialog {
	Q_OBJECT

	public:
		NBPasswordDialog( QWidget *parent = NULL );

		/* Password */
		QByteArray password();

		/* Clear the passwords */
		void clear();

	private:
		void createGUI();
		void setWindowProperties();

		NBPasswordEdit *passwd1LE;
		NBPasswordEdit *passwd2LE;

		QPushButton *okBtn;

	private Q_SLOTS:
		/* Check if the new passwords match */
		void validatePassword();
};
