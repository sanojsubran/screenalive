#-------------------------------------------------
#
# Project created by QtCreator 2016-06-05T10:13:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vigilante
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += User32.lib

RC_FILE = vigilante.rc

#RESOURCES += resources/resource.qrc
