#-------------------------------------------------
#
# Project created by QtCreator 2014-08-15T14:34:50
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = GPSMonitor
TEMPLATE = app


SOURCES += main.cpp\
    iconhelper.cpp \
    frmmessagebox.cpp \
    gpsmonitor.cpp \
    detectedserialportthread.cpp \
    receivedatathread.cpp \
    resolvenmea.cpp \
    myspeedwatch.cpp \
    steerwheelwidget..cpp \
    myindicator.cpp \
    mylcdnumber.cpp \
    gpschart.cpp \
    qcustomplot.cpp

HEADERS  += \
    iconhelper.h \
    frmmessagebox.h \
    myhelper.h \
    detectedserialportthread.h \
    receivedatathread.h \
    gpsmonitor.h \
    resolvenmea.h \
    myspeedwatch.h \
    steerwheelwidget.h \
    myindicator.h \
    mylcdnumber.h \
    gpschart.h \
    qcustomplot.h

FORMS    += \
    frmmessagebox.ui \
    gpsmonitor.ui

MOC_DIR=temp/moc
RCC_DIR=temp/rcc
UI_DIR=temp/ui
OBJECTS_DIR=temp/obj
DESTDIR=bin

win32:RC_FILE=main.rc

RESOURCES += \
    rc.qrc \
    resource.qrc
