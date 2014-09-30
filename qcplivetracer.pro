QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = qcplivetracer
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           qcustomplot.cpp \
           qcplivetracer.cpp

HEADERS += mainwindow.h \
           qcustomplot.h \
           qcplivetracer.h \
           colorbutton.h

FORMS   += mainwindow.ui

