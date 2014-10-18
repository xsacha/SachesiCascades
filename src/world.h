#pragma once
// Copyright (C) 2014 Sacha Refshauge

#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <bb/cascades/QListDataModel>
#include <bb/system/SystemPrompt>
#include <bb/system/SystemUiInputMode>
#include <bb/system/SystemToast>
#include <bb/data/JsonDataAccess>
#include <bb/system/InvokeRequest>
#include "worldapps.h"

// TODO: Use cas for the download page. Maybe Search too?
// Need git revision before this happens so we can revert back if things go awry

class AppWorld : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool    listing         READ listing   WRITE setListing    NOTIFY listingChanged)
    Q_PROPERTY(QStringList more        READ more                          NOTIFY moreChanged)
    Q_PROPERTY(int     server          READ serverInt WRITE setServerInt  NOTIFY serverChanged)
    Q_PROPERTY(int     osVer           READ osVer     WRITE setOsVer      NOTIFY osVerChanged)
    Q_PROPERTY(int     model           READ model     WRITE setModel      NOTIFY modelChanged)
    Q_PROPERTY(bb::cascades::DataModel* appList READ appList NOTIFY appListChanged)
    Q_PROPERTY(AppWorldApps* contentItem READ contentItem NOTIFY contentItemChanged)
    Q_PROPERTY(int appCount READ appCount NOTIFY appListChanged)
