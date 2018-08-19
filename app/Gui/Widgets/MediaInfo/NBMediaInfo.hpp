/*
	*
	* Copyright 2018 Britanicus <marcusbritanicus@gmail.com>
	*
*/

#pragma once

#include "Global.hpp"

#ifdef USE_MEDIAINFO

#include <MediaInfo/MediaInfo.h>
#include <ZenLib/Ztring.h>
#include <ZenLib/ZtringListList.h>

using namespace MediaInfoLib;
using namespace ZenLib;

/* From char* to ZString */
#define ZString( _DATA )		ZenLib::Ztring().From_UTF8( _DATA )

/* From QString to ZString */
#define FromQString( _DATA )	ZenLib::Ztring().From_UTF8( _DATA.toUtf8().constData() )

/* From ZString to QString */
#define FromZString( _DATA )	QString::fromUtf8( ZenLib::Ztring( _DATA ).To_UTF8().c_str() )

class NBMediaInfoNode;

class NBMediaInfoModel : public QAbstractItemModel {
	Q_OBJECT

	public:
		NBMediaInfoModel( QString fileName = QString() );

		QString quickInfo() const;

		int rowCount( const QModelIndex &parent = QModelIndex() ) const;
		int columnCount( const QModelIndex &parent = QModelIndex() ) const;

		QVariant data( const QModelIndex &idx, int role = Qt::DisplayRole ) const;

		QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex parent( const QModelIndex &index = QModelIndex() ) const;

	private:
		void setupModel();

		bool loaded;
		QString mFileName;

		QString mQuickInfo;

		NBMediaInfoNode *rootNode;
};

class NBMediaInfoWidget : public QTreeView {
	Q_OBJECT

	public:
		NBMediaInfoWidget( QString fileName, QWidget *parent = 0 );
		void setFileName( QString fileName );
};

#endif
