#-------------------------------------------------
#
# Project created by QtCreator 2016-12-05T06:06:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = xboxdrv-mapper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    evdevdevicefinder.cpp \
    joystickdevice.cpp

HEADERS  += mainwindow.h \
    evdevdevicefinder.h \
    joystickdevice.h

FORMS    += mainwindow.ui

LIBS += -ludev -levdev

