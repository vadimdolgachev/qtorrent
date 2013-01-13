#include <QCoreApplication>
#include <QFile>
#include <QDebug>

#include "TorrentClient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFile file("/home/vadim/Downloads/kubuntu-12.10-desktop-amd64.iso(1).torrent");
//    QFile file("/home/vadim/Downloads/openSUSE-12.2-KDE-LiveCD-x86_64.iso.torrent");
//        QFile file("C:/Users/Sasha/Documents/[rutracker.org].t4267879.torrent");
//        QFile file("C:/Users/Sasha/Downloads/kubuntu-12.10-desktop-amd64.iso.torrent");
    TorrentClient client;
    if (file.open(QIODevice::ReadOnly)) {
        client.setTorrent(file.readAll());
        client.requestPeers();
    } else {
        qDebug() << "file not open";
    }    return a.exec();
}
