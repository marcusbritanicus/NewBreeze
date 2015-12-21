/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#ifndef PARTITION_H
#define PARTITION_H

#include <QtCore>
#include "DBusObject.h"
#include <QDebug>

class QDBusArgument;

namespace QDrive
{
	/** \brief Partition object based on several UDisks2 interfaces
	 *
	 * Uses UDisks2's <a
	 * href="http://udisks.freedesktop.org/docs/latest/gdbus-org.freedesktop.UDisks2.Block.html">Block</a>,
	 * <a
	 * href="http://udisks.freedesktop.org/docs/latest/gdbus-org.freedesktop.UDisks2.Partition.html">Partition</a>
	 * and <a
	 * href="http://udisks.freedesktop.org/docs/latest/gdbus-org.freedesktop.UDisks2.Filesystem.html">Filesystem</a>
	 * interfaces. Check the UDisks2 documentation for information about
	 * properties and methods.
	 */
	class Partition : public DBusObject
	{
		Q_OBJECT
		Q_PROPERTY(QString device READ device CONSTANT);
		Q_PROPERTY(QString id READ id CONSTANT);
		Q_PROPERTY(qulonglong size READ size CONSTANT);
		Q_PROPERTY(bool readOnly READ isReadOnly CONSTANT);
		Q_PROPERTY(QDBusObjectPath drivePath READ drivePath CONSTANT);
		Q_PROPERTY(QObject *driveObject READ driveObject CONSTANT);
		Q_PROPERTY(QString label READ label CONSTANT);
		Q_PROPERTY(QString uuid READ uuid CONSTANT);
		Q_PROPERTY(int partitionNumber READ partitionNumber CONSTANT);
		Q_PROPERTY(qulonglong offset READ offset CONSTANT);
		Q_PROPERTY(QString type READ type CONSTANT);
		Q_PROPERTY(bool mounted READ isMounted NOTIFY mountPointsChanged);
		Q_PROPERTY(QStringList mountPoints READ mountPoints NOTIFY
				mountPointsChanged);
		/** \brief Available space in mounted filesystem
		 *
		 * \note Filesystem must be mounted
		 * \returns Number of available bytes, or 0 if filesystem is not mounted
		 * or if available space could not be retrieved.
		 */
		Q_PROPERTY(quint64 availableSpace READ availableSpace);

	signals:
		void mountPointsChanged(const QStringList mountPoints);

	public:
		Partition(const QDBusObjectPath &path, QObject *parent = NULL);

		/** \brief Test whether object is a partition by calling isPartition()
		 * */
		bool isValid() const;
		/** \brief Check whether object at given path is a partition
		 *
		 * This function calls introspect() and ensures that the object provides
		 * the
		 * href="http://udisks.freedesktop.org/docs/latest/gdbus-org.freedesktop.UDisks2.Block.html">Block</a>,
		 * <a
		 * href="http://udisks.freedesktop.org/docs/latest/gdbus-org.freedesktop.UDisks2.Partition.html">Partition</a>
		 * and <a
		 * href="http://udisks.freedesktop.org/docs/latest/gdbus-org.freedesktop.UDisks2.Filesystem.html">Filesystem</a>
		 * interfaces.
		 */
		static bool isPartition(const QDBusObjectPath &path, int replyTimeout);
		static bool isPartition(const QList<QString> &interfaces);

		QString device() const;
		QString id() const;
		qulonglong size() const;
		bool isReadOnly() const;
		QDBusObjectPath drivePath() const;
		QObject *driveObject() const;
		QString label() const;
		QString uuid() const;

		int partitionNumber() const;
		qulonglong offset() const;
		/** Partition type
		 *
		 * For DOS partition tables, the string is a hexadecimal number (see
		 * https://en.wikipedia.org/wiki/Partition_type).
		 * For GPT partition tables, the string is a UUID (see
		 * https://en.wikipedia.org/wiki/GUID_Partition_Table)
		 */
		QString type() const;

		bool isMounted() const;
		QStringList mountPoints() const;
		quint64 availableSpace() const;

	public slots:
		void mount() const;
		void unmount(bool force = false) const;

	private slots:
		void inspectFileSysPropertyChanges(const QString &interface, const
				QVariantMap &changedProperties, const QStringList
				&invalidatedProperties);

	private:
		QStringList dBusArgConvertAAY(const QDBusArgument &byteArrayArray) const;
	};
};

QDebug operator <<(QDebug d, const QDrive::Partition &partition);

#endif // PARTITION_H
