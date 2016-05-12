#-------------------------------------------------
#
# Project created by QtCreator 2016-05-12T15:06:39
#
#-------------------------------------------------

win32:RC_ICONS += Images/Project1.ico

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PingMonitorQt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    pingthread.cpp \
    adddialog.cpp

HEADERS  += mainwindow.h \
    pingthread.h \
    adddialog.h

FORMS    += mainwindow.ui \
    adddialog.ui

RESOURCES += \
    resource.qrc
