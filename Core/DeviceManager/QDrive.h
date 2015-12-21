/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#ifndef QDRIVE_H
#define QDRIVE_H

#include <QtCore>
#include <stdexcept>
#include <QString>
#include "Drive.h"
#include "Partition.h"
#include <QDebug>

namespace QDrive
{
	class DBusException : public std::runtime_error
	{
	public:
		DBusException(const std::string message)
			: runtime_error(message)
		{
		};
	};

	/** \brief UDisks2 object paths */
	namespace UDisks2
	{
		QString service();
		QString path();
		QString path(const QString &subPath);
	};

	/** \brief UDisks2 and D-Bus interfaces */
	namespace Interface
	{
		QString UDisks2();
		QString UDisks2(const QString &interface);
		QString ObjectManager();
		QString Introspectable();
		QString Properties();
	};

	/** \brief List all drives on the computer
	 *
	 * \param replyTimeout Maximum (blocking) waiting time for a reply in ms
	 * \returns A list of device object paths
	 */
	QList<QDBusObjectPath> drives(int replyTimeout);
	/** \brief List all partitions on the computer
	 *
	 * \param replyTimeout Maximum (blocking) waiting time for a reply in ms
	 * \returns A list of partition object paths
	 */
	QList<QDBusObjectPath> partitions(int replyTimeout);
};

QDebug operator <<(QDebug d, const QDBusObjectPath &path);

#endif // QDRIVE_H
