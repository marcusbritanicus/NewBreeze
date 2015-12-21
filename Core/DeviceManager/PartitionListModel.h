/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#ifndef PARTITIONLISTMODEL_H
#define PARTITIONLISTMODEL_H

#include <QtCore>
#include "Monitor.h"

namespace QDrive
{
	class Partition;
	class Drive;

	/** \brief A table model for showing drive/partition information
	 *
	 * This table model lists all partitions matching a given filter and shows
	 * the partitions' drive properties as well. The filter can be used to list
	 * only partitions belonging to drives matching certain criteria, for
	 * instance removable media.
	 *
	 * The model implements user-defined roles and can be used from QML.
	 *
	 * \note enumerate() must be called after creating the model to probe for
	 * drives/partitions. The drivePropertyFilter can be set before
	 * enumeration, preventing unwanted signal emissions.
	 */
	class PartitionListModel : public QAbstractTableModel
	{
		Q_OBJECT
		Q_ENUMS(Role)
		/** \brief Filter consisting of drive property names and values
		 *
		 * Every drive discovered during enumerate() and during hotplug events
		 * will be checked against these criteria. Only if all the listed
		 * properties and their values match, the partitions belonging to the
		 * given drive will be added to the model. Changing the filter
		 * automatically repopulates the model.
		 */
		Q_PROPERTY(QVariantMap drivePropertyFilter READ drivePropertyFilter
				WRITE setDrivePropertyFilter);
		/** \brief Number of partitions belonging to drives matching filter */
		Q_PROPERTY(int partitionCount READ rowCount NOTIFY partitionCountChanged);
		/** \brief Maximum time in ms spent blocking for a D-Bus reply
		  *
		  * Default is 100 ms
		  */
		Q_PROPERTY(int replyTimeout READ replyTimeout WRITE setReplyTimeout);
		/** \brief Number of mounted partitions
		 *
		 * This property contains the number of mounted partitions (filtered by
		 * by drivePropertyFilter). The property may be -1 if a D-Bus error
		 * occured during updating of the property. If the value ever becomes
		 * -1 the value must be manually probed, since a changed signal will not
		 *  be emitted when it becomes valid again.
		 */
		Q_PROPERTY(int mountedPartitionCount READ mountedPartitionCount NOTIFY
				mountedPartitionCountChanged);

	signals:
		void dBusError(const QString &errorMessage) const;
		void partitionCountChanged(int partitionCount) const;
		void mountedPartitionCountChanged(int mountedPartitionCount) const;
		/** \brief A partition belonging to a drive matching the installed
		 * filter is added due to a hotplug event */
		void partitionAdded(const QString &path);
		void partitionRemoved(const QString &path);

	public:
		enum Role
		{
			/** \brief Partition path (retuns QString, not QDBusObjectPath) */
			PartitionPath = Qt::UserRole + 1,
			/** \see Partition::device */
			PartitionDevice,
			/** \see Partition::id */
			PartitionID,
			/** \see Partition::size */
			PartitionSize,
			/** \see Partition::readOnly */
			PartitionReadOnly,
			/** \see Partition::label */
			PartitionLabel,
			/** \see Partition::uuid */
			PartitionUUID,
			/** \see Partition::partitionNumber */
			PartitionNumber,
			/** Partition number + space + partition label */
			PartitionNumberAndLabel,
			/** \see Partition::offset */
			PartitionOffset,
			/** \see Partition::type */
			PartitionType,
			/** \see Partition::mounted */
			PartitionMounted,
			/** \see Partition::mountPoints */
			PartitionMountPoints,
			/** \see Partition::availableSpace */
			PartitionAvailableSpace,

			/** Drive path (returns QString, not QDBusObjectPath) */
			DrivePath,
			/** \see Drive::vendor */
			DriveVendor,
			/** \see Drive::model */
			DriveModel,
			/** \see Drive::serial */
			DriveSerial,
			/** \see Drive::id */
			DriveID,
			/** \see Drive::mediaRemovable */
			DriveMediaRemovable,
			/** \see Drive::mediaAvailable */
			DriveMediaAvailable,
			/** \see Drive::size */
			DriveSize,
			/** \see Drive::timeDetected */
			DriveTimeDetected,
			/** \see Drive::timeMediaDetected */
			DriveMediaTimeDetected,
			/** \see Drive::usb */
			DriveUSB,
			/** \see Drive::removable */
			DriveRemovable,
		};

		explicit PartitionListModel(QObject *parent = NULL);
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		QHash<int, QByteArray> roleNames() const;
		bool validIndex(const QModelIndex &index, int role) const;
		bool validIndex(int row, int column) const;
		Q_INVOKABLE QVariant data(int row, int role) const;
		Q_INVOKABLE QVariant data(const QString &path, int role) const;

		QVariantMap drivePropertyFilter() const;
		void setDrivePropertyFilter(const QVariantMap &propertyFilter);
		int replyTimeout() const;
		void setReplyTimeout(int replyTimeout);
		int mountedPartitionCount() const;

	public slots:
		void enumerate();
		void mountPartition(int row) const;
		void mountPartition(const QString &path) const;
		void mountAll() const;
		void unmountPartition(int row, bool force = false) const;
		void unmountPartition(const QString &path, bool force = false) const;
		void unmountAll(bool force = false) const;

#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
	private:
#else
	private slots:
#endif
		void addPartition(const QDBusObjectPath &path);
		void removePartition(const QDBusObjectPath &path);
		void updatePartitionMountPoints();

	private:
		void populateList();
		void clear();
		bool filterDrive(const Drive &drive);

		int m_replyTimeout;
		Monitor m_monitor;
		QList<Partition *> m_partitions;
		QHash<int, QByteArray> m_roleNames;
		QVariantMap m_drivePropertyFilter;
		mutable int m_mountedCount;
		mutable bool m_mountedCountDirty;
	};
};

#endif // PARTITIONLISTMODEL_H
