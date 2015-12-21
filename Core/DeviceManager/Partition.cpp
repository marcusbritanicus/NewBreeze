/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#include "QDrive.h"
#include "Partition.h"
#include "Drive.h"
#include "AvailableSpace.h"
using namespace QDrive;
#include <QXmlStreamReader>
#include <QDBusConnection>
#include <QDBusArgument>
#include <QDBusError>
#include <QDBusMessage>
#include <QDBusReply>

Partition::Partition(const QDBusObjectPath &path, QObject *parent)
	: DBusObject(path, parent)
{
	QDBusConnection sys = QDBusConnection::systemBus();
	bool connectionOK = sys.connect( UDisks2::service(), path.path(), 
			Interface::Properties(), "PropertiesChanged", this, SLOT(
				inspectFileSysPropertyChanges(const QString &, const QVariantMap &, 
					const QStringList &)));

	if (!connectionOK)
		throw DBusException("Could not connect PropertiesChanged signal");
}

bool Partition::isValid() const
{
	try
	{
		return isPartition(path(), replyTimeout());
	}
	catch (const DBusException &exception)
	{
		return false;
	}
}

bool Partition::isPartition(const QDBusObjectPath &path, int replyTimeout)
{
	QXmlStreamReader xml(introspect(path.path(), replyTimeout));

	bool hasBlockInterface = false;
	bool hasPartitionInterface = false;
	bool hasFileSystemInterface = false;
	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement() && xml.name() == "interface")
		{
			QStringRef interface = xml.attributes().value("name");
			if (!hasBlockInterface && interface == Interface::UDisks2("Block"))
				hasBlockInterface = true;
			else if (!hasPartitionInterface && interface == Interface::UDisks2(
						"Partition"))
				hasPartitionInterface = true;
			else if (!hasFileSystemInterface && interface == Interface::UDisks2(
						"Filesystem"))
				hasFileSystemInterface = true;

			if (hasBlockInterface && hasPartitionInterface && hasFileSystemInterface)
				return true;
		}
	}

	return false;
}

bool Partition::isPartition(const QList<QString> &interfaces)
{
	bool hasBlockInterface = false;
	bool hasPartitionInterface = false;
	bool hasFileSystemInterface = false;

	foreach (const QString &interface, interfaces)
	{
		if (!hasBlockInterface && interface == Interface::UDisks2("Block"))
			hasBlockInterface = true;
		else if (!hasPartitionInterface && interface == Interface::UDisks2(
					"Partition"))
			hasPartitionInterface = true;
		else if (!hasFileSystemInterface && interface == Interface::UDisks2(
					"Filesystem"))
			hasFileSystemInterface = true;

		if (hasBlockInterface && hasPartitionInterface && hasFileSystemInterface)
			return true;
	}

	return false;
}

QString Partition::device() const
{
	return dbusProperty(Interface::UDisks2("Block"), "Device").toString();
}

QString Partition::id() const
{
	return dbusProperty(Interface::UDisks2("Block"), "Id").toString();
}

qulonglong Partition::size() const
{
	return dbusProperty(Interface::UDisks2("Block"), "Size").toULongLong();
}

bool Partition::isReadOnly() const
{
	return dbusProperty(Interface::UDisks2("Block"), "ReadOnly").toBool();
}

QDBusObjectPath Partition::drivePath() const
{
	return dbusProperty(Interface::UDisks2("Block"), "Drive").value<QDBusObjectPath>();
}

QObject *Partition::driveObject() const
{
	return new Drive(drivePath());
}

QString Partition::label() const
{
	return dbusProperty(Interface::UDisks2("Block"), "IdLabel").toString();
}

QString Partition::uuid() const
{
	return dbusProperty(Interface::UDisks2("Block"), "IdUUID").toString();
}

int Partition::partitionNumber() const
{
	return dbusProperty(Interface::UDisks2("Partition"), "Number").toInt();
}

