#pragma once

#include <QString>
#include <QStringList>
#if QT_VERSION > QT_VERSION_CHECK(5, 0, 0)
#include <QQmlListProperty>
#else
#include <QDeclarativeListProperty>
#define QQmlListProperty QDeclarativeListProperty
#endif
#include <bb/cascades/DataModel>
#include <bb/cascades/QListDataModel>

class UpdateBundles: public QObject {
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString device READ device WRITE setDevice NOTIFY deviceChanged)
    Q_PROPERTY(int deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(int variantId READ variantId WRITE setVariantId NOTIFY variantIdChanged)
public:
    UpdateBundles(QObject *parent = 0)
    : QObject(parent)
    , _name(""), _device(""), _deviceId(0), _variantId(0) {}

    // For Qt4 we expect device input is Device Name.devid.varid
    UpdateBundles(QString name, QString device, QObject *parent = 0)
    : QObject(parent)
    , _name(name) {
        QStringList components = device.split('+');
        setDevice(components.at(0));
        setDeviceId(components.at(1).toInt());
        setVariantId(components.at(2).toInt());
    }
    QString name() const { return _name; }
    QString device() const { return _device; }
    int deviceId() const { return _deviceId; }
    int variantId() const { return _variantId; }

    void setName(QString name) { _name = name; emit nameChanged(); }
    void setDevice(QString device) { _device = device; emit deviceChanged(); }
    void setDeviceId(int deviceId) { _deviceId = deviceId; emit deviceIdChanged(); }
    void setVariantId(int variantId) { _variantId = variantId; emit variantIdChanged(); }

signals:
    void nameChanged();
    void deviceChanged();
    void deviceIdChanged();
    void variantIdChanged();
private:
    QString _name;
    QString _device;
    int _deviceId;
    int _variantId;
};

Q_DECLARE_METATYPE(UpdateBundles* );
