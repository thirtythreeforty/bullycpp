PROJECT = bullycpp
CONFIG += qt c++11
QT += core gui network serialport widgets

SOURCES += \
    bullycpp/MemRow.cpp \
    bullycpp/PicBootloaderDriver.cpp \
    bullycpp.cpp \
    GitHubUpdateChecker.cpp \
    MainWindow.cpp \
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
    GitHubUpdateChecker.h \
    InterceptQPlainTextEdit.h \
    MainWindow.h \
    QtPicBootloaderDriver.h \
    QtPicDriver.h \
    SerialPort.h \
    QStdStreamBuf.h

FORMS += mainwindow.ui

RESOURCES += bullycpp.qrc

ICON = bullycpp.icns

TARGET = BullyCPP
