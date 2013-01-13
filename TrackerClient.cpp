#include "TrackerClient.h"
#include "TorrentClient.h"
#include "BencodeParser.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QTimer>

TrackerClient::TrackerClient(TorrentClient *client, QObject *parent) :
    QObject(parent),
    mClient(client),
    mFirstSeeding(true)
{
    mNAM = new QNetworkAccessManager(this);
    connect(mNAM, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onRequestPeers(QNetworkReply*)));
}

void TrackerClient::start(const MetaInfo &metaInfo)
{
    mMetaInfo = metaInfo;
    onRequestPeers();
}

QList<Peer> TrackerClient::listPeers() const
{
    return mListPeers;
}

void TrackerClient::onRequestPeers(QNetworkReply *reply)
{
    qDebug() << "request peers done";
    const QByteArray &data = reply->readAll();
//    qDebug() << data;
    BencodeParser parser;
    parser.parse(data);

    const QByteArray &peerTmp = parser.dictionary().value("peers").toByteArray();
//    qDebug() << parser.dictionary();
    mListPeers.clear();
    for (int i = 0; i < peerTmp.size(); i += 6) {
        Peer tmp;
        uchar *data = (uchar *)peerTmp.constData() + i;
        int port = (int(data[4]) << 8) + data[5];
        uint ipAddress = 0;
        ipAddress += uint(data[0]) << 24;
        ipAddress += uint(data[1]) << 16;
        ipAddress += uint(data[2]) << 8;
        ipAddress += uint(data[3]);
        tmp.ip.setAddress(ipAddress);
        tmp.port = port;
        mListPeers << tmp;
//        qDebug() << i / 6 << port << tmp.ip.toString();
    }
    qDebug() << "got nums peers: " << mListPeers.size();
    if (mListPeers.size() <= 1) {
        mClient->changePort();
        quint32 msec = parser.dictionary().value("min interval").toUInt();
        qDebug() << "request peers again through: " << msec << "ms";
        QTimer::singleShot(msec, this, SLOT(onRequestPeers()));
    } else {
        emit onGetPeers();
    }
}

void TrackerClient::onRequestPeers()
{
//    qDebug() << "TrackerClient::onRequestPeers";
    const QString &url = mMetaInfo.announceUrl();
    QString passkey = "?";

    QByteArray infoHash = mClient->infoHash();
    QString endcodeSum;
    for (int i = 0; i < infoHash.size(); ++i) {
        endcodeSum += '%';
        endcodeSum += QString::number(infoHash[i], 16).right(2).rightJustified(2, '0');
    }
    QByteArray query;
    if (url.contains("?uk"))
        query += "&";
    else
        query += passkey;
    query += "info_hash=" + endcodeSum;
    query += "&peer_id=" + mClient->peerId();
    query += "&port=" + mClient->port();
    query += "&compact=1";
    query += "&numwant=30";
    quint32 key = qrand() % 99999999;
    query += "&key=" + QString::number(key);
    QList<QHostAddress> myIpAddresses = QNetworkInterface::allAddresses();
    query += "&ip=" + QUrl::toPercentEncoding(myIpAddresses[myIpAddresses.size()-1].toString());
    query += "&uploaded=0";

    if (!mFirstSeeding) {
        query += "&downloaded=0";
        query += "&left=0";
    } else {
        query += "&downloaded=0";
        query += "&left=" + QByteArray::number(mMetaInfo.totalSize());
    }

    //    if (seeding && firstSeeding) {
    //        query += "&event=completed";
    //        firstSeeding = false;
    //    } else if (firstTrackerRequest) {
    //        firstTrackerRequest = false;
    //        query += "&event=started";
    //    } else if(lastTrackerRequest) {
    //        query += "&event=stopped";
    //    }

    query += "&event=started";
//    qDebug() << url.toUtf8() + query;
    QNetworkRequest request(QUrl::fromEncoded(url.toUtf8() + query));

    request.setRawHeader("Host", QUrl(url).host().toUtf8());
    request.setRawHeader("User-Agent", "qtorrent");
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Connection", "close");

//    foreach (QByteArray item, request.rawHeaderList()) {
//        qDebug() << item << request.rawHeader(item);
//    }
    mNAM->get(request);
}
