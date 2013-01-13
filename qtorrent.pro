#-------------------------------------------------
#
# Project created by QtCreator 2013-01-01T20:55:13
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = libqtorrent
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

OTHER_FILES += \
    README.md

HEADERS += \
    src/TrackerClient.h \
    src/TorrentClient.h \
    src/RateControl.h \
    src/PeerWire.h \
    src/Peer.h \
    src/MetaInfo.h \
    src/BencodeParser.h

SOURCES += \
    src/TrackerClient.cpp \
    src/TorrentClient.cpp \
    src/RateControl.cpp \
    src/PeerWire.cpp \
    src/Peer.cpp \
    src/MetaInfo.cpp \
    src/main.cpp \
    src/BencodeParser.cpp

