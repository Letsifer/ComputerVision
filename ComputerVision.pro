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
    interestpointsfinder.cpp \
    descriptorfinder.cpp \
    descriptormatcher.cpp \
    ransacalgorithm.cpp

HEADERS  += mainwindow.h \
    MyImage.h \
    kernel.h \
    pyramid.h \
    interestpointsfinder.h \
    descriptorfinder.h \
    descriptormatcher.h \
    ransacalgorithm.h

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
    images/table1.jpg \
    images/table2.jpg \
    images/table3.jpg \
    images/table4.jpg \
    images/table5.jpg \
    images/table6.jpg \
    images/1.png \
    images/2.png \
    images/lena.jpg \
    images/lena90.jpg \
    images/lena180.jpg \
    images/lena-min.jpg \
    images/lena-min-for30-0.jpg \
    images/lena-min-for30-30.jpg \
    images/lena-min-for45-0.jpg \
    images/lena-min-for45-45.jpg \
    images/lena-min-for5-0.jpg \
    images/lena-min-for5-5.jpg \
    images/lena-min-for60-0.jpg \
    images/lena-min-for60-60.jpg \
    images/rounds.jpg \
    images/view1.jpg \
    images/view2.jpg

unix|win32: LIBS += -L$$PWD/gslLib/ -lgsl

INCLUDEPATH += $$PWD/gsl
DEPENDPATH += $$PWD/gsl

unix|win32: LIBS += -L$$PWD/gslLib/ -lgslcblas

INCLUDEPATH += $$PWD/gsl
DEPENDPATH += $$PWD/gsl
