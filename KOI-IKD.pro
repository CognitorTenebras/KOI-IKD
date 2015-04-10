#-------------------------------------------------
#
# Project created by QtCreator 2015-01-15T14:41:57
#
#-------------------------------------------------

QT       += core gui

QT += widgets

TARGET = KOI-IKD
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    view.cpp \
    pivols.cpp \
    pivolsthread.cpp


HEADERS  += mainwindow.h \
    fvcamerastateinfo.h \
    fvtypes.h \
    MovieFileParams.h \
    view.h \
    stdafx.h \
    pivols.h \
    pivolsthread.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    befor.jpg \
    next.jpg \
    play.jpg \
    stop.jpg
