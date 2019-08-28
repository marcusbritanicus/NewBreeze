/*
	*
	* NBTrashModel.hpp - NewBreeze NBTrashModel Class Header
	*

	*
	* This file was originally written by me for CoreFM.
	* It is being reused here with suitable modifications.
	*
*/

#include "NBTrashModel.hpp"

NBTrashNode::NBTrashNode( QString path, QString trashPath, QString size, QString dateTime ) {

	mName = baseName( path );
	mPath = QString( path );
	mSize = QString( size );

	mIcon = QIcon::fromTheme( mimeDb.mimeTypeForFile( trashPath ).iconName() );
	mTrashPath = QString( trashPath );

	mDate = QDateTime::fromString( dateTime, "yyyyMMddTHH:mm:ss" );
	if ( mDate.isNull() )
		mDate = QDateTime::fromString( dateTime, "yyyy-MM-ddTHH:mm:ss" );
};

QVariant NBTrashNode::data( int role, int column ) {

	switch( role ) {
		case Qt::DisplayRole: {

			if ( column == 0 )
				return mName;

			else if ( column == 1 )
				return mPath;

			else if ( column == 2 )
				return mSize;

			else if ( column == 3 )
				return mDate;

			else
				return QVariant();
		}

		case Qt::DecorationRole: {

			return mIcon;
		}

		case Qt::UserRole + 1: {

			return mTrashPath;
		}

		default:
			return QVariant();
	}
};

NBTrashModel::NBTrashModel() : QAbstractItemModel() {

	setupModelData();
};

NBTrashModel::~NBTrashModel() {

	nodes.clear();
};

int NBTrashModel::rowCount( const QModelIndex & ) const {

	return nodes.count();
};

int NBTrashModel::columnCount( const QModelIndex & ) const {

	return 3;
};

Qt::ItemFlags NBTrashModel::flags( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
};

QVariant NBTrashModel::data( const QModelIndex &index, int role ) const {

	if ( not index.isValid() )
		return QVariant();

	NBTrashNode *node = static_cast<NBTrashNode*>( index.internalPointer() );
	switch( role ) {

		case Qt::DisplayRole: {
			switch( index.column() ) {
				case 0:
					return node->data( role, 0 );

				case 1:
					return node->data( role, 1 );

				case 2:
					return formatSize( node->data( role, 2 ).toLongLong() );

				case 3:
					return node->data( role, 2 ).value<QDateTime>().toString( "MMM dd, yyyy hh:mm:ss" );
			}

			return QString();
		}

		case Qt::DecorationRole: {
			if ( index.column() == 0 )
				return node->data( role, 0 );

			else
				return QVariant();
		}

		case Qt::TextAlignmentRole: {
			if ( index.column() == 0 )
				return ( 0x0001 | 0x0080 );

			else if ( index.column() == 1 )
				return ( 0x0001 | 0x0080 );

			else
				return Qt::AlignCenter;
		}

		case Qt::InitialSortOrderRole: {

			return Qt::AscendingOrder;
		}

		case Qt::UserRole + 1: {

			return node->data( Qt::UserRole + 1, 0 );
		}

		default: {

			return QVariant();
		}
	}
};

QVariant NBTrashModel::headerData( int section, Qt::Orientation orientation, int role ) const {

	QStringList headerList = QStringList() << "Name" << "Original Path" << "Deleteion Date";

	if ( ( orientation == Qt::Horizontal ) and ( role == Qt::DisplayRole ) )
		return headerList.at( section );

	else
		return QVariant();
};

QModelIndex NBTrashModel::index( int row, int column, const QModelIndex & ) const {

	if ( ( row >= 0 ) and ( row < nodes.count() ) )
		return createIndex( row, column, nodes.at( row ) );

	else
		return QModelIndex();
};

QModelIndex NBTrashModel::parent( const QModelIndex & ) const {

	return QModelIndex();
};

void NBTrashModel::reload() {

	setupModelData();
};

