#-------------------------------------------------
#
# Project created by QtCreator 2011-12-29T05:28:35
#
#-------------------------------------------------

QT       += core gui

TARGET = client
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    logindialog.cpp \
    serverlistmodel.cpp \
    newserverdialog.cpp

HEADERS  += mainwindow.h \
    logindialog.h \
    serverlistmodel.h \
    newserverdialog.h

FORMS    += mainwindow.ui \
    logindialog.ui \
    newserverdialog.ui

INCLUDEPATH += /opt/local/include
LIBS += -L/opt/local/lib
LIBS += -lboost_date_time
LIBS += -lboost_exception
LIBS += -lboost_filesystem
LIBS += -lboost_iostreams
LIBS += -lboost_locale
LIBS += -lboost_program_options
LIBS += -lboost_python
LIBS += -lboost_random
LIBS += -lboost_regex
LIBS += -lboost_serialization
LIBS += -lboost_signals
LIBS += -lboost_system
LIBS += -lboost_thread
LIBS += -lboost_timer
LIBS += -lboost_unit_test_framework
LIBS += -lboost_wave
LIBS += -lboost_wserialization

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../base/release/ -lbase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../base/debug/ -lbase
else:symbian: LIBS += -lbase
else:unix: LIBS += -L$$OUT_PWD/../base/ -lbase

INCLUDEPATH += $$PWD/../base
DEPENDPATH += $$PWD/../base

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../base/release/base.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../base/debug/base.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../base/libbase.a

RESOURCES += \
    icons.qrc
