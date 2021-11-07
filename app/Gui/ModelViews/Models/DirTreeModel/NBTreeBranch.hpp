/*
	*
	* NBTreeBranch.hpp - NewBreeze NBTreeBranch Class Header
	*
*/

#pragma once

#include <QtCore>
#include <QtGui>

#include "newbreeze.hpp"

class NBTreeBranch : public QObject {
	Q_OBJECT

	public:
		NBTreeBranch();
		NBTreeBranch( QString, QIcon, NBTreeBranch *parent = 0 );

		int childCount();

		void clearChildren();

		void addChild( NBTreeBranch* );
		void removeChild( NBTreeBranch* );

		NBTreeBranch* child( int );
		NBTreeBranch* child( QString );
		QList<NBTreeBranch*> children();

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
		QList<NBTreeBranch*> childNodes;
		NBTreeBranch *parentNode;

		QString mPath;
		QIcon mIcon;

		QStringList __nameFilters;
		bool __showHidden;

		QBasicTimer pollTimer;

	public Q_SLOTS:
		void explore();
};

bool caseInsensitiveNameSort( NBTreeBranch *first, NBTreeBranch *second );
