#ifndef TORRENTCLIENT_H
#define TORRENTCLIENT_H

#include "MetaInfo.h"
#include "TrackerClient.h"
#include "RateControl.h"

#include <QObject>
#include <QTimer>

class PeerWire;

class TorrentClient : public QObject
{
    Q_OBJECT
public:
    explicit TorrentClient(QObject *parent = 0);
    void setTorrent(const QByteArray &data);
    void requestPeers();
    QByteArray infoHash() const;
    QByteArray peerId() const;
    QString port() const;
    void changePort();

private slots:
    void onRequestPeers();
    void onConnectedPeers();
    void onTransfer();
    void onGetNextBlock();

private:
    MetaInfo mMetaInfo;
    TrackerClient *mTrackerClient;
    QByteArray mInfoHash;
    QList<PeerWire *> mListPeers;
    quint32 mCurrentPiece;
    quint32 mCurrentOffset;
    double mNumPieces;
    quint16 mPort;
    QTimer mTimer;
    quint32 mRate;
    RateControl rateControle;
    bool mIsChocked;
    bool mIsInterested;
};

#endif // TORRENTCLIENT_H
