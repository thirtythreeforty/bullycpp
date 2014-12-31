PROJECT = bullycpp
CONFIG += qt c++11
QT += core serialport gui widgets

SOURCES += \
    bullycpp/MemRow.cpp \
    bullycpp/PicBootloaderDriver.cpp \
    bullycpp.cpp \
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
    InterceptQPlainTextEdit.h \
    MainWindow.h \
    QtPicBootloaderDriver.h \
    QtPicDriver.h \
    SerialPort.h

FORMS += mainwindow.ui

TARGET = bullycpp.bin
