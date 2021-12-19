#-------------------------------------------------
#
# Project created by QtCreator 2016-03-17T09:30:55
#
#-------------------------------------------------

# 1.0.1
# Qt5

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileSplitter
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11

RC_FILE = resources.rc

SOURCES += main.cpp\
        mainwindow.cpp \
        splitterthread.cpp

HEADERS  += mainwindow.h \
        splitterthread.h

FORMS    += mainwindow.ui
