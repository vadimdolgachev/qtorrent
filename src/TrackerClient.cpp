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
    mFirstSeeding(true),
    mFirstTrackerRequest(true),
    mIsSeeding(false),
    mLastTrackerRequest(false),
    mClient(client)
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
    BencodeParser parser;
    parser.parse(data);

    if (parser.dictionary().isEmpty()) {
        qWarning() << "error request peers";
        return;
    }
    const QString &failureReason = parser.dictionary().value("failure reason").toString();
    if (!failureReason.isEmpty()) {
        const QString &warningMessage = parser.dictionary().value("warning message").toString();
        qWarning() << "error request peers: " << failureReason << warningMessage;
        return;
    }

    const quint32 complete = parser.dictionary().value("complete").toUInt();
    const quint32 incomplete = parser.dictionary().value("incomplete").toUInt();
    const QString &trackerId = parser.dictionary().value("tracker id").toString();
    qDebug() << "incomplete" << complete << "\nincomplete" << incomplete
             << "\ntracker id" << trackerId;

    if (parser.dictionary().value("peers").type() == QVariant::ByteArray) {
//        peers: (binary model)
        const QByteArray &peers = parser.dictionary().value("peers").toByteArray();
        mListPeers.clear();
        for (int i = 0; i < peers.size(); i += 6) {
            Peer peer;
            uchar *data = (uchar *)peers.constData() + i;
            int port = (int(data[4]) << 8) + data[5];
            uint ipAddress = 0;
            ipAddress += uint(data[0]) << 24;
            ipAddress += uint(data[1]) << 16;
            ipAddress += uint(data[2]) << 8;
            ipAddress += uint(data[3]);
            peer.ip.setAddress(ipAddress);
            peer.port = port;
            mListPeers << peer;
        }
    } else {
//         peers: (dictionary model)
        qWarning() << "reply has peers(dictionary model). exit";
        return;
    }
    qDebug() << "got nums peers: " << mListPeers.size();
    if (mListPeers.size() <= 1) {
        mClient->changePort();
        quint32 msec = parser.dictionary().value("interval").toUInt();
        qDebug() << "request peers again through: " << msec << "ms";
        QTimer::singleShot(msec, this, SLOT(onRequestPeers()));
    } else {
        emit onGetPeers();
    }
}

void TrackerClient::onRequestPeers()
{
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
    QList<QHostAddress> ipAddresses = QNetworkInterface::allAddresses();
    query += "&ip=" +
            QUrl::toPercentEncoding(ipAddresses[ipAddresses.size()-1].toString());
    query += "&uploaded=0";

    if (!mFirstSeeding) {
        query += "&downloaded=0";
        query += "&left=0";
    } else {
        query += "&downloaded=0";
        query += "&left=" + QByteArray::number(mMetaInfo.totalSize());
    }

    if (mFirstTrackerRequest) {
        mFirstTrackerRequest = false;
        query += "&event=started";
    } else if(mLastTrackerRequest) {
        query += "&event=stopped";
    } else {
        query += "&event=completed";
    }

    QNetworkRequest request(QUrl::fromEncoded(url.toUtf8() + query));
    request.setRawHeader("Host", QUrl(url).host().toUtf8());
    request.setRawHeader("User-Agent", "qtorrent");
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Connection", "close");

    mNAM->get(request);
}
