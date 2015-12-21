/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#ifndef DBUSOBJECT_H
#define DBUSOBJECT_H

#include <QtCore>
#include <QObject>
#include <QDBusObjectPath>

namespace QDrive
{
	/**
	 * \brief Object with a D-Bus address and possibly properties
	 *
	 * This helper class provides helpful methods like introspect() for listing
	 * the object's methods and properties, and dbusProperty() for accessing
	 * properties.
	 */
	class DBusObject : public QObject
	{
		Q_OBJECT
		// Q_PROPERTY(QDBusObjectPath path MEMBER m_path);
		/** \brief Tells whether the object is accessible and valid
		 *
		 * Subclasses should ensure that the object provides the expected
		 * interfaces.
		 */
		Q_PROPERTY(bool valid READ isValid);

	public:
		DBusObject(const QDBusObjectPath &path, QObject *parent = NULL);
		bool operator ==(const QDBusObjectPath &path) const;

		QDBusObjectPath path() const;
		/** \brief Check if the object is valid
		 *
		 * If a D-Bus error occurs the result is false. This function should be
		 * overriden; the base implementation only checks if introspect() on the
		 * object's path is non-empty (checks whether it exists).
		 */
		virtual bool isValid() const;
		/** \brief Maximum time in ms spent blocking for a D-Bus reply
		 *
		 * Default is 100 ms
		 */
		int replyTimeout() const;

		void setReplyTimeout(int replyTimeout);

		/** \brief Retrieve the value of a property on the given interface
		 *
		 * If a D-Bus error occcurs a DBusException will be thrown.
		 */
		QVariant dbusProperty(const QString &interface, const QString &property) const;
		/** \brief List the object's methods, signals and properties
		 *
		 * If a D-Bus error occcurs a DBusException will be thrown.
		 *
		 * \returns XML result as a QString
		 */
		static QString introspect(const QString &path, int replyTimeout);
		QString introspect() const;

	private:
		QDBusObjectPath m_path;
		int m_replyTimeout;
	};
};

#endif // DBUSOBJECT_H
