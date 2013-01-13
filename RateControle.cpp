#include "RateControle.h"
#include "PeerWire.h"

#include <QTimer>

RateControl::RateControl(QObject *parent) :
    QObject(parent),
    mListPeers(0)
{
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(getRate()));
}

void RateControl::setListPeers(QList<PeerWire *> *list)
{
    mListPeers = list;
    timer->start();
}

void RateControl::getRate()
{
    if (!mListPeers)
        return;

    quint64 sum = 0;
    foreach (PeerWire *peer, *mListPeers)
        sum += peer->peerRate();
    qDebug() << "rate: " << sum;
    timer->start();
}
