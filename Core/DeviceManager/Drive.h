/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#ifndef DRIVE_H
#define DRIVE_H

#include <QtCore>
#include "DBusObject.h"
#include <QDateTime>
#include <QDebug>

namespace QDrive
{
	/** \brief Drive object based on UDisks2 Drive interface
	 *
	 * Check the <a href="http://udisks.freedesktop.org/docs/latest/">UDisks2
	 * documentation</a> for information about properties
	 */
	class Drive : public DBusObject
	{
		Q_OBJECT
		Q_PROPERTY(QString vendor READ vendor CONSTANT);
		Q_PROPERTY(QString model READ model CONSTANT);
		Q_PROPERTY(QString serial READ serial CONSTANT);
		Q_PROPERTY(QString id READ id CONSTANT);
		Q_PROPERTY(bool mediaRemovable READ isMediaRemovable CONSTANT);
		Q_PROPERTY(bool mediaAvailable READ isMediaAvailable CONSTANT);
		Q_PROPERTY(qulonglong size READ size CONSTANT);
		Q_PROPERTY(QDateTime timeDetected READ timeDetected CONSTANT);
		Q_PROPERTY(QDateTime timeMediaDetected READ timeMediaDetected CONSTANT);
		Q_PROPERTY(bool usb READ isUSB CONSTANT);
		Q_PROPERTY(bool removable READ isRemovable CONSTANT);

	public:
		Drive(const QDBusObjectPath &path, QObject *parent = NULL);

		/** \brief Tells whether object is a drive by calling isDrive() */
		bool isValid() const;
		/** \brief Check whether object at given path is a drive
		 *
		 * This function calls introspect() and ensures that the object provides
		 * the <a
		 * href="http://udisks.freedesktop.org/docs/latest/gdbus-org.freedesktop.UDisks2.Drive.html">UDisks2
		 * Drive interface</a>
		 */
		static bool isDrive(const QDBusObjectPath &path, int replyTimeout);
		/** \brief Check the given list of interfaces to see whether it contains
		 * <a
		 * href="http://udisks.freedesktop.org/docs/latest/gdbus-org.freedesktop.UDisks2.Drive.html">Drive</a>
		 */
		static bool isDrive(const QList<QString> &interfaces);

		QString vendor() const;
		QString model() const;
		QString serial() const;
		QString id() const;
		bool isMediaRemovable() const;
		bool isMediaAvailable() const;
		qulonglong size() const;
		QDateTime timeDetected() const;
		QDateTime timeMediaDetected() const;
		bool isUSB() const;
		bool isRemovable() const;
	};
};

QDebug operator <<(QDebug d, const QDrive::Drive &drive);

#endif // DRIVE_H
