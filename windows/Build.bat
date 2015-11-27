@echo off

MKDIR build-BullyCPPw
CD build-BullyCPPw
QMAKE ../../src/bullycpp.pro -spec win32-msvc2013 CONFIG+=release
NMAKE
CD ..

MKDIR build-BullyCPP
CD build-BullyCPP
QMAKE ../../src/cmdline/cmdline.pro -spec win32-msvc2013 CONFIG+=release
NMAKE
CD ..

COPY build-BullyCPPw\release\BullyCPPw.exe .
COPY build-BullyCPP\release\BullyCPP.exe .
