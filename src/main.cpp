#include <QCoreApplication>
#include <QFile>
#include <QDebug>

#include "TorrentClient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFile file("/home/vadim/ProjectQt/qtorrent/test/kubuntu-12.10-desktop-amd64.iso.torrent");
    TorrentClient client;
    if (file.open(QIODevice::ReadOnly)) {
        client.setTorrent(file.readAll());
        client.requestPeers();
    } else {
        qDebug() << "file not open";
    }    return a.exec();
}
