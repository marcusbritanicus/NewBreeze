/*
	*
	* NBPasswordDialog.hpp - New EncFS Volume Class header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBPasswordEdit.hpp>
#include <NBPasswordInput.hpp>
#include <NBTools.hpp>

class NBPasswordDialog : public QDialog {
	Q_OBJECT

	public:
		NBPasswordDialog( bool changePassword = false, QWidget *parent = NULL );

		/* Password */
		QString password();

		/* Password */
		QString oldPassword();

		/* Clear the passwords */
		void clear();

	private:
		void createGUI();
		void setWindowProperties();

		NBPasswordEdit *oldPasswdLE;

		NBPasswordEdit *passwd1LE;
		NBPasswordEdit *passwd2LE;

		QPushButton *okBtn;

		bool mChangePassword;

	public Q_SLOTS:
		/* Set the password validated */
		void setValidated( bool );

	private Q_SLOTS:
		/* Check if the old password is correct */
		void validateOldPassword();

		/* Check if the new passwords match */
		void validatePassword();

	Q_SIGNALS:
		void oldPassword( QString );
};
