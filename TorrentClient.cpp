#include "TorrentClient.h"
#include "BencodeParser.h"
#include "PeerWire.h"

#include <QDebug>
#include <QCryptographicHash>
#include <qmath.h>

TorrentClient::TorrentClient(QObject *parent) :
    QObject(parent),
    mTrackerClient(new TrackerClient(this)),
    mCurrentPiece(0),
    mCurrentOffset(0),
    mPort(6881)
{
    connect(mTrackerClient, SIGNAL(onGetPeers()), this, SLOT(onConnectedPeers()));
    qsrand(QDateTime::currentDateTime().currentMSecsSinceEpoch());
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(onRequestPeers()));
}

void TorrentClient::setTorrent(const QByteArray &data)
{
    mMetaInfo.parse(data);
    mInfoHash = QCryptographicHash::hash(mMetaInfo.infoValue(),
                                         QCryptographicHash::Sha1);
    mNumPieces = mMetaInfo.totalSize() / mMetaInfo.pieceLength();
    qDebug() << "number pieces" << mNumPieces << mMetaInfo.pieceLength();
}

void TorrentClient::requestPeers()
{
    qDebug() << "start";
    mTrackerClient->start(mMetaInfo);
}

QByteArray TorrentClient::infoHash() const
{
    return mInfoHash;
}

QByteArray TorrentClient::peerId() const
{
    QByteArray suffix;

    for (int i = 0; i < 12; ++i) {
        const QChar ch('a' + qrand() % 19);
        suffix += qrand() % 2 ? ch.toUpper() : ch;
    }
    return "-QT0001-" + suffix;
}

QString TorrentClient::port() const
{
    return QString::number(mPort);
}

void TorrentClient::changePort()
{
    if (mPort == 6889)
        mPort = 6880;
    ++mPort;
}

void TorrentClient::onRequestPeers()
{
    foreach (PeerWire *peer, mListPeers) {
        if (peer->bytes() / 1024 <= 50) {
            qDebug() << "delete peer: " << peer->peerId();
            mListPeers.removeOne(peer);
            delete peer;
        }
    }
    qDebug() << "nums peers: " << mListPeers.size();
    if (mListPeers.size() < 10)
        mTrackerClient->onRequestPeers();
}

void TorrentClient::onConnectedPeers()
{
    mTimer.start(10000);

    const QList<Peer> &peers = mTrackerClient->listPeers();
    const int size = peers.size();
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < mListPeers.size(); ++j) {
            if (mListPeers[j]->peerAddress() == peers[i].ip) {
                qDebug() << "continue peer";
                continue;
            }
        }
        PeerWire *peer = new PeerWire(infoHash(), peerId(), QString::number(i));
        connect(peer, SIGNAL(onTransfer()), this, SLOT(onTransfer()));
        connect(peer, SIGNAL(onDownloadedBlock()), this, SLOT(onGetNextBlock()));
        mListPeers << peer;
//        qDebug() << "connect to peer" << peer;
        peer->connectToHost(peers[i].ip, peers[i].port, QIODevice::ReadWrite);
    }
    rateControle.setListPeers(&mListPeers);
}

void TorrentClient::onTransfer()
{
    PeerWire *peer = qobject_cast<PeerWire *>(sender());
//    qDebug() << "start transfer" << peer->peerId();
    onGetNextBlock();
}

void TorrentClient::onGetNextBlock()
{
//    qDebug() << "get block" << mCurrentPiece;
    PeerWire *peer = qobject_cast<PeerWire *>(sender());
    quint32 length = pow(2.0, 15); // 32768 byte
    peer->request(0, 0, length);
//    ++mCurrentPiece;
}
