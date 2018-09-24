#include "udisks2.h"

// Custom type for unmarhsalling byte arrays
typedef QList<unsigned char> dbus_ay;
QString dbus_ay_toString(const dbus_ay &data) {
    QString output;
    foreach(unsigned char c, data)
        if (c)  output.append(c);
    return output;
}
Q_DECLARE_METATYPE(dbus_ay)

UDisks2::UDisks2(QObject *parent) :
    QObject(parent)
{
    auto system = QDBusConnection::systemBus();
    system.connect("org.freedesktop.UDisks2",
                   "/org/freedesktop/UDisks2",
                   "org.freedesktop.DBus.ObjectManager",
                   "InterfacesAdded",
                   this,
                   SLOT(dbus_interfaceAdded(QDBusObjectPath,QMap<QString,QVariant>)));

    system.connect("org.freedesktop.UDisks2",
                   "/org/freedesktop/UDisks2",
                   "org.freedesktop.DBus.ObjectManager",
                   "InterfacesRemoved",
                   this,
                   SLOT(dbus_interfaceRemoved(QDBusObjectPath,QStringList)));

    foreach (QString block, blockDevices()) {
        addBlock(block);
    }
    foreach (QString drive, drives()) {
        addDrive(drive);
    }
}

static QString lastPart(const QString &path) {
    return path.split('/').last();
}

UDisks2::~UDisks2()
{

}

QStringList UDisks2::blockDevices()
{
    QDBusInterface ud2("org.freedesktop.UDisks2",
                       "/org/freedesktop/UDisks2/block_devices",
                       "org.freedesktop.DBus.Introspectable",
                       QDBusConnection::systemBus());
    QDBusReply<QString> reply = ud2.call("Introspect");
    if (!reply.isValid())
        return QStringList();
    QXmlStreamReader xml(reply.value());
    QStringList response;
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name().toString() == "node") {
            QString name = xml.attributes().value("name").toString();
            if (!name.isEmpty())
                response << name;
        }
    }
    return response;
}

UDisks2Block *UDisks2::blockDevice(const QString &node)
{
    return blocks_.contains(node) ? blocks_[node] : NULL;
}

QStringList UDisks2::drives()
{
    QDBusInterface ud2("org.freedesktop.UDisks2",
                       "/org/freedesktop/UDisks2/drives",
                       "org.freedesktop.DBus.Introspectable",
                       QDBusConnection::systemBus());
    if (!ud2.isValid())
        return QStringList();
    QDBusReply<QString> reply = ud2.call("Introspect");
    if (!reply.isValid())
        return QStringList();
    QXmlStreamReader xml(reply.value());
    QStringList response;
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name().toString() == "node") {
            QString name = xml.attributes().value("name").toString();
            if (!name.isEmpty())
                response << name;
        }
    }
    return response;
}

UDisks2Drive *UDisks2::drive(const QString &node)
{
    return drives_.contains(node) ? drives_[node] : NULL;
}

void UDisks2::addDrive(const QString &node)
{
    if (drives_.contains(node))
        drives_.value(node)->update();
    else {
        auto drive = new UDisks2Drive(node, this);
        connect(drive, &UDisks2Drive::changed,
                this, &UDisks2::driveChanged);
        drives_.insert(node, drive);
        emit driveAdded(node);
    }
}

void UDisks2::addBlock(const QString &node)
{
    if (blocks_.contains(node))
        blocks_.value(node)->update();
    else {
        auto block = new UDisks2Block(node, this);
        connect(block, &UDisks2Block::filesystemAdded,
                this, &UDisks2::filesystemAdded);
        connect(block, &UDisks2Block::filesystemRemoved,
                this, &UDisks2::filesystemRemoved);
        connect(block, &UDisks2Block::filesystemChanged,
                this, &UDisks2::filesystemChanged);
        connect(block, &UDisks2Block::changed,
                this, &UDisks2::blockDeviceChanged);
        blocks_.insert(node, block);
        emit blockDeviceAdded(node);
    }
}

