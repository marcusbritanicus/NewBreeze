/*
	*
	* NBTreeBranch.hpp - NewBreeze NBTreeBranch Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "newbreeze.hpp"

class NBTreeBranch : public QObject {
	Q_OBJECT

	public:
		NBTreeBranch();
		NBTreeBranch( QString, QIcon, NBTreeBranch *parent = 0 );

		int branchCount();

		void clearBranches();

		void addBranch( NBTreeBranch* );
		void removeBranch( NBTreeBranch* );

		NBTreeBranch* branch( int );
		NBTreeBranch* branch( QString );
		QList<NBTreeBranch*> branches();

		QVariant data( int role ) const;
		bool setData( int column, QVariant data );

		/* Filters */
		QStringList nameFilters();
		void setNameFilters( QStringList );
		void clearNameFilters();

		bool showHidden() const;
		void setShowHidden( bool );

		NBTreeBranch *parent();
		int row();

		void sort();

		QString name();
		QIcon icon();

	private:
		QList<NBTreeBranch*> mBranches;
		NBTreeBranch *parentNode;

		QString mPath;
		QIcon mIcon;

		QStringList __nameFilters;
		bool __showHidden;
};

bool caseInsensitiveNameSort( NBTreeBranch *first, NBTreeBranch *second );
bool matchesFilter( QStringList filters, QString text );
