/*
	*
	* NBArchiveDialog.hpp - NBArchiveDialog.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDialog.hpp"
#include "NBFileDialog.hpp"
#include "NBButtons.hpp"

class NBArchiveDialog : public NBDialog {
	Q_OBJECT

	public:
		static QString newArchive( QWidget * );

	private:
		NBArchiveDialog( QWidget* );
		void createGUI();

		QString filename, location, format;
		QLineEdit *locationLE;
		QString archive();

	private Q_SLOTS:
		void updateFileName( QString );
		void updateDirName();
		void updateFormat( QString );
};
