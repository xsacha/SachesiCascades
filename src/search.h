#pragma once

#include <QtNetwork>
#include <QObject>
#include <bb/cascades/DataModel>

#include "worldapps.h"

class Search : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString updateMessage READ updateMessage NOTIFY updateMessageChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString multiscanVersion READ multiscanVersion NOTIFY updateMessageChanged)
    Q_PROPERTY(bool    multiscan READ multiscan WRITE setMultiscan NOTIFY multiscanChanged)
    Q_PROPERTY(int     scanning READ scanning WRITE setScanning NOTIFY scanningChanged)
    Q_PROPERTY(int     maxId READ maxId NOTIFY maxIdChanged)
    Q_PROPERTY(bb::cascades::DataModel* appList READ appList NOTIFY updateMessageChanged)

public:
    Search(QObject* parent = 0);
    Q_INVOKABLE void updateDetailRequest(int carrier, int country, int device);
    Q_INVOKABLE void grabLinks();

    Q_INVOKABLE QString nameFromVariant(unsigned int device, unsigned int variant);
    Q_INVOKABLE QString hwidFromVariant(unsigned int device, unsigned int variant);
    Q_INVOKABLE unsigned int variantCount(unsigned int device);

    void    setMultiscan(const bool &multiscan);
    void    setScanning(const int &scanning);

    QString updateMessage() const { return _updateMessage; }
    QString error() const { return _error; }
    QString multiscanVersion() const { return _multiscanVersion; }
    bool multiscan() const { return _multiscan; }
    int scanning() const { return _scanning; }
    int maxId() const { return _maxId; }
    bb::cascades::DataModel* appList() {
        return new bb::cascades::QListDataModel<AppWorldApps*>(_updateAppList);
    }

    signals:
    void softwareReleaseChanged();
    void updateMessageChanged();
    void errorChanged();
    void multiscanChanged();
    void scanningChanged();
    void hasPotentialLinksChanged();
    void maxIdChanged();
    void appListChanged();
    private slots:
    void serverReply();
    void showFirmwareData(QByteArray data, QString variant);
    void serverError(QNetworkReply::NetworkError error);

    private:
    QString NPCFromLocale(int country, int carrier);

    QNetworkAccessManager *manager;
    QList<AppWorldApps*> _updateAppList;
    QString _updateMessage;
    QString _softwareRelease;
    QString _versionRelease;
    QString _error;
    QString _multiscanVersion;
    bool _multiscan;
    int _scanning;
    int _maxId, _dlBytes, _dlTotal;
    int _options;
    int _type;
};