void NBTrashModel::setupModelData() {

	nodes.clear();

	beginResetModel();

	/* Loading home trash */
	QString trashLoc = NBXdg::homeTrashLocation();
	QDir trashDir = QDir( trashLoc + "/files/" );
	trashDir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
	Q_FOREACH( QFileInfo info, trashDir.entryInfoList() ) {
		QString trashPath = info.filePath();
		QString infoPath = trashLoc + "/info/" + info.fileName() + ".trashinfo";

		QSettings trashInfo( infoPath, QSettings::NativeFormat );
		QString origPath = trashInfo.value( "Trash Info/Path" ).toString();
		QString dateTime = trashInfo.value( "Trash Info/DeletionDate" ).toString();

		QString size = ( isDir( trashPath ) ? QString( "%1 items" ).arg( nChildren( trashPath ) ) : formatSize( getSize( trashPath ) ) );

		nodes << new NBTrashNode( origPath, trashPath, size, dateTime );
	}

	/* Loading all trash */
	Q_FOREACH( QStorageInfo devInfo, QStorageInfo::mountedVolumes() ) {
		/* We don't want to check the home directory for trash once more */
		if ( devInfo.rootPath() == QDir::homePath() )
			continue;

		QString trashLoc = NBXdg::trashLocation( devInfo.rootPath() );
		if ( NBXdg::homeTrashLocation() == trashLoc )
			continue;

		if ( not trashLoc.isEmpty() ) {
			QDir trash( trashLoc + "/info/" );
			trash.setNameFilters( QStringList() << "*.trashinfo" );
			Q_FOREACH( QString entry, trash.entryList() ) {

				QSettings trashInfo( trash.absoluteFilePath( entry ), QSettings::NativeFormat );

				QString origPath = trashInfo.value( "Trash Info/Path" ).toString();
				QString delDate = trashInfo.value( "Trash Info/DeletionDate" ).toString();
				QString trashPath = QString( trashInfo.fileName() ).replace( "/info/", "/files/" ).replace( ".trashinfo", "" );

				QString size = ( isDir( trashPath ) ? QString( "%1 items" ).arg( nChildren( trashPath ) ) : formatSize( getSize( trashPath ) ) );

				nodes << new NBTrashNode( origPath, trashPath, size, delDate );
			}
		}
	}

	endResetModel();
};

void NBTrashModel::restore( QStringList toBeRestored ) {

	QStringList failed;

	Q_FOREACH( QString fileToRestore, toBeRestored ) {
		QDir trash = QDir( NBXdg::trashLocation( fileToRestore ) );

		// Read trashinfo
		QSettings trashInfo( trash.path() + "/info/" + baseName( fileToRestore ) + ".trashinfo", QSettings::IniFormat );
		if ( trashInfo.contains( "Trash Info/Path" ) ) {
			QString origPath = trashInfo.value( "Trash Info/Path" ).toString();

			if ( exists( origPath ) ) {
				int i = 1;

				QString origLoc = dirName( origPath );
				QString origName = baseName( origPath );

				/* A hundred files having the same name? */
				while ( i < 100 ) {
					origPath = origLoc + QString( "(%1) " ).arg( i ) + origName;
					if ( exists( origPath ) )
						i++;

					else
						break;
				}

				/* A hundred files having the same name? */
				if ( i >= 100 ) {
					failed << fileToRestore;
					continue;
				}
			}

			/* Failed to move it to original path */
			if ( not QFile::rename( fileToRestore, origPath ) )
				failed << fileToRestore;

			/* Success! Remove the info file */
			else
				QFile::remove( trashInfo.fileName() );
		}

		else {

			qDebug() << "Problematic info file:" << trashInfo.fileName();
			failed << fileToRestore;
		}
	}

	emit restored( failed );
	setupModelData();
};

void NBTrashModel::removeFromDisk( QStringList toBeDeleted ) {

	QStringList failed;

	Q_FOREACH( QString fileToDelete, toBeDeleted ) {
		QDir trash = QDir( NBXdg::trashLocation( fileToDelete ) );

		// Read trashinfo
		QString trashInfoPath( trash.path() + "/info/" + baseName( fileToDelete ) + ".trashinfo" );
		if ( isDir( fileToDelete ) ) {
			if ( not removeDir( fileToDelete ) )
				failed << fileToDelete;

			else
				QFile::remove( trashInfoPath );
		}

		else if ( isFile( fileToDelete ) ) {
			if ( not QFile::remove( fileToDelete ) )
				failed << fileToDelete;

			else
				QFile::remove( trashInfoPath );
		}

		else {
			qDebug() << "Cannot remove" << fileToDelete << "from trash";
			failed << fileToDelete;
		}
	}

	qDebug() << failed;

	emit deleted( failed );
	setupModelData();
};
