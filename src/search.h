#pragma once

#include <QtNetwork>
#include <QObject>
#include <bb/cascades/DataModel>

#include "updatebundles.h"
#include "worldapps.h"

enum SearchType {
    SearchIdle,
    SearchBundles,
    SearchUpdate,
    SearchScanner,
};

class Search : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString updateMessage READ updateMessage NOTIFY updateMessageChanged)
    Q_PROPERTY(QString softwareRelease READ softwareRelease NOTIFY softwareReleaseChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString multiscanVersion READ multiscanVersion NOTIFY updateMessageChanged)
    Q_PROPERTY(int     hasPotentialLinks READ hasPotentialLinks NOTIFY hasPotentialLinksChanged)
    Q_PROPERTY(bool    multiscan READ multiscan WRITE setMultiscan NOTIFY multiscanChanged)
    Q_PROPERTY(int     scanning READ scanning WRITE setScanning NOTIFY scanningChanged)
    Q_PROPERTY(int     autoscan READ autoscan WRITE setAutoscan NOTIFY autoscanChanged)
    Q_PROPERTY(int     maxId READ maxId NOTIFY maxIdChanged)
    Q_PROPERTY(int     searchMode READ searchMode WRITE setSearchMode NOTIFY searchModeChanged)
    Q_PROPERTY(bb::cascades::DataModel* appList READ appList NOTIFY updateMessageChanged)
    Q_PROPERTY(bb::cascades::DataModel* bundleList READ bundleList NOTIFY bundleListChanged)

public:
    Search(QObject* parent = 0);
    Q_INVOKABLE void reverseLookup(QString OSver, bool skip);
    Q_INVOKABLE void generatePotentialLinks(QString osVersion);
    Q_INVOKABLE void availableBundleRequest(int country, int carrier, int device);
    Q_INVOKABLE void fromBundleRequest(int country, int carrier, int device, int variant, QString OSver);
    Q_INVOKABLE void updateDetailRequest(int carrier, int country, int device, int variant = -1, QString OSver = "");
    Q_INVOKABLE void grabLinks();

    Q_INVOKABLE QString nameFromVariant(unsigned int device, unsigned int variant);
    Q_INVOKABLE QString hwidFromVariant(unsigned int device, unsigned int variant);
    Q_INVOKABLE unsigned int variantCount(unsigned int device);
    void writeDisplayFile(QString type, QString writeText);

    void    setMultiscan(const bool &multiscan);
    void    setScanning(const int &scanning);
    void    setAutoscan(const int &autoscan);
    void    setSearchMode(const int &searchMode) { _searchMode = searchMode; emit searchModeChanged(); }

    QString updateMessage() const { return _updateMessage; }
    QString softwareRelease() const { return _softwareRelease; }
    QString error() const { return _error; }
    QString multiscanVersion() const { return _multiscanVersion; }
    bool multiscan() const { return _multiscan; }
    int scanning() const { return _scanning; }
    int hasPotentialLinks() const { return _hasPotentialLinks; }
    int autoscan() const { return _autoscan; }
    int maxId() const { return _maxId; }
    int searchMode() const { return _searchMode; }
    bb::cascades::DataModel* appList() {
        return new bb::cascades::QListDataModel<AppWorldApps*>(_updateAppList);
    }
    bb::cascades::DataModel* bundleList() {
            return new bb::cascades::QListDataModel<UpdateBundles*>(_updateBundlesList);
        }

signals:
    void softwareReleaseChanged();
    void updateMessageChanged();
    void errorChanged();
    void multiscanChanged();
    void hasPotentialLinksChanged();
    void scanningChanged();
    void autoscanChanged();
    void maxIdChanged();
    void searchModeChanged();
    void appListChanged();
    void bundleListChanged();

private slots:
    void validateDownload();
    void newSRVersion();
    void bundleReply();
    void serverReply();
    void fromBundleToUpdateReply();
    void showFirmwareData(QByteArray data, QString variant);
    void serverError(QNetworkReply::NetworkError error);

    private:
    QString NPCFromLocale(int country, int carrier);

    QNetworkAccessManager *manager;
    QList<AppWorldApps*> _updateAppList;
    QList<UpdateBundles*> _updateBundlesList;
    QString _updateMessage;
    QString _softwareRelease;
    QString _versionRelease;
    QString _error;
    QString _multiscanVersion;
    bool _multiscan;
    int _hasPotentialLinks;
    int _scanning, _autoscan;
    int _maxId, _dlBytes, _dlTotal;
    int _options;
    int _type;
    int _searchMode;
    SearchType _searchType;
};
