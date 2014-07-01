#-------------------------------------------------
#
# Project created by QtCreator 2014-03-13T16:45:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimpleGUIforimageenhance

TEMPLATE = app
SOURCES += main.cpp\
        mainwindow.cpp

INCLUDEPATH += /usr/include/opencv2

LIBS += -L/usr/lib
LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
