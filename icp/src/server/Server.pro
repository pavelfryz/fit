# -------------------------------------------------
# Project created by QtCreator 2012-04-29T15:33:30
# -------------------------------------------------

QT += core \
      network \
      xml

QT -= gui

DESTDIR = ../../server2012
OBJECTS_DIR = build

TARGET = server2012
CONFIG += console\
            thread
CONFIG -= app_bundle

TEMPLATE = app

users.path = ../../server2012
users.files += ../../examples/server/users.xml
nets.path = ../../server2012/nets
nets.files += ../../examples/server/nets/*

INSTALLS += nets \
            users

SOURCES += server2012.cpp \
    server.cpp \
    serverthread.cpp \
    net/net.cpp \
    net/arc.cpp \
    net/place.cpp \
    net/transition.cpp

HEADERS += \
    server.h \
    serverthread.h \
    net/net.h \
    net/arc.h \
    net/place.h \
    net/transition.h
