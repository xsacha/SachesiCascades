#include "search.h"
#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>

// TODO: Connect Bundles to Results
//       Create Scanner

Search::Search(QObject* parent)
: QObject(parent)
, _updateMessage("")
, _softwareRelease("")
, _versionRelease("")
, _scanning(0)
{
    manager = new QNetworkAccessManager();
}

void Search::grabLinks() {
    QString updated;
    foreach (AppWorldApps* app, _updateAppList) {
        updated.append(app->packageId() + "\n");
    }

    QString dir = "shared/misc/Sachesi/";
    QDir(dir).mkpath(".");
    QString name = dir + tr("Links") + ".txt";
    QFile links(name);
    if (!links.open(QIODevice::WriteOnly)) {
        bb::system::SystemDialog* dialog = new bb::system::SystemDialog();
        dialog->setTitle(tr("Unable to display ") + tr("Links"));
        dialog->setBody(tr("The permission 'Shared Files' is required to create and display the links. Please enable this permission, restart the application and try again."));
        dialog->exec();
        if (dialog->buttonSelection() == dialog->confirmButton()) {
            QDesktopServices::openUrl(QUrl("settings://permissions"));
        }
        dialog->deleteLater();
        return;
    }
    links.write(updated.toLocal8Bit());
    links.close();
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString("/accounts/1000/") + name));
}

QString Search::NPCFromLocale(int carrier, int country) {
    QString homeNPC;
    homeNPC.sprintf("%03d%03d%d", carrier, country, carrier ? 30 : 60);
    return homeNPC;
}

static QStringList dev[] = {
        // 0 = Z30 (A Series) + Classic
        QStringList() << "STA 100-1" << "STA 100-2" << "STA 100-3" << "STA 100-4" << "STA 100-5" << "STA 100-6" << "Classic AT/T" << "Classic Verizon" << "Classic ROW" << " Classic NA",
        QStringList() << "8C00240A" << "8D00240A" << "8E00240A" << "8F00240A" << "9500240A" << "B500240A" << "9400270A" << "9500270A" << "9600270A" << "9700270A",
        // 1 = Z10 (L Series) OMAP
        QStringList() << "STL 100-1",
        QStringList() << "4002607",
        // 2 = Z10 (L Series) Qualcomm + P9982  (TK Series)
        QStringList() << "STL 100-2" << "STL 100-3" << "STL 100-4" << "STK 100-1" << "STK 100-2",
        QStringList() << "8700240A" << "8500240A" << "8400240A" << "A500240A" << "A600240A",
        // 3 = Z3  (J Series) + Cafe
        QStringList() << "STJ 100-1" << "Cafe NA" << "Cafe Europe/ME/Asia" << "Cafe ROW" << "Cafe AT/T" << "Cafe LatinAm" << "Cafe Verizon",
        QStringList() << "04002E07" << "87002A07" << "8C002A07" << "9600240A" << "9700240A" << "9C00240A" << "A700240A",
        // 4 = Passport / Q30 (W Series)
        QStringList() << "SQW 100-1" << "SQW 100-2" << "SQW 100-3" << "SQW 100-4" << "Passport Wichita",
        QStringList() << "87002C0A" << "85002C0A" << "84002C0A" << "86002C0A" << "8C002C0A",
        // 5 = Q5 (R Series) + Q10 (N Series) + P9983 (QK Series)
        QStringList() << "SQR 100-1" << "SQR 100-2" << "SQR 100-3" << "SQN 100-1" << "SQN 100-2" << "SQN 100-3" << "SQN 100-4" << "SQN 100-5" << "SQK 100-1" << "SQK 100-2",
        QStringList() << "84002A0A" << "85002A0A" << "86002A0A" << "8400270A" << "8500270A" << "8600270A" << "8C00270A" << "8700270A"  << "8F00270A" << "8E00270A",
        // 6 = Dev Alpha
        QStringList() << "Alpha A" << "Alpha B" << "Alpha C",
        QStringList() << "4002307" << "4002607" << "8D00270A",
        // 7 = Ontario Series
        QStringList() << "Ontario NA" << "Ontario Verizon" << "Ontario Sprint" << "Ontario ROW" << "China",
        QStringList() << "AE00240A" << "AF00240A" << "B400240A" << "B600240A" << "BC00240A",
};

