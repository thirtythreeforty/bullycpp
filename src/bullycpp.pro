PROJECT = bullycpp
CONFIG += qt c++11
QT += core gui network serialport widgets

VERSION = 1.1.0
DEFINES += VERSION_STRING=\\\"v$$VERSION\\\"

SOURCES += \
    bullycpp/MemRow.cpp \
    bullycpp/PicBootloaderDriver.cpp \
    bullycpp.cpp \
    dataxfer/dataXfer.c \
    dataxfer/dataXferImpl.c \
    dataxfer/DataXferWrap.cpp \
    MainWindow.cpp \
    QtDataXfer.cpp \
    QtPicBootloaderDriver.cpp \
    QtPicDriver.cpp \
    SerialPort.cpp \
    widgets/StickyQButton.cpp

HEADERS += \
    bullycpp/IProgressCallback.h \
    bullycpp/ISerialPort.h \
    bullycpp/MemRow.h \
    bullycpp/PicBootloaderDriver.h \
    bullycpp/PicDevice.h \
    bullycpp/util.h \
    dataxfer/dataXfer.h \
    dataxfer/dataXferImpl.h \
    dataxfer/IDataXferWrapCallbacks.h \
    dataxfer/DataXferWrap.h \
    MainWindow.h \
    QStdStreamBuf.h \
    QtDataXfer.h \
    QtPicBootloaderDriver.h \
    QtPicDriver.h \
    SerialPort.h \
    widgets/CollapsingQTabWidget.h \
    widgets/InterceptQPlainTextEdit.h \
    widgets/StickyQButton.h \
    widgets/PopupAlertQComboBox.h

FORMS += mainwindow.ui \
    StickyQButton.ui

RESOURCES += bullycpp.qrc

RC_ICONS = bullycpp.ico
ICON = bullycpp.icns

TARGET = BullyCPP
win32:TARGET = BullyCPPw

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr/local/
    }

    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share

    target.path = $$BINDIR

    desktop.path = $$DATADIR/applications/
    desktop.files += BullyCPP.desktop

    icon.path = $$DATADIR/icons/
    icon.files += BullyCPP.png

    INSTALLS += target desktop icon
}

# These features can be set on the command line
# as in: $ qmake FEATURES+=the-feature
if(contains(FEATURES, no_update_check)) {
    DEFINES += NO_UPDATE_CHECK
} else {
    SOURCES += GitHubUpdateChecker.cpp
    HEADERS += GitHubUpdateChecker.h
}

if(contains(FEATURES, test)) {
    DEFINES += OUT_CHAR

    SOURCES -= \
        bullycpp.cpp \

    SOURCES += \
        bullycpp_test.cpp \
        dataxfer/unitTests.c \

    TARGET = BullyCPP_test
}
