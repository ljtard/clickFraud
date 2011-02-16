# -------------------------------------------------
# Project created by QtCreator 2011-02-13T12:21:24
# -------------------------------------------------
QT += network \
    webkit

TARGET = ClickFraud
TEMPLATE = app

DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
CONFIG += console
INCLUDEPATH += src \
                build


CONFIG += console \
    release
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/mywebview.cpp
HEADERS += src/mainwindow.h \
    src/mywebview.h
FORMS += ui/mainwindow.ui
RESOURCES += clickfraud.qrc
