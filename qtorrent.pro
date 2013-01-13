#-------------------------------------------------
#
# Project created by QtCreator 2013-01-01T20:55:13
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = torrentClient
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

OTHER_FILES += \
    README.md

HEADERS += \
    TrackerClient.h \
    TorrentClient.h \
    RateControle.h \
    PeerWire.h \
    Peer.h \
    MetaInfo.h \
    BencodeParser.h

SOURCES += \
    TrackerClient.cpp \
    TorrentClient.cpp \
    RateControle.cpp \
    PeerWire.cpp \
    Peer.cpp \
    MetaInfo.cpp \
    main.cpp \
    BencodeParser.cpp

