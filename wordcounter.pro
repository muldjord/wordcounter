TEMPLATE = app
TARGET = Wordcounter
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += link_pkgconfig debug
QT_CONFIG -= no-pkg-config
PKGCONFIG += tesseract poppler-qt5
QT += core gui widgets

RESOURCES += wordcounter.qrc
RC_FILE = wordcounter.rc
TRANSLATIONS = wordcounter_da_DK.ts

include(./VERSION)
DEFINES+=VERSION=\\\"$$VERSION\\\"

# Input
HEADERS += src/mainwindow.h \
           src/datawidget.h \
           src/ocrwidget.h \
           src/chardata.h \
           src/lineedit.h \
           src/combobox.h \
           src/checkbox.h \
           src/configpages.h \
           src/configdialog.h \
           src/aboutbox.h

SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/datawidget.cpp \
           src/ocrwidget.cpp \
           src/chardata.cpp \
           src/lineedit.cpp \
           src/combobox.cpp \
           src/checkbox.cpp \
           src/configpages.cpp \
           src/configdialog.cpp \
           src/aboutbox.cpp
