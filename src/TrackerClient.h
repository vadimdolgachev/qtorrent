#ifndef TRACKERCLIENT_H
#define TRACKERCLIENT_H

#include "MetaInfo.h"
#include "Peer.h"

#include <QObject>

class TorrentClient;
class QNetworkAccessManager;
class QNetworkReply;

class TrackerClient : public QObject
{
    Q_OBJECT
public:
    TrackerClient(TorrentClient *client, QObject *parent = 0);
    void start(const MetaInfo &metaInfo);
    QList<Peer> listPeers() const;

signals:
    void onGetPeers();

public slots:
    void onRequestPeers();

private slots:
    void onRequestPeers(QNetworkReply *reply);

private:
    bool mFirstSeeding;
    bool mFirstTrackerRequest;
    bool mIsSeeding;
    bool mLastTrackerRequest;
    MetaInfo mMetaInfo;
    QList<Peer> mListPeers;
    QNetworkAccessManager *mNAM;
    quint32 mTimeRequestPeers;
    TorrentClient *mClient;
};

#endif // TRACKERCLIENT_H
