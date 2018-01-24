#-------------------------------------------------
#
# Project created by QtCreator 2017-09-05T10:05:26
#
#-------------------------------------------------

QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cyjApp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    surfacecommunication.cpp \
    autoalgorithm.cpp \
    pid.cpp \
    trackmemory.cpp \
    CAN.cpp \
    SICK511.cpp \
    sick400.cpp

HEADERS  += mainwindow.h \
    surfacecommunication.h \
    autoalgorithm.h \
    pid.h \
    trackmemory.h \
    CAN.h \
    SICK511.h \
    cyjdata.h \
    sick400.h

FORMS    += mainwindow.ui