QString Search::nameFromVariant(unsigned int device, unsigned int variant) {
    Q_ASSERT(variantCount(device) > variant);
    return dev[device*2][variant];
}
QString Search::hwidFromVariant(unsigned int device, unsigned int variant) {
    Q_ASSERT(variantCount(device) > variant);
    return dev[device*2+1][variant];
}
unsigned int Search::variantCount(unsigned int device) {
    return dev[device*2].count();
}

void Search::availableBundleRequest(int country, int carrier, int device)
{
    QString requestUrl = "https://cs.sl.blackberry.com/cse/availableBundles/1.0.0/";
    QString homeNPC = NPCFromLocale(carrier, country);
    // We select 'Any' always!
    int start = 0;
    int end   = variantCount(device);
    setScanning(variantCount(device));
    if (_scanning) {
        _searchType = SearchBundles;
        _error = ""; emit errorChanged();
        _updateMessage = ""; emit updateMessageChanged();
        if (_scanning > 1)
            setMultiscan(true);
    }
    // Clean this up. Memory is an issue for BfB
    if (_updateBundlesList.count()) {
        foreach(UpdateBundles* bundle, _updateBundlesList) {
            bundle->deleteLater();
        }
        _updateBundlesList.clear();
        emit bundleListChanged();
    }
    for (int i = start; i < end; i++) {
        QString query = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<availableBundlesRequest version=\"1.0.0\" authEchoTS=\"%1\">"
                "<deviceId><pin>0x24F1C502</pin></deviceId>"
                "<clientProperties>"
                "<hardware><id>0x%2</id><isBootROMSecure>true</isBootROMSecure></hardware>"
                "<network><vendorId>0x0</vendorId><homeNPC>0x%3</homeNPC><currentNPC>0x%3</currentNPC></network>"
                "<software><currentLocale>en_US</currentLocale><legalLocale>en_US</legalLocale><osVersion>10.0.0.0</osVersion><radioVersion>10.0.0.0</radioVersion></software>"
                "</clientProperties>"
                "<updateDirectives><bundleVersionFilter></bundleVersionFilter></updateDirectives>"
                "</availableBundlesRequest>")
                        .arg(QDateTime::currentMSecsSinceEpoch()) // Current time, in case it cares one day
                        .arg(hwidFromVariant(device, i)) // Search Device HWID
                        .arg(homeNPC); // Country + Carrier

        // Pass the variant in the request so it can be retrieved out-of-order
        QNetworkRequest request;
        request.setRawHeader("Content-Type", "text/xml;charset=UTF-8");
        request.setUrl(QUrl(requestUrl));
        request.setAttribute(QNetworkRequest::CustomVerbAttribute, nameFromVariant(device, i) + "+" + QString::number(device) + "+" + QString::number(i));
        QNetworkReply* reply = manager->post(request, query.toUtf8());
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(serverError(QNetworkReply::NetworkError)));
        connect(reply, SIGNAL(finished()), this, SLOT(bundleReply()));
    }
}

void Search::bundleReply() {
    QNetworkReply *reply = (QNetworkReply *)sender();
    if (reply->size() > 0) {
        QString device = reply->request().attribute(QNetworkRequest::CustomVerbAttribute).toString();
        QByteArray data = reply->readAll();
        QXmlStreamReader xml(data);
        while(!xml.atEnd() && !xml.hasError()) {
            if(xml.tokenType() == QXmlStreamReader::StartElement) {
                if (xml.name() == "bundle" && xml.attributes().hasAttribute("version")) {
                    QString name = xml.attributes().value("version").toString();
                    bool exists = false;
                    foreach (UpdateBundles* bundle, _updateBundlesList) {
                        if (bundle->name() == name) {
                            exists = true;
                            break;
                        }
                    }
                    if (!exists) {
                        _updateBundlesList.append(new UpdateBundles(name, device));
                        //emit bundleListChanged();
                    }
                }
            }
            xml.readNext();
        }
    }
    setScanning(_scanning - 1);
    reply->deleteLater();
}

