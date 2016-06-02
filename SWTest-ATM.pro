#
# @file SWTest-ATM.pro
#
# Implementations for a SWTeste-ATME1.
#
# SWTeste-ATME1 Project.
#
# Copyright (c) 2015-2016 BlueSalute Company.
#
# @author Ederson Moura - <ederson.moura@bluesalute.com>
#
#
# $Id: Exp$
#

# Modules.
QT += core\
      gui\
      widgets\
      serialport\
      printsupport\
      network\
      webkitwidgets\
      multimedia\
      multimediawidgets

VERSION = 1.0
TARGET = TestATM-E1
TEMPLATE = app

CONFIG(debug, debug|release) {
    message("Build application in mode DEBUG.")
    QT += testlib
    CONFIG += console
    CONFIG += testcase
    DEFINES += TESTATM_DEBUG
    DESTDIR = $$PWD/build/debug
} else {
    message("Build application in mode RELEASE.")
    CONFIG -= console
    QMAKE_CFLAGS_RELEASE    = -O3
    QMAKE_CXXFLAGS_RELEASE  = $$QMAKE_CFLAGS_RELEASE
    DEFINES += TESTATM_RELEASE                                 
    DESTDIR = $$PWD/build/release
}

# Flags compiler
QMAKE_CFLAGS   = -std=c++0x -ansi
QMAKE_CXXFLAGS = $$QMAKE_CFLAGS

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

win32 {
    INCLUDEPATH += $$PWD/deps/include/
    LIBS += -lwinscard -L$$PWD/deps/lib/win32 -lQsLog2
    win32:RC_ICONS += res/images/win-icon.ico
    QMAKE_TARGET_VERSION = 1.0
    QMAKE_TARGET_COMPANY = BlueSalute Company Ltda.
    QMAKE_TARGET_PRODUCT = SWTest-ATM.
    QMAKE_TARGET_DESCRIPTION = Test software for ATMs.
    QMAKE_TARGET_COPYRIGHT = Copyright 2015-2016 BlueSalute Company. All rights reserved.
} else {
    INCLUDEPATH +=  $$PWD/deps/include/
    LIBS += /usr/lib/x86_64-linux-gnu/libpcsclite.so /home/edm/workspace/sandbox/SPRINT-11/SWTest-ATM/deps/lib/linux32/libQsLog.so
}

INCLUDEPATH += $$PWD/src

SOURCES += src/main.cpp\
           src/mainwindow.cpp\
           src/atmmodelselect.cpp\
           src/settingsserial.cpp\
           src/console.cpp\
           src/serialloopbackfit.cpp\
           src/atmfit.cpp\
           src/dispenserfit.cpp\
           src/billvalidatorfit.cpp\
           src/coinvalidatorfit.cpp\
           src/serialcommandsfit.cpp\
           src/printerfit.cpp\
           src/smartcardreaderfit.cpp\
           src/multiiofit.cpp\
           src/logpreviewer.cpp\
           src/about.cpp\
           src/cameraimagesettings.cpp\
           src/cameravideosettings.cpp\
           src/camerafit.cpp\
           src/searchdevices.cpp\
           src/atm/e1.cpp\
           src/utils/serialport.cpp\
           src/utils/log.cpp\
           src/utils/osinfo.cpp\
           src/utils/tools.cpp\
           src/utils/contrib/qcgaugewidget.cpp\
           src/devices/dispenser/abstractcrt591.cpp\
           src/devices/billvalidator/abstractcashcode609.cpp\
           src/devices/printer/abstractT500.cpp\
           src/devices/multiio/abstractled10v2.cpp\
           src/devices/cardreader/abstractcrt603cz1.cpp\
           src/devices/coinvalidator/abstractmei9520e.cpp\
           test/unit_e1.cpp

HEADERS  += src/mainwindow.hpp\
            src/atmmodelselect.hpp\
            src/settingsserial.hpp\
            src/console.hpp\
            src/serialloopbackfit.hpp\
            src/atmfit.hpp\
            src/dispenserfit.hpp\
            src/billvalidatorfit.hpp\
            src/coinvalidatorfit.hpp\
            src/serialcommandsfit.hpp\
            src/printerfit.hpp\
            src/smartcardreaderfit.hpp\
            src/multiiofit.hpp\
            src/logpreviewer.hpp\
            src/about.hpp\
            src/cameraimagesettings.hpp\
            src/cameravideosettings.hpp\
            src/camerafit.hpp\
            src/searchdevices.hpp\
            src/atm/e1.hpp\
            src/utils/serialport.hpp\
            src/utils/log.hpp\
            src/utils/osinfo.hpp\
            src/utils/tools.hpp\
            src/utils/contrib/qcgaugewidget.hpp\
            src/devices/dispenser/abstractcrt591.hpp\
            src/devices/billvalidator/abstractcashcode609.hpp\
            src/devices/printer/abstractT500.hpp\
            src/devices/multiio/abstractled10v2.hpp\
            src/devices/cardreader/abstractcrt603cz1.hpp\
            src/devices/coinvalidator/abstractmei9520e.hpp\
            test/unit_e1.hpp

FORMS    += ui/mainwindow.ui\
            ui/atmmodelselect.ui\
            ui/settingsserial.ui\
            ui/console.ui\
            ui/serialloopbackfit.ui\
            ui/atmfit.ui\
            ui/dispenserfit.ui\
            ui/billvalidatorfit.ui\
            ui/coinvalidatorfit.ui\
            ui/serialcommandsfit.ui\
            ui/printerfit.ui\
            ui/smartcardreaderfit.ui\
            ui/multiiofit.ui\
            ui/logpreviewer.ui\
            ui/about.ui\
            ui/cameraimagesettings.ui\
            ui/cameravideosettings.ui\
            ui/camerafit.ui\
            ui/searchdevices.ui

RESOURCES += images.qrc\
             others.qrc

# var, prepend, append.
defineReplace(prependAll) {
    for(a,$$1):result += $$2$${a}$$3
    return($$result)
}

# Supported languages.
LANGUAGES = pt_BR

# Available translations.
TRANSLATIONS = $$prependAll(LANGUAGES, $$PWD/ts/TestATM_, .ts)
#TRANSLATIONS = $$prependAll(LANGUAGES, ts/TestATM_, .ts)

# Used to embed the qm files in resources.
#TRANSLATIONS_FILES =

TRANSLATIONS_FILES = $$PWD/ts/TestATM_pt_BR.qm

# Run LRELEASE to generate the qm files.
#qtPrepareTool(LRELEASE, lrelease)
#for(tsfile, TRANSLATIONS) {
#    qmfile = $$shadowed($$tsfile)
#    qmfile ~= s,\\.ts$,.qm,
#    qmdir = $$dirname(qmfile)
#    !exists($$qmdir) {
#        mkpath($$qmdir)|error("Aborting.")
#    }
#    command = $$LRELEASE -removeidentical $$tsfile -qm  $$qmfile
#    system($$command)|error("Failed to run: $$command")
#    TRANSLATIONS_FILES += $$qmfile
#}
