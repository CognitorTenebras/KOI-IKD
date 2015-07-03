#-------------------------------------------------
#
# Project created by QtCreator 2015-01-15T14:41:57
#
#-------------------------------------------------

QT += core gui

QT += widgets

INCLUDEPATH += "..\KOI-IKD\tiff-4.0.4beta\libtiff"

LIBS += Setupapi.lib \
        -L"..\KOI-IKD\tiff-4.0.4beta\libtiff" -llibtiff

TARGET = KOI-IKD
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    view.cpp \
    pivolsthread.cpp \
    picture.cpp \
    pivols.cpp \
    videoplayer.cpp \
    resources.cpp


HEADERS  += mainwindow.h \
    view.h \
    pivolsthread.h \
    picture.h \
    pivols.h \
    videoplayer.h \
    resources.h


FORMS    += mainwindow.ui

OTHER_FILES += \
    play.jpg \
    stop.jpg \
    recordNO.jpg \
    recordYES.jpg

#QMAKE_CXXFLAGS += /MD