void UDisks2::removeDrive(const QString &node)
{
    if (drives_.contains(node))
        delete drives_.take(node);
}

void UDisks2::removeBlock(const QString &node)
{
    if (blocks_.contains(node))
        delete blocks_.take(node);
}

void UDisks2::dbus_interfaceAdded(const QDBusObjectPath &path, const QMap<QString, QVariant> &interfaces)
{
    // path: o [path]
    // nodes: a{sa{sv} [dict of strings dict of string variants]
    QString node = lastPart(path.path());
    if (path.path().startsWith("/org/freedesktop/UDisks2/block_devices")) {
        if (interfaces.contains("org.freedesktop.UDisks2.Block"))
            addBlock(node);
        if (interfaces.contains("org.freedesktop.UDisks2.Filesystem")
                && blocks_.contains(node))
            blocks_[node]->addFilesystem();
    } else if (path.path().startsWith("/org/freedesktop/UDisks2/drives")) {
        if (interfaces.contains("org.freedesktop.UDisks2.Drive"))
            addDrive(node);
    }
}

void UDisks2::dbus_interfaceRemoved(const QDBusObjectPath &path, const QStringList &interfaces)
{
    // path: o [path]
    // interfaces: as [list of strings]
    QString node = lastPart(path.path());
    if (path.path().startsWith("/org/freedesktop/UDisks2/block_devices")) {
        if (interfaces.contains("org.freedesktop.UDisks2.Block")) {
            removeBlock(node);
            emit blockDeviceRemoved(node);
        }
        if (interfaces.contains("org.freedesktop.UDisks2.Filesystem")
                && blocks_.contains(node)) {
            blocks_[node]->removeFilesystem();
        }
    } else if (path.path().startsWith("/org/freedesktop/UDisks2/drives")) {
        removeDrive(node);
        emit driveRemoved(node);
    }
}

UDisks2Block::UDisks2Block(const QString &node, QObject *parent) :
    QObject(parent), name(node), fs(NULL)
{
    QDBusConnection system = QDBusConnection::systemBus();
    dbus = new QDBusInterface("org.freedesktop.UDisks2",
                              "/org/freedesktop/UDisks2/block_devices/" + node,
                              "org.freedesktop.UDisks2.Block",
                              system, parent);
    system.connect(dbus->service(), dbus->path(),
                   "org.freedesktop.DBus.Properties", "PropertiesChanged",
                   this, SLOT(self_propertiesChanged(QString,QVariantMap,QStringList)));
    update();
    if (!type.isEmpty())
        addFilesystem();
}

QString UDisks2Block::toString()
{
    return QString("name: %1\ndev: %2\nid: %3\ndrive: %4\nsize: %5\n"
                   "readonly: %6\nusage: %7\ntype: %8")
            .arg(name).arg(dev).arg(id).arg(drive).arg(size).arg(readonly)
            .arg(usage).arg(type);
}

void UDisks2Block::update()
{
    dev = dbus->property("Device").toString();
    id = dbus->property("Id").toString();
    drive = lastPart(dbus->property("Drive").value<QDBusObjectPath>().path());
    size = dbus->property("Size").toULongLong();
    readonly = dbus->property("ReadOnly").toBool();
    usage = dbus->property("IdUsage").toString();
    type = dbus->property("IdType").toString();
}

void UDisks2Block::updateFilesystem()
{
    if (fs)
        fs->update();
}

void UDisks2Block::addFilesystem()
{
    if (fs && !fs->isValid()) {
        delete fs;
        fs = NULL;
    }
    if (!fs)
        fs = new UDisks2Filesystem(name);
    emit filesystemAdded(name);
}

void UDisks2Block::removeFilesystem()
{
    emit filesystemRemoved(name);
    if (fs) delete fs;
    fs = NULL;
}

