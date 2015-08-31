@echo off
MKDIR build
CD build
QMAKE ../src/bullycpp.pro -spec win32-msvc2013 CONFIG+=release
NMAKE
CD ..
COPY build\release\BullyCPP.exe .