qulonglong Partition::offset() const
{
	return dbusProperty(Interface::UDisks2("Partition"), "Offset").toULongLong();
}

QString Partition::type() const
{
	return dbusProperty(Interface::UDisks2("Partition"), "Type").toString();
}

bool Partition::isMounted() const
{
	return !mountPoints().isEmpty();
}

QStringList Partition::mountPoints() const
{
	const QDBusArgument rawList = dbusProperty(Interface::UDisks2("Filesystem"), 
			"MountPoints").value<QDBusArgument>();

	return dBusArgConvertAAY(rawList);
}

quint64 Partition::availableSpace() const
{
	if (!isMounted())
		return 0;

	return QDrive::availableSpace(mountPoints().at(0));
}

void Partition::mount() const
{
	QDBusConnection sys = QDBusConnection::systemBus();
	QDBusMessage mount = QDBusMessage::createMethodCall(UDisks2::service(), 
			path().path(), Interface::UDisks2("Filesystem"), "Mount");

	QVariantList arguments;
	QVariantMap options;
	options["auth.no_user_interaction"] = true;
	arguments << options;

	mount.setArguments(arguments);

	QDBusReply<QString> reply = sys.call(mount, QDBus::Block, replyTimeout());
	if (!reply.isValid())
		throw DBusException(reply.error().message().toStdString());
}

void Partition::unmount(bool force) const
{
	QDBusConnection sys = QDBusConnection::systemBus();
	QDBusMessage unmount = QDBusMessage::createMethodCall(UDisks2::service(), 
			path().path(), Interface::UDisks2("Filesystem"), "Unmount");

	QVariantList arguments;
	QVariantMap options;
	if (force)
		options["force"] = true;

	arguments << options;

	unmount.setArguments(arguments);

	/* TODO: This can return an error. Please catch it … */
	sys.call(unmount, QDBus::NoBlock);
}

void Partition::inspectFileSysPropertyChanges(const QString &interface, const 
		QVariantMap &changedProperties, const QStringList &invalidatedProperties)
{
	Q_UNUSED(interface);

	if (invalidatedProperties.contains("MountPoints"))
	{
		emit mountPointsChanged(QStringList());
		return;
	}

	if (changedProperties.contains("MountPoints"))
		emit mountPointsChanged(dBusArgConvertAAY(changedProperties.value(
						"MountPoints").value<QDBusArgument>()));
}

QStringList Partition::dBusArgConvertAAY(const QDBusArgument &byteArrayArray) const
{
	QStringList stringList;
	byteArrayArray.beginArray();
	while (!byteArrayArray.atEnd())
		stringList << qdbus_cast<QByteArray>(byteArrayArray);

	byteArrayArray.endArray();

	return stringList;
}


QDebug operator <<(QDebug d, const QDrive::Partition &partition)
{
	d.nospace() << "Partition " << partition.path().path() << ":\n";

	bool valid = partition.isValid();
	d.nospace() << "  valid:\t\t" << valid;
	if (valid)
	{
		d.nospace() << "\n  device:\t\t" << partition.device() << '\n';
		d.nospace() << "  ID:\t\t\t" << partition.id() << '\n';
		d.nospace() << "  size:\t\t\t" << partition.size() << '\n';
		d.nospace() << "  read-only:\t\t" << partition.isReadOnly() << '\n';
		d.nospace() << "  drive:\t\t" << partition.drivePath().path() << '\n';
		d.nospace() << "  label:\t\t" << partition.label() << '\n';
		d.nospace() << "  UUID:\t\t\t" << partition.uuid() << '\n';
		d.nospace() << "  partition number:\t" << partition.partitionNumber() << '\n';
		d.nospace() << "  offset:\t\t" << partition.offset() << '\n';
		d.nospace() << "  type:\t\t\t" << partition.type() << '\n';
		d.nospace() << "  mounted:\t\t" << partition.isMounted() << '\n';
		d.nospace() << "  mount points:\t\t" << partition.mountPoints();
	}

	return d.nospace();
}
