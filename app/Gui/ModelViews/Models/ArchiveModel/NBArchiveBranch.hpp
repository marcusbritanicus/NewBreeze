/*
	*
	* NBArchiveBranch.hpp - NewBreeze NBArchiveBranch Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "newbreeze.hpp"

class NBArchiveBranch : public QObject {
	Q_OBJECT

	public:
		NBArchiveBranch();
		NBArchiveBranch( QString, QIcon, NBArchiveBranch *parent = 0 );

		int branchCount();

		void clearBranches();

		void addBranch( NBArchiveBranch* );
		void removeBranch( NBArchiveBranch* );

		NBArchiveBranch* branch( int );
		NBArchiveBranch* branch( QString );
		QList<NBArchiveBranch*> branches();

		QVariant data( int role ) const;
		bool setData( int column, QVariant data );

		/* Filters */
		QStringList nameFilters();
		void setNameFilters( QStringList );
		void clearNameFilters();

		bool showHidden() const;
		void setShowHidden( bool );

		NBArchiveBranch *parent();
		int row();

		void sort();

		QString name();
		QIcon icon();

	private:
		QList<NBArchiveBranch*> mBranches;
		NBArchiveBranch *parentNode;

		QString mPath;
		QIcon mIcon;

		QStringList __nameFilters;
		bool __showHidden;
};

bool caseInsensitiveNameSort( NBArchiveBranch *first, NBArchiveBranch *second );
