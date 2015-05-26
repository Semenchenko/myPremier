#-------------------------------------------------
#
# Project created by QtCreator 2015-02-04T16:34:29
#
#-------------------------------------------------

QT       += core gui

QT       += widgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TermIn
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    portsettings.cpp \
    inputset.cpp

HEADERS  += mainwindow.h \
    portsettings.h \
    inputset.h

FORMS    += mainwindow.ui \
    portsettings.ui \
    inputset.ui

RESOURCES += \
    termin.qrc
