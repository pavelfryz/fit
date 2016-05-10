# -------------------------------------------------
# Project created by QtCreator 2012-04-24T11:23:15
# -------------------------------------------------

QT += network \
      xml \
      xmlpatterns

DESTDIR = ../../pn2012
OBJECTS_DIR = build

TARGET = pn2012

config.path = ../../pn2012
config.files += ../../examples/client/*.xml
nets.path = ../../pn2012/nets
nets.files += ../../examples/client/nets/*

INSTALLS += nets \
            config

TEMPLATE = app

SOURCES += pn2012.cpp \
    mainwindow.cpp \
    drawFrame.cpp \
    GPlace.cpp \
    GTransition.cpp \
    GArc.cpp \
    dialogs.cpp \
    communication.cpp

HEADERS += mainwindow.h \
    drawFrame.h \
    GPlace.h \
    GTransition.h \
    GArc.h \
    dialogs.h \
    communication.h

FORMS += forms/mainwindow.ui \
    forms/dialogArc.ui \
    forms/dialogPlace.ui \
    forms/dialogTrans.ui \
    forms/connection.ui \
    forms/loginDialog.ui \
    forms/registerDialog.ui \
    forms/runDialog.ui \
    forms/propertiesDialog.ui \
    forms/openDialog.ui \
    forms/findDialog.ui \
    forms/simulationsDialog.ui \
    forms/helpDialog.ui
