QT += core
QT -= gui
QT += xlsx

CONFIG += c++11

TARGET = qtxls2csv
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += \
  ../../../

SOURCES += \
    ../../../ar_general_purpose/xlcsv/ole.cpp \
    ../../../ar_general_purpose/xlcsv/xls.cpp \
    ../../../ar_general_purpose/xlcsv/xlstool.cpp \
    ../../../ar_general_purpose/csv.cpp \
    ../../../ar_general_purpose/xlcsv.cpp \
    ../../../ar_general_purpose/xlutils.cpp \
    ../../../ar_general_purpose/strutils.cpp \
    ../../../ar_general_purpose/qcout.cpp \
    main.cpp

HEADERS += \
    ../../../ar_general_purpose/xlcsv/brdb.c.h \
    ../../../ar_general_purpose/xlcsv/brdb.h \
    ../../../ar_general_purpose/xlcsv/ole.h \
    ../../../ar_general_purpose/xlcsv/xls.h \
    ../../../ar_general_purpose/xlcsv/xlsstruct.h \
    ../../../ar_general_purpose/xlcsv/xlstool.h \
    ../../../ar_general_purpose/xlcsv/xlstypes.h \
    ../../../ar_general_purpose/csv.h \
    ../../../ar_general_purpose/xlcsv.h \
    ../../../ar_general_purpose/xlutils.h \
    ../../../ar_general_purpose/strutils.h \
    ../../../ar_general_purpose/qcout.h

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

