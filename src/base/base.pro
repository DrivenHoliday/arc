#-------------------------------------------------
#
# Project created by QtCreator 2011-11-20T03:33:54
#
#-------------------------------------------------

QT       -= core gui

TARGET = base
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += /opt/local/include

SOURCES +=

HEADERS +=
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}