void Search::fromBundleRequest(int country, int carrier, int device, int variant, QString OSver)
{
    QString requestUrl = "https://cs.sl.blackberry.com/cse/srVersionLookup/2.0/";
    QString query = QString("<srVersionLookupRequest version=\"2.0.0\" authEchoTS=\"%1\">"
            "<clientProperties>"
            "<hardware><pin>0x2FFFFFB3</pin><bsn>1140011878</bsn><id>0x%2</id></hardware>"
            "<software><osVersion>%3</osVersion></software>"
            "</clientProperties>"
            "</srVersionLookupRequest>")
                        .arg(QDateTime::currentMSecsSinceEpoch())
                        .arg(hwidFromVariant(device, variant))
                        .arg(OSver);
    QNetworkRequest request;
    request.setRawHeader("Content-Type", "text/xml;charset=UTF-8");
    request.setUrl(QUrl(requestUrl));
    // Because we use Qt4, this is easier
    request.setAttribute(QNetworkRequest::CustomVerbAttribute, QString::number(country) + "+" + QString::number(carrier) + "+" + QString::number(device) + "+" + QString::number(variant));
    QNetworkReply* reply = manager->post(request, query.toUtf8());
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(serverError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(fromBundleToUpdateReply()));

}

void Search::fromBundleToUpdateReply() {
    QNetworkReply *reply = (QNetworkReply *)sender();
    QByteArray data = reply->readAll();
    QXmlStreamReader xml(data);
    QString swRelease = "";
    while(!xml.atEnd() && !xml.hasError()) {
        if(xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == "softwareReleaseVersion") {
                swRelease = xml.readElementText();
            }
        }
        xml.readNext();
    }
    if (swRelease == "")
        setScanning(_scanning - 1);
    else {
        QStringList components = reply->request().attribute(QNetworkRequest::CustomVerbAttribute).toString().split('+');
        updateDetailRequest(components.at(0).toInt(), components.at(1).toInt(), components.at(2).toInt(), components.at(3).toInt(), swRelease);
    }

    reply->deleteLater();
}

void Search::updateDetailRequest(int country, int carrier, int device, int variant, QString SRver)
{
    QString up = "upgrade";
    QString requestUrl = "https://cs.sl.blackberry.com/cse/updateDetails/2.2/";
    QString version = "2.2.1";

    QString homeNPC = NPCFromLocale(carrier, country);

    // We select 'Any' always!
    int start = (variant == -1) ? 0 : variant;
    int end   = (variant == -1) ? variantCount(device) : variant + 1;
    setScanning(end - start);
    if (_scanning) {
        _searchType = SearchUpdate;
        _error = ""; emit errorChanged();
        _updateMessage = ""; emit updateMessageChanged();
        if (_scanning > 1)
            setMultiscan(true);
    }
    for (int i = start; i < end; i++) {
        QString query = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<updateDetailRequest version=\"%1\" authEchoTS=\"%2\">"
                "<clientProperties>"
                "<hardware>"
                "<pin>0x2FFFFFB3</pin><bsn>1128121361</bsn><imei>004401139269240</imei><id>0x%3</id>"
                "</hardware>"
                "<network>"
                "<homeNPC>0x%4</homeNPC><iccid>89014104255505565333</iccid>"
                "</network>"
                "<software>"
                "<currentLocale>en_US</currentLocale><legalLocale>en_US</legalLocale>"
                "</software>"
                "</clientProperties>"
                "<updateDirectives><allowPatching type=\"REDBEND\">true</allowPatching><upgradeMode>%5</upgradeMode><provideDescriptions>false</provideDescriptions><provideFiles>true</provideFiles><queryType>NOTIFICATION_CHECK</queryType></updateDirectives>"
                "<pollType>manual</pollType>"
                "<resultPackageSetCriteria>"
                "<softwareRelease softwareReleaseVersion=\"%6\" />"
                "<releaseIndependent><packageType operation=\"include\">application</packageType></releaseIndependent>"
                "</resultPackageSetCriteria>"
                "</updateDetailRequest>")
                                .arg(version) // API Version
                                .arg(QDateTime::currentMSecsSinceEpoch()) // Current time, in case it cares one day
                                .arg(hwidFromVariant(device, i)) // Search Device HWID
                                .arg(homeNPC) // Country + Carrier
                                .arg(up) // Upgrade or Repair?
                                // 2.3.0 doesn't support 'latest'. Does this mean we need to do availableBundles lookup first?
                                .arg(SRver != "" ? SRver : "latest");

        // Pass the variant in the request so it can be retrieved out-of-order
        QNetworkRequest request;
        request.setRawHeader("Content-Type", "text/xml;charset=UTF-8");
        request.setUrl(QUrl(requestUrl));
        request.setAttribute(QNetworkRequest::CustomVerbAttribute, nameFromVariant(device, i));
        QNetworkReply* reply = manager->post(request, query.toUtf8());
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(serverError(QNetworkReply::NetworkError)));
        connect(reply, SIGNAL(finished()), this, SLOT(serverReply()));
    }
}

