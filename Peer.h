#ifndef PEER_H
#define PEER_H

#include <QHostAddress>

class Peer
{
public:
    Peer();

    QHostAddress ip;
    quint32 port;
    QString id;
};

#endif // PEER_H
