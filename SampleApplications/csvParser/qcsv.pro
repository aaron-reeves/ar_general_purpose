#-------------------------------------------------
#
# Project created by QtCreator 2014-05-01T21:20:56
#
#-------------------------------------------------

QT       += core gui

CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qcsv
TEMPLATE = app

INCLUDEPATH += \
  ../../../

SOURCES += \
  main.cpp \
  mainwindow.cpp \
    ../../../ar_general_purpose/csv.cpp \
    ../../../ar_general_purpose/strutils.cpp

HEADERS  += \
  mainwindow.h \
    ../../../ar_general_purpose/csv.h \
    ../../../ar_general_purpose/strutils.h

FORMS    += \
  ui/mainwindow.ui
