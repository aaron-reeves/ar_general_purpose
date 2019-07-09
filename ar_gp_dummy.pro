QT += widgets sql concurrent xml xlsx

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BbEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += \
  -lpsapi \
  C:/libs/C_libs/filemagic-5.03/lib/magic.lib \
  C:/libs/C_libs/libxls-github/lib/libxlsreader.a \
  C:/libs/Qt_libs/QOds/lib/libods.lib \
  C:/libs/C_libs/sprng-2.0a_naadsm/lib/libsprng.lib \
  C:/libs/C_libs/glib-2.22.2/lib/glib-2.0.lib \
  C:/libs/C_libs/gsl-1.8/lib/libgsl.a

INCLUDEPATH += \
  C:/libs/C_libs/filemagic-5.03/include \
  C:/libs/C_libs/libxls-github/include \
  C:/libs/C_libs/glib-2.22.2/include/glib-2.0 \
  C:/libs/Qt_libs/QOds \
  C:/libs/C_libs/gsl-1.8/include \
  C:/libs/C_libs/sprng-2.0a_naadsm/include

INCLUDEPATH += ../ \
               libnaadsm/include

DEFINES += SRC_FILE_NAME=\\\"unamed_file\\\" # To make libods play nicely.
DEFINES += SIMPLE_SPRNG

SOURCES += \
        ccmdline.cpp \
        cconcurrentrunner.cpp \
        cconfigfile.cpp \
        cencryption.cpp \
        cerror.cpp \
        cfilelist.cpp \
        cformstring.cpp \
        clookuptable.cpp \
        cmagic8ball.cpp \
        cqstring.cpp \
        cqstringlist.cpp \
        cquerytable.cpp \
        creverselookupmap.cpp \
        cspreadsheetarray.cpp \
        csv.cpp \
        cxmldom.cpp \
        datetimeutils.cpp \
        debugutils.cpp \
        filemagic.cpp \
        getlastinsertid.cpp \
        help.cpp \
        libnaadsm/src/rng.c \
        linesinfile.cpp \
        log.cpp \
        logbrowser.cpp \
        logbrowserdialog.cpp \
        main.cpp \
        odsutils.cpp \
        qarcommandlineparser.cpp \
        qcout.cpp \
        qdoublevector.cpp \
        qintvector.cpp \
        qmath.cpp \
        qorderedhash.cpp \
        returncodes.cpp \
        seh.cpp \
        strutils.cpp \
        sysutils.cpp \
        xlcsv.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
  arcommon.h \
  arxl.h \
  ccmdline.h \
  cconcurrentrunner.h \
  cconfigfile.h \
  cencryption.h \
  cerror.h \
  cfilelist.h \
  cformstring.h \
  clookup.h \
  clookuptable.h \
  clookuptable2.h \
  cmagic8ball.h \
  cqstring.h \
  cqstringlist.h \
  cquerytable.h \
  creverselookupmap.h \
  cspreadsheetarray.h \
  csv.h \
  ctwodarray.h \
  cxmldom.h \
  datetimeutils.h \
  debugutils.h \
  filemagic.h \
  getlastinsertid.h \
  help.h \
  linesinfile.h \
  log.h \
  logbrowser.h \
  logbrowserdialog.h \
  odsutils.h \
  qarcommandlineparser.h \
  qcout.h \
  qdoublevector.h \
  qintvector.h \
  qmath.h \
  qorderedhash.h \
  qtypeinfo.h \
  returncodes.h \
  seh.h \
  strutils.h \
  sysutils.h \
  xlcsv.h
