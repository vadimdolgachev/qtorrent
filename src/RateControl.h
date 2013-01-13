#ifndef RATECONTROLE_H
#define RATECONTROLE_H

#include <QObject>

class QTimer;
class PeerWire;

class RateControl : public QObject
{
    Q_OBJECT
public:
    explicit RateControl(QObject *parent = 0);
    
    void setListPeers(QList<PeerWire *> *list);
signals:
    
public slots:
    void getRate();

private:
    QTimer *mTimer;
    quint64 numBytes;
    QList<PeerWire *> *mListPeers;
};

#endif // RATECONTROLE_H
