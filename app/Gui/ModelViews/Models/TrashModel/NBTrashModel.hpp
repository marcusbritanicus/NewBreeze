/*
	*
	* NBTrashModel.cpp - NewBreeze NBTrashModel Class
	*

	*
	* This file was originally written by me for CoreFM.
	* It is being reused here with suitable modifications.
	*
*/

#pragma once

#include "Global.hpp"

class NBTrashNode {

	public:
		NBTrashNode( QString path, QString trashPath, QString size, QString date );
		QVariant data( int, int );

	private:
		QString mName;
		QString mPath;
		QDateTime mDate;
		QString mSize;

		QString mTrashPath;

		QIcon mIcon;
};

typedef QList<NBTrashNode *> TrashNodes;

class NBTrashModel : public QAbstractItemModel {
    Q_OBJECT

	public:
		NBTrashModel();
		~NBTrashModel();

		int rowCount( const QModelIndex &parent = QModelIndex() ) const;
		int columnCount( const QModelIndex &parent = QModelIndex() ) const;

		QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
		QVariant headerData( int column, Qt::Orientation, int role ) const;

		QModelIndex index( int row, int column, const QModelIndex &parent ) const;
		QModelIndex parent( const QModelIndex & ) const;

		Qt::ItemFlags flags( const QModelIndex &index ) const;

		void reload();

	private:
		void setupModelData();

		TrashNodes nodes;

	public Q_SLOTS:
		void restore( QStringList );
		void removeFromDisk( QStringList );

	Q_SIGNALS:
		void restored( QStringList );
		void deleted( QStringList );
};
