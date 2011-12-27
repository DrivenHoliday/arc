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

SOURCES += \
    detail_tcp_connection.cpp \
    detail_tcp_acceptor.cpp

HEADERS += \
    detail_tcp_connection.hpp \
    detail_tcp_acceptor.hpp \
    detail_raw_buffer.hpp \
    detail_generic_serialization.hpp \
    detail_generic_connection.hpp \
    arc_detail_net_type.hpp \
    arc_connection.hpp
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}



















