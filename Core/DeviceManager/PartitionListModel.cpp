/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#include "QDrive.h"
#include "PartitionListModel.h"
#include "Partition.h"
#include "Drive.h"
//#include "Version.h"
using namespace QDrive;
//#include <QtQml>
#include <QtCore>

PartitionListModel::PartitionListModel(QObject *parent)
	: QAbstractTableModel(parent), m_replyTimeout(100),
	m_mountedCount(0), m_mountedCountDirty(false)
{
	m_roleNames[PartitionPath]					= "PartitionPath";
	m_roleNames[PartitionDevice] 				= "PartitionDevice";
	m_roleNames[PartitionID]					= "PartitionID";
	m_roleNames[PartitionSize] 				= "PartitionSize";
	m_roleNames[PartitionReadOnly]			= "PartitionReadOnly";
	m_roleNames[PartitionLabel] 				= "PartitionLabel";
	m_roleNames[PartitionUUID]					= "PartitionUUID";
	m_roleNames[PartitionNumber] 				= "PartitionNumber";
	m_roleNames[PartitionNumberAndLabel]	= "PartitionNumberAndLabel";
	m_roleNames[PartitionOffset]				= "PartitionOffset";
	m_roleNames[PartitionType]					= "PartitionType";
	m_roleNames[PartitionMounted] 			= "PartitionMounted";
	m_roleNames[PartitionMountPoints] 		= "PartitionMountPoints";
	m_roleNames[PartitionAvailableSpace]	= "PartitionAvailableSpace";

	m_roleNames[DrivePath]						= "DrivePath";
	m_roleNames[DriveVendor]					= "DriveVendor";
	m_roleNames[DriveModel]						= "DriveModel";
	m_roleNames[DriveSerial]					= "DriveSerial";
	m_roleNames[DriveID]							= "DriveID";
	m_roleNames[DriveMediaRemovable]			= "DriveMediaRemovable";
	m_roleNames[DriveMediaAvailable]			= "DriveMediaAvailable";
	m_roleNames[DriveSize]						= "DriveSize";
	m_roleNames[DriveTimeDetected]			= "DriveTimeDetected";
	m_roleNames[DriveMediaTimeDetected]		= "DriveMediaTimeDetected";
	m_roleNames[DriveUSB] 						= "DriveUSB";
	m_roleNames[DriveRemovable] 				= "DriveRemovable";

#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
	connect(&m_monitor, &Monitor::partitionAdded, this,
			&PartitionListModel::addPartition);
	connect(&m_monitor, &Monitor::partitionRemoved, this,
			&PartitionListModel::removePartition);
#else
	connect(&m_monitor, SIGNAL(partitionAdded(const QDBusObjectPath &)), this,
			SLOT(addPartition(const QDBusObjectPath &)));
	connect(&m_monitor, SIGNAL(partitionRemoved(const QDBusObjectPath &)), this,
			SLOT(removePartition(const QDBusObjectPath &)));
#endif
}

int PartitionListModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_partitions.size();
}

int PartitionListModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_roleNames.size();
}

QVariant PartitionListModel::data(const QModelIndex &index, int role) const
{
	if (!validIndex(index, role))
		return QVariant();

	Partition *partition = m_partitions.at(index.row());
	if (!partition->isValid())
		return QVariant();

	try
	{
		Drive drive(partition->drivePath());
		drive.setReplyTimeout(m_replyTimeout);

		switch (role)
		{
			case PartitionPath:
				return partition->path().path();
			case PartitionDevice:
				return partition->device();
			case PartitionID:
				return partition->id();
			case PartitionSize:
				return partition->size();
			case PartitionReadOnly:
				return partition->isReadOnly();
			case PartitionLabel:
				return partition->label();
			case PartitionUUID:
				return partition->uuid();
			case PartitionNumber:
				return partition->partitionNumber();
			case PartitionNumberAndLabel:
				return QString("%1: %2").arg(partition->partitionNumber()).arg(
						partition->label());
			case PartitionOffset:
				return partition->offset();
			case PartitionType:
				return partition->type();
			case PartitionMounted:
				return partition->isMounted();
			case PartitionMountPoints:
				return partition->mountPoints();
			case PartitionAvailableSpace:
				if (partition->isMounted())
					return partition->availableSpace();
				else
					return QVariant();

			case DrivePath:
				return drive.path().path();
			case DriveVendor:
				return drive.vendor();
			case DriveModel:
				return drive.model();
			case DriveSerial:
				return drive.serial();
			case DriveID:
				return drive.id();
			case DriveMediaRemovable:
				return drive.isMediaRemovable();
			case DriveMediaAvailable:
				return drive.isMediaAvailable();
			case DriveSize:
				return drive.size();
			case DriveTimeDetected:
				return drive.timeDetected();
			case DriveMediaTimeDetected:
				return drive.timeMediaDetected();
			case DriveUSB:
				return drive.isUSB();
			case DriveRemovable:
				return drive.isRemovable();
		}
	}
	catch (const DBusException &exception)
	{
		emit dBusError(exception.what());
		return QVariant();
	}

	return QVariant();
}

