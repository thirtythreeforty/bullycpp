PROJECT = bullycpp
CONFIG += qt c++11 console
QT += serialport

LIBS += -lboost_program_options

SOURCES += bullycpp.cpp

SOURCES += PicBootloaderDriver.cpp
HEADERS += PicBootloaderDriver.h

HEADERS += ISerialPort.h

SOURCES += SerialPort.cpp
HEADERS += SerialPort.h

SOURCES += MemRow.cpp
HEADERS += MemRow.h

HEADERS += PicDevice.h

TARGET = bullycpp.bin
