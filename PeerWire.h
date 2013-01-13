#ifndef PEERWIRE_H
#define PEERWIRE_H

#include <QTcpSocket>
#include <QTimer>

class PeerWire : public QTcpSocket
{
    Q_OBJECT
public:
    explicit PeerWire(const QString &infoHash,
                      const QString &peerId,
                      const QString &id = "",
                      QTcpSocket *parent = 0);
    
    void request(int piece, int offset, int length);
    void choke();
    void unchoke();
    void interested();
    void notInterested();
    void have();
    void bitfield();
    void piece();
    void cancel();

    QByteArray getBlock();
    double peerRate() const;
    quint64 bytes() const;
    QString peerId() const;

signals:
    void onTransfer();
    void onDownloadedBlock();

protected slots:
    void connectToHostImplementation(const QString &hostName,
                                     quint16 port,
                                     OpenMode mode);

private slots:
    void onConnected();
    void onReadyRead();
    void onSocketStateChanged(QAbstractSocket::SocketState state);
    void onSendState();

protected:
    void timerEvent(QTimerEvent *event);

private:
    void sendHandshake();
    void toNetworkOrder(int val, char *out);

    const QString mId;
    const QString mInfoHash;
    const QString mPeerId;
    QTcpSocket mSocket;
    bool isSendHandshake;
    quint64 mSizeBytePerSec;
    quint64 mSizeByteAll;
    QTimer mTimer;
    double mCurrentRate;
    bool mIsChoked;
    bool mIsInterested;
    QByteArray mBuffer;
};

#endif // PEERWIRE_H
