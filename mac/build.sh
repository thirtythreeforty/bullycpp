#!/bin/sh

qmake ../src CONFIG+=release
make
macdeployqt BullyCPP.app -dmg