void Search::serverReply()
{
    QNetworkReply *reply = (QNetworkReply *)sender();
    QByteArray data = reply->readAll();
    //for (int i = 0; i < data.size(); i += 3000) qDebug() << data.mid(i, 3000);
    showFirmwareData(data, reply->request().attribute(QNetworkRequest::CustomVerbAttribute).toString());
    setScanning(_scanning-1);
    reply->deleteLater();
}

void Search::showFirmwareData(QByteArray data, QString variant)
{
    QXmlStreamReader xml(data);
    QString ver = "";
    QString os = "";
    QString radio = "";
    QString addr = "";
    QString currentaddr = "";
    QList<AppWorldApps*> newApps;
    while(!xml.atEnd() && !xml.hasError()) {
        if(xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == "package")
            {
                AppWorldApps* newApp = new AppWorldApps();

                // Remember: this name *can* change
                newApp->setFriendlyName(xml.attributes().value("name").toString());
                // Remember: this name *can* change
                newApp->setName(xml.attributes().value("path").toString());
                // Remember: this size *can* change
                newApp->setSize(xml.attributes().value("downloadSize").toString().toInt());
                newApp->setVersion(xml.attributes().value("version").toString());
                newApp->setVersionId(xml.attributes().value("id").toString());
                newApp->setChecksum(xml.attributes().value("checksum").toString());
                QString type = xml.attributes().value("type").toString();
                if (type == "system:os" || type == "system:desktop") {
                    os = newApp->name().split('/').at(1);
                    newApp->setType("os");
                    newApp->setIsMarked(true);
                    newApp->setIsAvailable(true);
                } else if (type == "system:radio") {
                    radio = newApp->name().split('/').at(1);
                    newApp->setType("radio");
                    newApp->setIsMarked(true);
                    newApp->setIsAvailable(true);
                } else {
                    newApp->setType("application");
                }
                newApp->setPackageId(currentaddr + "/" + newApp->name());
                newApp->setName(newApp->name().split("/").last());

                newApps.append(newApp);
            }
            else if (xml.name() == "friendlyMessage")
            {
                _error = xml.readElementText().split(QChar('.'))[0]; emit errorChanged();
            }
            else if (xml.name() == "fileSet")
            {
                currentaddr = xml.attributes().value("url").toString();
                if (addr == "")
                    addr = currentaddr;
            }
            else if (xml.name() == "softwareReleaseMetadata")
            {
                ver = xml.attributes().value("softwareReleaseVersion").toString();
            }
            else if (xml.name() == "bundle")
            {
                QString newver = xml.attributes().value("version").toString();
                if (ver == "" || (ver.split(".").last().toInt() < newver.split(".").last().toInt()))
                    ver = newver;
            }
            // No longer check for descriptions
            /*else if (xml.name() == "description" && desc == "")
            {
                desc = xml.readElementText();
            }*/
        }
        xml.readNext();
    }
    // Check if the version string is newer.
    bool isNewer = !_multiscan || _multiscanVersion == "";
    if (!isNewer && ver != "") {
        QStringList newVer = ver.split('.');
        QStringList oldVer = _multiscanVersion.split('.');
        for (int i = 0; i < qMin(newVer.count(), oldVer.count()); i++) {
            int newBuild = newVer.at(i).toInt();
            int oldBuild = oldVer.at(i).toInt();
            if (newBuild > oldBuild)
                isNewer = true;
            if (newBuild != oldBuild)
                break;
        }
    }
    if (isNewer) {
        // Update software release versions
        if (_multiscan)
            _multiscanVersion = ver;
        _versionRelease = ver;
        if (ver == "") {
            _updateMessage = "";
        } else {
            // Delete old list
            if (_updateAppList.count()) {
                foreach (AppWorldApps* app, _updateAppList) {
                    app->disconnect(SIGNAL(isMarkedChanged()));
                    app->deleteLater();
                }
                _updateAppList.clear();
            }
            // Put this new list up for display
            _updateAppList = newApps;

            _updateMessage = QString(tr("Update %1 (%2 | %3) is available for %4!"))
                                            .arg(ver)
                                            .arg(os)
                                            .arg(radio)
                                            .arg(variant);

            _error = ""; emit errorChanged();
        }
        // TODO: Putting it here makes user see the values changed. However, putting it below crashes
        //emit updateMessageChanged();
    }
}