UDisks2Filesystem *UDisks2Block::fileSystem()
{
    return fs;
}

void UDisks2Block::self_propertiesChanged(const QString &interface, const QVariantMap &, const QStringList& )
{
    if (interface == "org.freedesktop.UDisks2.Block") {
        update();
        emit changed(name);
    }
    if (interface == "org.freedesktop.UDisks2.Filesystem") {
        updateFilesystem();
        emit filesystemChanged(name);
    }
}


UDisks2Drive::UDisks2Drive(const QString &node, QObject *parent) :
    QObject(parent), name(node)
{
    QDBusConnection system = QDBusConnection::systemBus();
    dbus = new QDBusInterface("org.freedesktop.UDisks2",
                              "/org/freedesktop/UDisks2/drives/" + node,
                              "org.freedesktop.UDisks2.Drive",
                              system, parent);
    system.connect(dbus->service(), dbus->path(),
                   "org.freedesktop.DBus.Properties", "PropertiesChanged",
                   this, SLOT(self_propertiesChanged(QString,QVariantMap,QStringList)));
    update();
}

QString UDisks2Drive::toString()
{
    return QString("name: %1\nsize: %2\nvendor: %3\nmodel: %4\nserial: %5\n"
                   "id: %6\nmedia: %7\noptical: %8\nremovable: %9\navailable: %10")
            .arg(name).arg(size).arg(vendor).arg(model).arg(serial).arg(id)
            .arg(media).arg(optical).arg(removable).arg(available);
}

void UDisks2Drive::update()
{
    size = dbus->property("Size").toULongLong();
    vendor = dbus->property("Vendor").toString();
    model = dbus->property("Model").toString();
    serial = dbus->property("Serial").toString();
    id = dbus->property("Id").toString();
    media = dbus->property("Media").toString();
    optical = dbus->property("Optical").toBool();
    removable = dbus->property("MediaRemovable").toBool();
    available = dbus->property("MediaAvailable").toBool();
}

void UDisks2Drive::self_propertiesChanged(const QString &, const QVariantMap &, const QStringList &)
{
    update();
    emit this->changed(name);
}


UDisks2Filesystem::UDisks2Filesystem(const QString &node, QObject *parent)
    : QObject(parent)
{
    name = node;
    QDBusConnection system = QDBusConnection::systemBus();
    dbus = new QDBusInterface("org.freedesktop.UDisks2",
                              "/org/freedesktop/UDisks2/block_devices/" + node,
                              "org.freedesktop.UDisks2.Filesystem",
                              system, parent);
    dbusProp = new QDBusInterface("org.freedesktop.UDisks2",
                              "/org/freedesktop/UDisks2/block_devices/" + node,
                              "org.freedesktop.DBus.Properties",
                              system, parent);
    emit update();
}

QStringList UDisks2Filesystem::mountPoints() const { return mountPoints_; }

QString UDisks2Filesystem::mount()
{
    QDBusMessage reply = dbus->call("Mount", QVariantMap());
    return reply.arguments().first().toString();
}

void UDisks2Filesystem::unmount()
{   dbus->call("Unmount", QVariantMap());

}

void UDisks2Filesystem::update()
{
    mountPoints_.clear();
    QDBusMessage reply = dbusProp->call("Get", "org.freedesktop.UDisks2.Filesystem", "MountPoints");
    if (reply.type() == QDBusMessage::ErrorMessage) {
        // No such interface, the device had no filesystem
        return;
    }
    QVariant v = reply.arguments().first();
    QDBusArgument arg = v.value<QDBusVariant>().variant().value<QDBusArgument>();
    arg.beginArray();
    while (!arg.atEnd()) {
        dbus_ay data;
        arg >> data;
        mountPoints_.append(dbus_ay_toString(data));
    }
}

bool UDisks2Filesystem::isValid()
{
    return dbus->isValid();
}
