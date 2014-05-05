#-------------------------------------------------
#
# Project created by QtCreator 2014-05-01T21:20:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qcsv
TEMPLATE = app

INCLUDEPATH += \
  C:/libs/Qt_libs

SOURCES += \
  main.cpp \
  mainwindow.cpp \
    ../../ar_general_purpose/csv.cpp

HEADERS  += \
  mainwindow.h \
    ../../ar_general_purpose/csv.h

FORMS    += \
  ui/mainwindow.ui
