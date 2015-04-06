PROJECT = bullycpp
CONFIG += qt c++11
QT += core gui network serialport widgets

DEFINES += OUT_CHAR

SOURCES += \
    bullycpp/MemRow.cpp \
    bullycpp/PicBootloaderDriver.cpp \
    bullycpp_test.cpp \
    dataxfer/dataXfer.c \
    dataxfer/dataXferImpl.c \
    dataxfer/DataXferWrap.cpp \
    dataxfer/unitTests.c \
    GitHubUpdateChecker.cpp \
    MainWindow.cpp \
    QtDataXfer.cpp \
    QtPicBootloaderDriver.cpp \
    QtPicDriver.cpp \
    SerialPort.cpp

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
    CollapsingQTabWidget.h \
    GitHubUpdateChecker.h \
    InterceptQPlainTextEdit.h \
    MainWindow.h \
    QStdStreamBuf.h \
    QtDataXfer.h \
    QtPicBootloaderDriver.h \
    QtPicDriver.h \
    SerialPort.h

FORMS += mainwindow.ui

RESOURCES += bullycpp.qrc

ICON = bullycpp.icns

TARGET = BullyCPP
