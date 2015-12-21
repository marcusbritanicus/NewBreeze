/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#include "QDrive.h"
#include "DBusObject.h"
using namespace QDrive;
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>

DBusObject::DBusObject(const QDBusObjectPath &path, QObject *parent)
	: QObject(parent), m_path(path), m_replyTimeout(100)
{
}

bool DBusObject::operator ==(const QDBusObjectPath &path) const
{
	return m_path == path;
}

QDBusObjectPath DBusObject::path() const
{
	return m_path;
}

bool DBusObject::isValid() const
{
	try
	{
		return !introspect().isEmpty();
	}
	catch (const DBusException &exception)
	{
		return false;
	}
}

int DBusObject::replyTimeout() const
{
	return m_replyTimeout;
}

void DBusObject::setReplyTimeout(int replyTimeout)
{
	m_replyTimeout = replyTimeout;
}

QVariant DBusObject::dbusProperty(const QString &interface, const QString 
		&property) const
{
	QDBusConnection sys = QDBusConnection::systemBus();
	QDBusMessage getProperty = QDBusMessage::createMethodCall(UDisks2::service(),
			m_path.path(), Interface::Properties(), "Get");

	QVariantList arguments;
	arguments << interface << property;
	getProperty.setArguments(arguments);

	QDBusReply<QVariant> reply = sys.call(getProperty, QDBus::Block, m_replyTimeout);
	if (!reply.isValid())
		throw DBusException(reply.error().message().toStdString());

	return reply.value();
}

QString DBusObject::introspect(const QString &path, int replyTimeout)
{
	QDBusConnection sys = QDBusConnection::systemBus();
	QDBusMessage introspect = QDBusMessage::createMethodCall(UDisks2::service(), 
			path, Interface::Introspectable(), "Introspect");

	QDBusReply<QString> reply = sys.call(introspect, QDBus::Block, replyTimeout);
	if (!reply.isValid())
		throw DBusException(reply.error().message().toStdString());

	return reply.value();
}

QString DBusObject::introspect() const
{
	return introspect(m_path.path(), m_replyTimeout);
}
