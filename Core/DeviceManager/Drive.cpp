/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#include "QDrive.h"
#include "Drive.h"
using namespace QDrive;
#include <QXmlStreamReader>

Drive::Drive(const QDBusObjectPath &path, QObject *parent)
	: DBusObject(path, parent)
{
}

bool Drive::isValid() const
{
	try
	{
		return isDrive(path(), replyTimeout());
	}
	catch (const DBusException &exception)
	{
		return false;
	}
}

bool Drive::isDrive(const QDBusObjectPath &path, int replyTimeout)
{
	QXmlStreamReader xml(introspect(path.path(), replyTimeout));
	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement() && xml.name() == "interface" &&
			xml.attributes().value("name") == Interface::UDisks2("Drive"))
			return true;
	}

	return false;
}

bool Drive::isDrive(const QList<QString> &interfaces)
{
	return interfaces.contains(Interface::UDisks2("Drive"));
}

QString Drive::vendor() const
{
	return dbusProperty(Interface::UDisks2("Drive"), "Vendor").toString();
}

QString Drive::model() const
{
	return dbusProperty(Interface::UDisks2("Drive"), "Model").toString();
}

QString Drive::serial() const
{
	return dbusProperty(Interface::UDisks2("Drive"), "Serial").toString();
}

QString Drive::id() const
{
	return dbusProperty(Interface::UDisks2("Drive"), "Id").toString();
}

bool Drive::isMediaRemovable() const
{
	return dbusProperty(Interface::UDisks2("Drive"), "MediaRemovable").toBool();
}

bool Drive::isMediaAvailable() const
{
	return dbusProperty(Interface::UDisks2("Drive"), "MediaAvailable").toBool();
}

qulonglong Drive::size() const
{
	return dbusProperty(Interface::UDisks2("Drive"), "Size").toULongLong();
}

QDateTime Drive::timeDetected() const
{
	return QDateTime::fromMSecsSinceEpoch( dbusProperty(Interface::UDisks2("Drive"), 
				"TimeDetected").toULongLong()/1000 );
}

QDateTime Drive::timeMediaDetected() const
{
	return QDateTime::fromMSecsSinceEpoch( dbusProperty(Interface::UDisks2("Drive"), 
				"TimeMediaDetected").toULongLong()/1000 );
}

bool Drive::isUSB() const
{
	return dbusProperty(Interface::UDisks2("Drive"), "ConnectionBus").toString()
		== "usb";
}

bool Drive::isRemovable() const
{
	return dbusProperty(Interface::UDisks2("Drive"), "Removable").toBool();
}


QDebug operator <<(QDebug d, const QDrive::Drive &drive)
{
	d.nospace() << "Drive " << drive.path().path() << ":\n";

	bool valid = drive.isValid();
	d.nospace() << "  valid:\t\t" << valid;
	if (valid)
	{
		d.nospace() << "\n  vendor:\t\t" << drive.vendor() << '\n';
		d.nospace() << "  model:\t\t" << drive.model() << '\n';
		d.nospace() << "  serial:\t\t" << drive.serial() << '\n';
		d.nospace() << "  id:\t\t\t" << drive.id() << '\n';
		d.nospace() << "  media removable:\t" << drive.isMediaRemovable() << '\n';
		d.nospace() << "  media available:\t" << drive.isMediaAvailable() << '\n';
		d.nospace() << "  size:\t\t\t" << drive.size() << '\n';
		d.nospace() << "  time detected:\t" << drive.timeDetected() << '\n';
		d.nospace() << " time media detected:\t" << drive.timeMediaDetected() << '\n';
		d.nospace() << " USB:\t\t\t" << drive.isUSB() << '\n';
		d.nospace() << "  removable:\t\t" << drive.isRemovable();
	}

	return d.nospace();
}
