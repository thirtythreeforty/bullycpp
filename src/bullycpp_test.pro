include(bullycpp.pro)

DEFINES += OUT_CHAR

SOURCES -= \
    bullycpp.cpp

SOURCES += \
    bullycpp_test.cpp \
    dataxfer/unitTests.c

TARGET = BullyCPP_test

