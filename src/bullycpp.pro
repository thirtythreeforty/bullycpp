PROJECT = bullycpp
CONFIG += qt c++11
QT += core gui network serialport widgets

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
    GitHubUpdateChecker.h \
    MainWindow.h \
    QStdStreamBuf.h \
    QtDataXfer.h \
    QtPicBootloaderDriver.h \
    QtPicDriver.h \
    SerialPort.h \
    widgets/CollapsingQTabWidget.h \
    widgets/InterceptQPlainTextEdit.h

FORMS += mainwindow.ui

RESOURCES += bullycpp.qrc

ICON = bullycpp.icns

TARGET = BullyCPP
