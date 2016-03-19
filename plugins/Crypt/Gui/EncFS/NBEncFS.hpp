/*
	*
	* NBDerypt.hpp - NewBreeze File Folder Decryption Class Header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBPasswordInput.hpp>
#include <NBNewEncFS.hpp>
#include <NBTools.hpp>

class NBEncFS : public QObject {
	Q_OBJECT

	public:
		/* Init */
		NBEncFS( QString source, QString target = QString(), QWidget *parent = NULL );

	public Q_SLOTS:
		void mountDir();
		void unmountDir();
		void createEncFS();

	private:
		QString mSource;
		QString mTarget;

		QWidget *mParent;
};