QVariant PartitionListModel::data(const QString &path, int role) const
{
	for (int row = 0; row < m_partitions.size(); ++row)
		if (m_partitions.at(row)->path().path() == path)
			return data(row, role);

	return QVariant();
}

QHash<int, QByteArray> PartitionListModel::roleNames() const
{
	return m_roleNames;
}

bool PartitionListModel::validIndex(const QModelIndex &index, int role) const
{
	int row = index.row();

	return !m_partitions.isEmpty() &&
		row >= 0 && row < rowCount() &&
		m_roleNames.contains(role);
}

bool PartitionListModel::validIndex(int row, int column) const
{
	return !m_partitions.isEmpty() &&
		row >= 0 && row < rowCount() &&
		column >= 0 && column < columnCount();
}

QVariant PartitionListModel::data(int row, int role) const
{
	/* Column is not used: */
	return data(index(row, 0), role);
}

QVariantMap PartitionListModel::drivePropertyFilter() const
{
	return m_drivePropertyFilter;
}

void PartitionListModel::setDrivePropertyFilter(const QVariantMap
		&propertyFilter)
{
	if (propertyFilter == m_drivePropertyFilter)
		return;

	m_drivePropertyFilter = propertyFilter;
	enumerate();
}

int PartitionListModel::replyTimeout() const
{
	return m_replyTimeout;
}

void PartitionListModel::setReplyTimeout(int replyTimeout)
{
	m_replyTimeout = replyTimeout;
}

int PartitionListModel::mountedPartitionCount() const
{
	if (!m_mountedCountDirty)
		return m_mountedCount;

	int mountedCount = 0;
	try
	{
		foreach (Partition *partition, m_partitions)
			if (partition->isMounted())
				++mountedCount;
	}
	catch (const DBusException &exception)
	{
		m_mountedCount = -1;
		m_mountedCountDirty = true;
		emit dBusError(exception.what());

		return -1;
	}

	m_mountedCount = mountedCount;
	m_mountedCountDirty = false;

	return m_mountedCount;
}

void PartitionListModel::populateList()
{
	try
	{
		QList<QDBusObjectPath> partitionPaths = QDrive::partitions(m_replyTimeout);
		foreach (const QDBusObjectPath &path, partitionPaths)
			addPartition(path);
	}
	catch (const DBusException &exception)
	{
		clear();
		emit dBusError(exception.what());
	}
}

void PartitionListModel::clear()
{
	int partitionCount = rowCount();

	qDeleteAll(m_partitions);
	m_partitions.clear();

	if (partitionCount)
		emit partitionCountChanged(0);

	bool mountedCountChanged = !m_mountedCountDirty && m_mountedCount != 0;
	m_mountedCount = 0;
	m_mountedCountDirty = false;
	if (mountedCountChanged)
		emit mountedPartitionCountChanged(m_mountedCount);
}

void PartitionListModel::addPartition(const QDBusObjectPath &path)
{
	bool hotplugEvent = sender();

	/* Reseting the whole model is easier than finding affected cells, and it
	 * should not be much of a performance penalty since most computers
	 * probably have very few partitions: */
	beginResetModel();

	try
	{
		Partition *partition = new Partition(path, this);
		partition->setReplyTimeout(m_replyTimeout);
		Drive drive(partition->drivePath());
		drive.setReplyTimeout(m_replyTimeout);
		if (filterDrive(drive))
		{
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
			connect(partition, &Partition::mountPointsChanged, this,
					&PartitionListModel::updatePartitionMountPoints);
#else
			connect(partition, SIGNAL(mountPointsChanged(const QStringList &)),
					this, SLOT(updatePartitionMountPoints()));
#endif
			m_partitions << partition;
			emit partitionCountChanged(rowCount());

			if (partition->isMounted() && !m_mountedCountDirty)
			{
				++m_mountedCount;
				emit mountedPartitionCountChanged(m_mountedCount);
			}

			if (hotplugEvent)
				emit partitionAdded(path.path());
		}
	}
	catch (const DBusException &exception)
	{
		emit dBusError(exception.what());
	}

	endResetModel();
}

