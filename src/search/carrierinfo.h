#pragma once
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlStreamReader>

class CarrierInfo : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString country     READ country     NOTIFY resultChanged)
    Q_PROPERTY(QString carrier     READ carrier     NOTIFY resultChanged)
    Q_PROPERTY(int     mcc         READ mcc         NOTIFY resultChanged)
    Q_PROPERTY(int     mnc         READ mnc         NOTIFY resultChanged)
    Q_PROPERTY(int     image       READ image       NOTIFY resultChanged)

public:
    CarrierInfo(QObject* parent = 0)
        : QObject(parent)
    , _mcc(-1)
    , _mnc(-1)
    , _image(0)
    {
        _manager = new QNetworkAccessManager();
    }

    Q_INVOKABLE void mccChange(int mcc) {
        if (mcc != _mcc) {
            _mcc = mcc;
            update();
        }
    }

    Q_INVOKABLE void mncChange(int mnc){
        if (mnc != _mnc) {
            _mnc = mnc;
            update();
        }
    }

public slots:
    void update() {
        if (_mnc == -1 || _mcc == -1)
            return;
        QNetworkRequest request(QString("http://appworld.blackberry.com/ClientAPI/checkcarrier?homemcc=%1&homemnc=%2&devicevendorid=-1&pin=0")
                                .arg(_mcc)
                                .arg(_mnc));
        request.setRawHeader("User-Agent", "AppWorld/5.1.0.60");
        QNetworkReply* reply = _manager->get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(carrierResponse()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), reply, SLOT(deleteLater()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(resetNamed()));
    }
    void carrierResponse() {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (reply->size() > 0 && (status == 200 || (status > 300 && status <= 308))) {
            if (reply->url().queryItemValue("homemcc").toInt() != _mcc || reply->url().queryItemValue("homemnc").toInt() != _mnc) {
                reply->deleteLater();
                return;
            }
            QXmlStreamReader xml(reply->readAll());
            while(!xml.atEnd() && !xml.hasError()) {
                if(xml.tokenType() == QXmlStreamReader::StartElement) {
                    if (xml.name() == "country") {
                        _country = xml.attributes().value("name").toString();
                    } else if (xml.name() == "carrier") {
                        _carrier = xml.attributes().value("name").toString().split(" ").first();
                        _image = xml.attributes().value("icon").toString().toInt();

                        // Don't show generic
                        if (_carrier == "default") {
                            _carrier = "";
                            _image = 0;
                        }
                    }
                }
                xml.readNext();
            }
            emit resultChanged();
        }
        reply->deleteLater();
    }
    void resetNamed() {
        _country = "";
        _carrier = "";
        _image = 0;
        emit resultChanged();
    }

    QString country() const { return _country; }
    QString carrier() const { return _carrier; }
    int mcc() const { return _mcc; }
    int mnc() const { return _mnc; }
    int image() const { return _image; }

signals:
    void resultChanged();
private:
    QString _country, _carrier;
    int _mcc, _mnc;
    int _image;
    QNetworkAccessManager* _manager;
};
