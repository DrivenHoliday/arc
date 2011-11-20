#-------------------------------------------------
#
# Project created by QtCreator 2011-11-20T03:29:57
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = base_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

LIBS += -L/opt/local/lib
LIBS += -lboost_thread

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../base/release/ -lbase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../base/debug/ -lbase
else:symbian: LIBS += -lbase
else:unix: LIBS += -L$$OUT_PWD/../base/ -lbase

INCLUDEPATH += $$PWD/../base
DEPENDPATH += $$PWD/../base

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../base/release/base.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../base/debug/base.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../base/libbase.a