void PartitionListModel::enumerate()
{
	beginResetModel();
	clear();
	populateList();
	endResetModel();
}

void PartitionListModel::mountPartition(int row) const
{
	if ( !(row >= 0 && row < rowCount()) )
		return;

	try
	{
		m_partitions.at(row)->mount();
	}
	catch (const DBusException &exception)
	{
		emit dBusError(exception.what());
	}
}

void PartitionListModel::mountPartition(const QString &path) const
{
	foreach (Partition *partition, m_partitions)
		if (partition->path().path() == path)
		{
			try
			{
				partition->mount();
			}
			catch (const DBusException &exception)
			{
				emit dBusError(exception.what());
			}
			return;
		}
}

void PartitionListModel::mountAll() const
{
	foreach (Partition *partition, m_partitions)
		try
		{
			partition->mount();
		}
		catch (const DBusException &exception)
		{
			emit dBusError(exception.what());
		}
}

void PartitionListModel::unmountPartition(int row, bool force) const
{
	if ( !(row >= 0 && row < rowCount()) )
		return;

	try
	{
		m_partitions.at(row)->unmount(force);
	}
	catch (const DBusException &exception)
	{
		emit dBusError(exception.what());
	}
}

void PartitionListModel::unmountPartition(const QString &path, bool force) const
{
	foreach (Partition *partition, m_partitions)
		if (partition->path().path() == path)
		{
			try
			{
				partition->unmount(force);
			}
			catch (const DBusException &exception)
			{
				emit dBusError(exception.what());
			}
			return;
		}
}

void PartitionListModel::unmountAll(bool force) const
{
	foreach (Partition *partition, m_partitions)
		try
		{
			partition->unmount(force);
		}
		catch (const DBusException &exception)
		{
			emit dBusError(exception.what());
		}
}

void PartitionListModel::removePartition(const QDBusObjectPath &path)
{
	bool anyMountedRemoved = false;

	QList<Partition *> partitionsToRemove;
	foreach (Partition *partition, m_partitions)
	{
		/* A drive may have numerous partitions, and at this time one or more of
		 * them may be inaccessible. Remove them straight away before attempting
		 * to access their properties after model is updated (which will trigger
		 * D-Bus errors): */
		if (partition->path() == path || !partition->isValid())
		{
			partitionsToRemove << partition;

			if (!m_mountedCountDirty && m_mountedCount > 0)
			{
				--m_mountedCount;
				anyMountedRemoved = true;
			}

			emit partitionRemoved(partition->path().path());
		}
	}

	beginResetModel();
	foreach (Partition *partition, partitionsToRemove)
	{
		m_partitions.removeAll(partition);
		delete partition;
	}
	endResetModel();

	if (!partitionsToRemove.isEmpty())
		emit partitionCountChanged(rowCount());

	if (anyMountedRemoved)
		emit mountedPartitionCountChanged(m_mountedCount);
}

void PartitionListModel::updatePartitionMountPoints()
{
	const Partition *partition = dynamic_cast<Partition *>(sender());
	if (partition)
	{
		if (!m_mountedCountDirty)
		{
			if (partition->isMounted())
				++m_mountedCount;
			else if (m_mountedCount > 0)
				--m_mountedCount;
		}
	}
	/* This should never happen: */
	else
	{
		m_mountedCount = -1;
		m_mountedCountDirty = true;
	}

	emit mountedPartitionCountChanged(m_mountedCount);

	/* Sure, this could be optimised by only updating the affected row … */
	beginResetModel();
	endResetModel();
}

bool PartitionListModel::filterDrive(const Drive &drive)
{
	QMapIterator<QString, QVariant> i(m_drivePropertyFilter);
	while (i.hasNext())
	{
		i.next();
		QByteArray propertyName = i.key().toLatin1();
		QVariant propertyValue = drive.property(propertyName.constData());
		if ( !(propertyValue.isValid() && propertyValue == i.value()) )
			return false;
	}

	return true;
}
