#include "RateControl.h"
#include "PeerWire.h"

#include <QTimer>

RateControl::RateControl(QObject *parent) :
    QObject(parent),
    mListPeers(0)
{
    mTimer = new QTimer(this);
    mTimer->setInterval(1000);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(getRate()));
}

void RateControl::setListPeers(QList<PeerWire *> *list)
{
    mListPeers = list;
    mTimer->start();
}

void RateControl::getRate()
{
    if (!mListPeers)
        return;

    quint64 sum = 0;
    foreach (PeerWire *peer, *mListPeers)
        sum += peer->peerRate();
    qDebug() << "rate: " << sum;
    mTimer->start();
}
