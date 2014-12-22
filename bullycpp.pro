PROJECT = bullycpp
CONFIG += qt c++11
QT += core serialport gui widgets

SOURCES += \
    bullycpp.cpp \
    MainWindow.cpp \
    QtPicBootloaderDriver.cpp \
    QtPicDriver.cpp \
    MemRow.cpp \
    PicBootloaderDriver.cpp \
    SerialPort.cpp

HEADERS += \
    MainWindow.h \
    InterceptQPlainTextEdit.h \
    QtPicBootloaderDriver.h \
    QtPicDriver.h \
    ISerialPort.h \
    MemRow.h \
    PicBootloaderDriver.h \
    PicDevice.h \
    SerialPort.h \
    IProgressCallback.h

FORMS += mainwindow.ui

TARGET = bullycpp