public:
    explicit AppWorld(QObject *parent = 0)
    : QObject(parent)
    , _listing(false)
    , _more()
    , _server(0)
    , _model(0)
    , _osVer(0)
    {
        _manager = new QNetworkAccessManager();
        _contentItem = new AppWorldApps();
    }

    Q_INVOKABLE QString utf8Encode(QString input) {
        return input.toUtf8();
    }

    Q_SLOT void onInvoke(const bb::system::InvokeRequest& invoke) {
        QString query = invoke.uri().path();
        if (query.isEmpty())
            showHome();
        if (query.startsWith("dl/") || query.startsWith("download/"))
            download(query.split('/').last());
        else if (query.toInt())
            showContentItem(query);
        else
            search(query);
    }

    QString queryOSModelString(QString model, bool osRequired = false) {
        if (model == "PlayBook")
            return "&model=PlayBook&os=2.1.0"; // PlayBook

        int osVer = _osVer;
        if (osVer == 3 && osRequired)
            osVer = 0;
        QString os;
        switch (osVer) {
            case 3:
                os = "";
                break;
            case 2:
                os = "10.1.0";
                break;
            case 1:
                os = "10.2.0";
                break;
            case 0:
            default:
                os = "10.9.0";
                break;
        }
        return QString("&model=%1&os=%2").arg(model).arg(os);
    }

    QString currentServer() {
        switch (_server) {
            case 2:
                return "http://eval.appworld.blackberry.com";
            case 1:
                return "http://enterprise.appworld.blackberry.com";
            case 0:
            default:
                return "http://appworld.blackberry.com";
        }
    }

    QString modelName() {
        switch (_model) {
            case 5:
                return "PlayBook";
            case 4:
                return "Imagination GPU";
            case 3:
                return "Qualcomm GPU";
            case 2:
                return "BlackBerry Z30";
            case 1:
                return "BlackBerry Q10";
            case 0:
            default:
                return "BlackBerry Passport";
        }
    }

    Q_INVOKABLE void download(QString id) {
        // Make sure it is downloadable, even if they chose the wrong phone
        QString model = modelName();
        if (!_supportedDeviceNames.empty() && !_supportedDeviceNames.contains(model))
            model = _supportedDeviceNames.first();
        QDesktopServices::openUrl("appworld://musicdownload/%2E./usfdownload?contentid=" + id + queryOSModelString(model, true));
    }

    void invalidIDToast() {
        bb::system::SystemToast *toast = new bb::system::SystemToast();
        toast->setBody(tr("Invalid ID"));
        toast->show();
        // Otherwise it disappears straight away!
        QTimer::singleShot(1000, toast, SLOT(deleteLater()));
    }

    Q_INVOKABLE void lookup() {
        bb::system::SystemPrompt *prompt = new bb::system::SystemPrompt();
        prompt->setTitle(tr("Enter Application ID"));
        prompt->inputField()->setInputMode(bb::system::SystemUiInputMode::NumericKeypad);
        bool invalid = true;
        while (invalid) {
            invalid = false;
            prompt->exec();
            if (prompt->buttonSelection() == prompt->confirmButton()) {
                QString entry = prompt->inputFieldTextEntry();
                // We convert to integer to make sure we don't get any malicious input
                if (entry.toInt()) {
                    showContentItem(entry);
                } else {
                    invalidIDToast();
                    invalid = true;
                }
            }
        }
        prompt->deleteLater();
    }

    Q_SLOT void appWhitelistResponse() {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        bb::data::JsonDataAccess json;
        const QVariant jsonva = json.loadFromBuffer(reply->readAll());
        const QMap<QString, QVariant> jsonreply = jsonva.toMap();
        QMap<QString, QVariant>::const_iterator it = jsonreply.find("cdDTO");
        if (it != jsonreply.end()) {
            // Print everything in dto array
            const QMap<QString, QVariant> dto = it.value().toMap();
            for (QMap<QString, QVariant>::const_iterator hdrIter = dto.begin(); hdrIter != dto.end(); ++hdrIter) {
                if (hdrIter.key() == "supportedDevices") {
                    _supportedDeviceNames = hdrIter.value().toStringList();
                }
            }
        }
        reply->deleteLater();
    }

    void findAppWhitelist(QString id) {
        QString server = currentServer();
        QNetworkRequest request(QString("%1/cas/content/%2")
                .arg(server)
                .arg(id));
        QNetworkReply* reply = _manager->get(request);
        QObject::connect(reply, SIGNAL(finished()), this, SLOT(appWhitelistResponse()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), reply, SLOT(deleteLater()));
    }

    Q_SLOT void searchLockerResponse() {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        QString server = currentServer();
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (reply->size() > 0 && (status == 200 || (status > 300 && status <= 308))) {
            QXmlStreamReader xml(reply->readAll());
            foreach(AppWorldApps* app, _appList)
            app->deleteLater();
            _appList.clear();
            _more.clear(); emit moreChanged();
            emit appListChanged();
            QString imageDepot = server + "/ClientAPI/image";
            while(!xml.atEnd() && !xml.hasError()) {
                if(xml.tokenType() == QXmlStreamReader::StartElement) {
                    if (xml.name() == "featured") {
                        imageDepot = xml.attributes().value("imagedepot").toString();
                    }
                    else if (xml.name() == "content") {
                        AppWorldApps* app = new AppWorldApps();
                        app->setId(xml.attributes().value("id").toString());
                        app->setImage(imageDepot + "/" + xml.attributes().value("cover").toString());
                        while(!xml.atEnd() && !xml.hasError()) {
                            xml.readNext();
                            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                                if (xml.name() == "product") {
                                    app->setType(xml.attributes().value("name").toString());
                                    //app->setTypeImage(imageDepot + "/" + xml.attributes().value("icon").toString());
                                }
                                else if (xml.name() == "image" && xml.attributes().value("imagetype") == "1")
                                    app->setImage(xml.attributes().value("src").toString());
                                else if (xml.name() == "name")
                                    app->setFriendlyName(xml.readElementText());
                                else if (xml.name() == "vendor")
                                    app->setVendor(xml.readElementText());
                            }
                            if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "content")
                                break;
                        }
                        _appList.append(app);
                    }
                }
                xml.readNext();
            }
        }
        emit appListChanged();
        _listing = true; emit listingChanged();
        reply->deleteLater();
    }

    Q_INVOKABLE void searchLocker(QString type) {
        QString server = currentServer();
        QNetworkRequest request(QString("%1/ClientAPI/%2?" + queryOSModelString(modelName()))
                .arg(server)
                .arg(type));
        request.setRawHeader("User-Agent", "AppWorld/5.1.0.60");
        QNetworkReply* reply = _manager->get(request);
        QObject::connect(reply, SIGNAL(finished()), this, SLOT(searchLockerResponse()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), reply, SLOT(deleteLater()));
    }

    Q_INVOKABLE void search(QString query) {
        QString server = currentServer();
        searchRequest(QString("%1/ClientAPI/searchlist?s=%2" + queryOSModelString(modelName()))
                .arg(server)
                .arg(query));
    }

    Q_INVOKABLE void showVendor(QString id) {
        QString server = currentServer();
        searchRequest(QString("%1/ClientAPI/vendorlistforappsgames/%2?" + queryOSModelString(modelName()))
                .arg(server)
                .arg(id));
    }

    Q_INVOKABLE void showCars() {
        QString server = currentServer();
        searchRequest(QString("%1/ClientAPI/autostorepages/?" + queryOSModelString(modelName(), true))
                .arg(server));
    }

    Q_INVOKABLE void showHome() {
        QString server = currentServer();
        searchRequest(QString("%1/ClientAPI/usfpage/?" + queryOSModelString(modelName(), true))
                .arg(server));
    }

    Q_INVOKABLE void searchMore(QString url, bool os_required = false) {
        searchRequest(url + queryOSModelString(modelName(), os_required));
    }

    Q_SLOT void searchRequestResponse() {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (reply->size() > 0 && (status == 200 || (status > 300 && status <= 308))) {
            QXmlStreamReader xml(reply->readAll());
            QString curType;
            foreach (AppWorldApps* app, _appList) app->deleteLater();
            _appList.clear();
            _more.clear(); emit moreChanged();
            while(!xml.atEnd() && !xml.hasError()) {
                if(xml.tokenType() == QXmlStreamReader::StartElement) {
                    if (xml.name() == "panel") {
                        curType = xml.attributes().value("displayname").toString(); // Eg. Apps, Games
                    } else if (xml.name() == "link") {
                        int linkType = xml.attributes().value("linktype").toString().toInt();
                        if (linkType == 3
                                || (linkType == 1 && xml.attributes().value("onactionbar").toString() == "false")
                                || (linkType == 2 && xml.attributes().value("showicon").toString() == "true")) {
                            AppWorldApps* app = new AppWorldApps();
                            if (linkType == 1 || linkType == 2) {
                                app->setType("category");
                                app->setId(xml.attributes().value("url").toString());
                            }
                            else {
                                // Check if this ID already exists and then skip
                                bool exists = false;
                                foreach (AppWorldApps* appobj, _appList) {
                                    if (appobj->id() == xml.attributes().value("id").toString())
                                        exists = true;
                                }
                                if (exists) {
                                    delete app;
                                    xml.readNext();
                                    continue;
                                }
                                app->setType(curType);
                                app->setName(xml.attributes().value("name").toString());
                                app->setId(xml.attributes().value("id").toString());
                            }
                            app->setFriendlyName(xml.attributes().value("displayname").toString());
                            while(!xml.atEnd() && !xml.hasError()) {
                                xml.readNext();
                                if (xml.tokenType() == QXmlStreamReader::StartElement) {
                                    if (xml.name() == "image"
                                            && ((xml.attributes().value("imagetype") == "1")
                                                    || (xml.attributes().value("imagetype") == "7")))
                                        app->setImage(xml.attributes().value("src").toString());
                                    else if (xml.name() == "vendor") {
                                        app->setVendorId(xml.attributes().value("id").toString());
                                        app->setVendor(xml.readElementText());
                                    } else if (xml.name() == "autocheck") {
                                        app->setFriendlyName(xml.attributes().value("make").toString());
                                    }
                                }
                                if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "link")
                                    break;
                            }
                            _appList.append(app);
                        } else if (linkType == 2 || linkType == 1) {
                            _more.append(xml.attributes().value("displayname").toString() + "," + xml.attributes().value("url").toString()); emit moreChanged();
                        } else if (linkType == 13) {
                            // This is either used for next page (no name) or a specific task like Search
                            if (!xml.attributes().hasAttribute("displayname"))
                                _more.append("Next Page," + xml.attributes().value("url").toString()); emit moreChanged();
                        }
                    }
                }
                xml.readNext();
            }
        }
        emit appListChanged();
        _listing = true; emit listingChanged();
        reply->deleteLater();
    }

    void searchRequest(QString requestString) {
        QNetworkRequest request(requestString);
        request.setRawHeader("User-Agent", "AppWorld/5.1.0.60");
        QNetworkReply* reply = _manager->get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(searchRequestResponse()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), reply, SLOT(deleteLater()));
    }

    Q_SLOT void showContentItemResponse() {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        QString server = currentServer();
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (reply->size() > 0 && (status == 200 || (status > 300 && status <= 308))) {
            QXmlStreamReader xml(reply->readAll());
            QString imageDepot = server + "/ClientAPI/image";
            _contentItem->deleteLater();
            _contentItem = new AppWorldApps();
            while(!xml.atEnd() && !xml.hasError()) {
                if(xml.tokenType() == QXmlStreamReader::StartElement) {
                    if (xml.name() == "content") {
                        imageDepot = xml.attributes().value("imagedepot").toString();
                        _contentItem->setDescription(QString("This application was created %1 and last updated %2.\n\n")
                                .arg(QDateTime::fromMSecsSinceEpoch(xml.attributes().value("createddate").toString().toLongLong()).toString())
                                .arg(QDateTime::fromMSecsSinceEpoch(xml.attributes().value("forsaledate").toString().toLongLong()).toString()));
                        _contentItem->setImage(imageDepot + "/" + xml.attributes().value("icon").toString());
                        _contentItem->setId(xml.attributes().value("id").toString());
                    } else if (xml.name() == "name" && _contentItem->friendlyName() == "") {
                        _contentItem->setFriendlyName(xml.readElementText());
                    } else if (xml.name() == "product") {
                        _contentItem->setType(xml.attributes().value("name").toString());
                    } else if (xml.name() == "packageid") {
                        _contentItem->setPackageId(xml.readElementText());
                    } else if (xml.name() == "packagename") {
                        _contentItem->setName(xml.readElementText());
                    } else if (xml.name() == "description") {
                        _contentItem->setDescription(_contentItem->description() + xml.readElementText());
                    } else if (xml.name() == "screenshot") {
                        _contentItem->_screenshots.append(imageDepot + "/" + xml.attributes().value("id").toString());
                        _contentItem->screenshotsDidChange();
                    } else if (xml.name() == "vendor") {
                        _contentItem->setVendorId(xml.attributes().value("id").toString());
                        _contentItem->setVendor(xml.readElementText());
                    } else if (xml.name() == "filebundle") {
                        _contentItem->setVersion(xml.attributes().value("version").toString());
                        _contentItem->setSize(xml.attributes().value("size").toString().toInt());
                        _contentItem->setFileId(xml.attributes().value("id").toString());
                    } else if (xml.name() == "price" && _contentItem->price() == "") {
                        _contentItem->setPrice(xml.readElementText());
                    } else if (xml.name() == "releasenotes") {
                        _contentItem->setDescription(_contentItem->description() + "\n\nRelease Notes (" + _contentItem->version() + ")\n" + xml.readElementText());
                    }
                }
                xml.readNext();
            }
            emit contentItemChanged();
            _listing = false; emit listingChanged();
        } else
            invalidIDToast();

        reply->deleteLater();
    }

    Q_INVOKABLE void showContentItem(QString item) {
        // Handle whitelists
        _supportedDeviceNames.clear();
        findAppWhitelist(item);
        // Check item
        QString server = currentServer();

        QNetworkRequest request(QString("%1/ClientAPI/content/%2?" + queryOSModelString(modelName()))
                .arg(server)
                .arg(item));
        request.setRawHeader("User-Agent", "AppWorld/5.1.0.60");
        QNetworkReply* reply = _manager->get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(showContentItemResponse()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), reply, SLOT(deleteLater()));
    }

    QStringList more() const { return _more; }
    bool listing() const { return _listing; }
    int serverInt() const { return _server; }
    int osVer() const { return _osVer; }
    int model() const { return _model; }
    void setListing(bool listing) { _listing = listing; emit listingChanged(); }
    void setServerInt(int server) { _server = server; emit serverChanged(); }
    void setOsVer(int osVer) { _osVer = osVer; emit osVerChanged(); }
    void setModel(int model) { _model = model; emit modelChanged(); }
    AppWorldApps* contentItem() const { return _contentItem; }
    bb::cascades::DataModel* appList() {
        return new bb::cascades::QListDataModel<AppWorldApps*>(_appList);
    }

    int appCount() const { return _appList.count(); }

    signals:
    void listingChanged();
    void moreChanged();
    void serverChanged();
    void osVerChanged();
    void modelChanged();
    void appListChanged();
    void contentItemChanged();

private:
    bool _listing;
    QStringList _more;
    int _server;
    int _model;
    int _osVer;

    QList<AppWorldApps*> _appList;
    AppWorldApps* _contentItem;
    QNetworkAccessManager* _manager;
    QStringList _supportedDeviceNames;
};

