#-------------------------------------------------
#
# Project created by QtCreator 2017-02-16T12:09:20
#
#-------------------------------------------------
CONFIG   += c++14
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ComputerVision
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    MyImage.cpp \
    kernel.cpp \
    pyramid.cpp \
    interestpointsfinder.cpp

HEADERS  += mainwindow.h \
    MyImage.h \
    kernel.h \
    pyramid.h \
    interestpointsfinder.h

FORMS    += mainwindow.ui

DISTFILES += \
    images/ lab1.jpg \
    images/ lab1X.jpg \
    images/ lab1Y.jpg \
    images/wall.jpg \
    images/Valve.png \
    images/ lab1GaussResult.jpg \
    images/ lab1GaussX.jpg \
    images/ lab1GaussY.jpg \
    images/ lab1SobelResult.jpg \
    images/ lab1SobelX.jpg \
    images/ lab1SobelY.jpg \
    images/ialta.jpg \
    images/image1.jpg \
    images/image2.jpg
