QT += core
QT -= gui
QT += xlsx

CONFIG += c++11

TARGET = xlsxMergedCells
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += \
  ../../../


win32 {
  LIBS += \
    C:/libs/C_libs/filemagic-5.03/lib/magic.lib \
    C:/libs/C_libs/libxls-github/lib/libxlsreader.a

  INCLUDEPATH += \
    C:/libs/C_libs/filemagic-5.03/include \
    C:/libs/C_libs/libxls-github/include
} else {
  LIBS += \
    /usr/lib/x86_64-linux-gnu/libmagic.so \
    /usr/local/libxls/lib/libxlsreader.so

  INCLUDEPATH += \
    /usr/local/libxls/include
}


#!win32 {
  ## FIXME: Set appropriate library and include path for Linux version of libxls
#}


SOURCES += \
    ../../../ar_general_purpose/filemagic.cpp \
    ../../../ar_general_purpose/csv.cpp \
    ../../../ar_general_purpose/strutils.cpp \
    ../../../ar_general_purpose/qcout.cpp \
    ../../../ar_general_purpose/cspreadsheetarray.cpp \
    main.cpp


HEADERS += \
    ../../../ar_general_purpose/filemagic.h \
    ../../../ar_general_purpose/csv.h \
    ../../../ar_general_purpose/strutils.h \
    ../../../ar_general_purpose/qcout.h \
    ../../../ar_general_purpose/cspreadsheetarray.h \
    ../../../ar_general_purpose/ctwodarray.h \
    ../../creverselookupmap.h



