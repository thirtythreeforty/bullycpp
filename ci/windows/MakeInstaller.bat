rem This script builds the MSI installer for BullyCPP.  Run Build.bat first.

rem Collect dependencies to find with heat.exe, but don't heat the actual
rem binaries; we need to call those out by hand in the WiX script.
mkdir Deploy
windeployqt --dir Deploy --no-compiler-runtime --no-opengl-sw --no-angle --no-system-d3d-compiler BullyCPPw.exe BullyCPP.exe
copy C:\OpenSSL-Win64\bin\libeay32.dll Deploy\
copy C:\OpenSSL-Win64\bin\ssleay32.dll Deploy\

rem Build the installer:
heat dir Deploy -cg BullyCPPDynFiles -ag -srd -sreg -dr INSTALLDIR -out Harvested.wxs
candle -arch x64 BullyCPP.wxs Harvested.wxs
light -b Deploy -ext WixUIExtension -cultures:en-us BullyCPP.wixobj Harvested.wixobj -out BullyCPP.msi
