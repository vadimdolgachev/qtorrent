#include "PeerWire.h"

#include <QDebug>
#include <QHostAddress>

static const int FullHeaderSize = 68;
static const char ProtocolId[] = "BitTorrent protocol";
static const char ProtocolIdSize = 19;
static const char Reserved[] = "\0\0\0\0\0\0\0\0";
static const char ReservedSize = 8;

PeerWire::PeerWire(const QString &infoHash, const QString &peerId, const QString &id, QTcpSocket *parent) :
    QTcpSocket(parent),
    mId(id),
    mInfoHash(infoHash),
    mPeerId(peerId),
    isSendHandshake(false),
    mSizeBytePerSec(0),
    mSizeByteAll(0),
    mCurrentRate(0),
    mIsChoked(true),
    mIsInterested(false)
{
    connect(&mSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(&mSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(&mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(onSendState()));
}

void PeerWire::onConnected()
{
//    qDebug() << "connect to peer: "
//             << mId << peerAddress().toString() << peerPort();
    sendHandshake();
}

void PeerWire::onReadyRead()
{
//    qDebug() << mId << "byte: " << mSocket.readAll().size();
    if (!isSendHandshake) {
        onSendState();
        startTimer(1000);
        isSendHandshake = true;
        emit onTransfer();
//        qDebug() << mId << "send handshake Ok";
    } else {
        mBuffer = mSocket.readAll();
        mSizeBytePerSec += mBuffer.size();
        mSizeByteAll += mSizeBytePerSec;
        emit onDownloadedBlock();
    }
}

void PeerWire::connectToHostImplementation(const QString &hostName,
                                           quint16 port,
                                           QIODevice::OpenMode mode)
{
    setOpenMode(mode);
    mSocket.connectToHost(hostName, port, mode);
}

void PeerWire::onSocketStateChanged(QAbstractSocket::SocketState state)
{
    setLocalAddress(mSocket.localAddress());
    setLocalPort(mSocket.localPort());
    setPeerName(mSocket.peerName());
    setPeerAddress(mSocket.peerAddress());
    setPeerPort(mSocket.peerPort());
    setSocketState(state);
}

void PeerWire::onSendState()
{
    mTimer.start(5000);
    unchoke();
    interested();
}

void PeerWire::timerEvent(QTimerEvent *event)
{
//    qDebug() << mId << "timer" << mSizeByte;
    mCurrentRate = mSizeBytePerSec / 1024.0;
    mSizeBytePerSec = 0;
    QTcpSocket::timerEvent(event);
}

void PeerWire::sendHandshake()
{
//    qDebug() << mId << "send Handshake";
    QByteArray data;
    data.append(ProtocolIdSize);
    data.append(ProtocolId, ProtocolIdSize);
    data.append(Reserved, ReservedSize);
    data.append(mInfoHash);
    data.append(mPeerId);
    mSocket.write(data, data.size());
}

void PeerWire::request(int piece, int offset, int length)
{
    QByteArray data;
    char message[] = {0, 0, 0, 0, 6};
    toNetworkOrder(13, message);
    data.append(message, sizeof(message));
    char numbers[4 * 3];
    toNetworkOrder(piece, &numbers[0]);
    toNetworkOrder(offset, &numbers[4]);
    toNetworkOrder(length, &numbers[8]);
    data.append(numbers, sizeof(numbers));
    mTimer.start();
    mSocket.write(data, data.size());
}

void PeerWire::choke()
{
}

void PeerWire::unchoke()
{
//    <len=0001><id=1>
    QByteArray data;
    char message[] = {0, 0, 0, 0, 1};
    toNetworkOrder(1, message);
    data.append(message, sizeof(message));
    mSocket.write(data, data.size());
}

void PeerWire::interested()
{
    QByteArray data;
    char message[] = {0, 0, 0, 0, 2};
    toNetworkOrder(1, message);
    data.append(message, sizeof(message));
    mSocket.write(data, data.size());
}

void PeerWire::notInterested()
{
}

void PeerWire::have()
{
}

void PeerWire::bitfield()
{
}

void PeerWire::piece()
{
}

void PeerWire::cancel()
{
}

QByteArray PeerWire::getBlock()
{
    return mBuffer;
}

double PeerWire::peerRate() const
{
    return mCurrentRate;
}

quint64 PeerWire::bytes() const
{
    return mSizeByteAll;
}

QString PeerWire::peerId() const
{
    return mId;
}

void PeerWire::toNetworkOrder(int val, char *out)
{
    unsigned char *uout = (unsigned char *)out;
    uout[3] = (val & 0xff);
    uout[2] = (val & 0xff00) >> 8;
    uout[1] = (val & 0xff0000) >> 16;
    uout[0] = (val & 0xff000000) >> 24;
}
