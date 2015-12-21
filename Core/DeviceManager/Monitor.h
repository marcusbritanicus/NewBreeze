/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#ifndef MONITOR_H
#define MONITOR_H

#include <QtCore>
#include <QObject>
#include <QMetaType>
#include <QVariant>

class QDBusObjectPath;

namespace QDrive
{
	/** \brief Drive and partition hotplug event monitor
	 *
	 * Emits a signal whenever a drive or a partition is added or removed
	 *
	 * \todo Make sure UDisks2 daemon is running before attempting to connect
	 * signals. Start the daemon if it is not running and attempt to
	 * reinitialise (connect signals) at a later time.
	 */
	class Monitor : public QObject
	{
		Q_OBJECT

	signals:
		void driveAdded(const QDBusObjectPath &path);
		void driveRemoved(const QDBusObjectPath &path);
		void partitionAdded(const QDBusObjectPath &path);
		void partitionRemoved(const QDBusObjectPath &path);

	public:
		typedef QMap<QString, QVariantMap> InterfaceList;

		explicit Monitor(QObject *parent = NULL);

	private slots:
		void checkAddedInterfaces(const QDBusObjectPath &path, const
				QDrive::Monitor::InterfaceList &interfaces);
		void checkRemovedInterfaces(const QDBusObjectPath &path, const QStringList
				&interfaces);
	};
};

Q_DECLARE_METATYPE(QDrive::Monitor::InterfaceList);

#endif // MONITOR_H
