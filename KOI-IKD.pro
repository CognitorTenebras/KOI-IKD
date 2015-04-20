#-------------------------------------------------
#
# Project created by QtCreator 2015-01-15T14:41:57
#
#-------------------------------------------------

QT       += core gui

QT += widgets

LIBS += Setupapi.lib

TARGET = KOI-IKD
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    view.cpp \
    pivols.cpp \
    pivolsthread.cpp \
    picture.cpp



HEADERS  += mainwindow.h \
    view.h \
    stdafx.h \
    pivols.h \
    pivolsthread.h \
    picture.h


FORMS    += mainwindow.ui

OTHER_FILES += \
    play.jpg \
    stop.jpg \
    recordNO.jpg \
    recordYES.jpg