void Search::serverError(QNetworkReply::NetworkError err)
{
    // Only show error if we are doing single scan or multiscan version is empty.
    if (!_multiscan || (_multiscanVersion == "" && _scanning == 1)) {
        QString errormsg = QString(tr("Error") + " %1 (%2)")
                                                                                                                                                                                                                .arg(err)
                                                                                                                                                                                                                .arg( ((QNetworkReply*)sender())->errorString() );
        _error = errormsg;
        emit errorChanged();
        _updateMessage = ""; emit updateMessageChanged();
    }
    setScanning(_scanning-1);
}

void Search::setMultiscan(const bool &multiscan) {
    _multiscan = multiscan; emit multiscanChanged();
    _multiscanVersion = ""; emit updateMessageChanged();
}

bool compareBundles(UpdateBundles* i, UpdateBundles* j)
{
    QStringList iList = i->name().split('.');
    QStringList jList = j->name().split('.');
    int max = qMin(iList.count(), jList.count());

    for (int c = 0; c < max; c++) {
        int iVal = iList.at(c).toInt();
        int jVal = jList.at(c).toInt();
        if (iVal != jVal)
            return iVal > jVal;
    }
    return false;
}
bool compareUpdate(AppWorldApps* i, AppWorldApps* j)
{
    if (i->type() != "application" && j->type() == "application")
        return true;
    if (j->type() != "application" && i->type() == "application")
        return false;
    /*if (i->isMarked() != j->isMarked())
        return i->isMarked();*/

    return (i->size() > j->size());
}
void Search::setScanning(const int &scanning) {
    // All scans complete
    if (scanning <= 0 && _multiscan) {
        setMultiscan(false);
    }
    // Completed scanning?
    if (_scanning != 0 && scanning == 0) {
        if (_searchType == SearchBundles) {
            qSort(_updateBundlesList.begin(), _updateBundlesList.end(), compareBundles);
            emit bundleListChanged();
        }
        if (_searchType == SearchUpdate) {
            qSort(_updateAppList.begin(), _updateAppList.end(), compareUpdate);
            emit updateMessageChanged();
        }
        // Check if we have have no updates
        if (_searchType != SearchIdle &&
                ((_searchType == SearchUpdate && _updateMessage == "")
                        || (_searchType == SearchBundles && _updateBundlesList.empty()))) {
            bb::system::SystemToast *toast = new bb::system::SystemToast();
            toast->setBody(tr("No updates available."));
            toast->show();
            // Otherwise it disappears straight away!
            QTimer::singleShot(1000, toast, SLOT(deleteLater()));
        }
        _searchType = SearchIdle;
    }
    _scanning = scanning; emit scanningChanged();
}
