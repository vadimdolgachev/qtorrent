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
    MetaInfo mMetaInfo;
    TorrentClient *mClient;
    QNetworkAccessManager *mNAM;
    QList<Peer> mListPeers;
    bool mFirstSeeding;
    quint32 mTimeRequestPeers;
};

#endif // TRACKERCLIENT_H
