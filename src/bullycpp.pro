PROJECT = bullycpp
CONFIG += qt c++11
QT += core gui network serialport widgets

VERSION = 0.8.1
DEFINES += VERSION_STRING=\\\"v$$VERSION\\\"

SOURCES += \
    bullycpp/MemRow.cpp \
    bullycpp/PicBootloaderDriver.cpp \
    bullycpp.cpp \
    dataxfer/dataXfer.c \
    dataxfer/dataXferImpl.c \
    dataxfer/DataXferWrap.cpp \
    GitHubUpdateChecker.cpp \
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
    GitHubUpdateChecker.h \
    MainWindow.h \
    QStdStreamBuf.h \
    QtDataXfer.h \
    QtPicBootloaderDriver.h \
    QtPicDriver.h \
    SerialPort.h \
    widgets/CollapsingQTabWidget.h \
    widgets/InterceptQPlainTextEdit.h \
    widgets/StickyQButton.h

FORMS += mainwindow.ui \
    StickyQButton.ui

RESOURCES += bullycpp.qrc

RC_ICONS = bullycpp.ico
ICON = bullycpp.icns

TARGET = BullyCPP
