BullyCPP
========

[![License GPL 3][badge-license]][license]
[![Appveyor build status][badge-appveyor]][appveyor]
[![Travis build status][badge-travis]][travis]
[![GitHub All Releases][badge-releases]][releases]

BullyCPP is a PC-side driver for the [Bully Bootloader](http://www.reesemicro.com/), a bootloader for the PIC24 and dsPIC33 series of microcontrollers.
The Bully Bootloader provides an excellent framework to download application code to these devices without needing to use an ICSP programmer such as a PICKit.
Instead, any UART serial adapter may be used.

Unfortunately, the stock/legacy Bully Bootloader driver program is a GUI .NET application written in C++/CLI, so it is constrained to the Windows platform.
It is difficult to use under Linux and Mac OS X.

In contrast, BullyCPP is written in C++11 using Qt 5, making it usable on all major platforms, especially Linux.
The code was initially translated from the open-source code of the legacy driver, but has been modified to use more standard C++ idioms and practices.
Furthermore, the backend is completely independent of Qt, allowing it to be retargeted very easily.

![BullyCPP screenshot](https://github.com/thirtythreeforty/bullycpp/raw/master/screenshot.png)

Casual users will probably be interested in Windows binaries, which can be found on the [GitHub Releases](https://github.com/thirtythreeforty/bullycpp/releases) page.

Building
--------
BullyCPP depends on Qt 5.2 or later.  Qt is cross-platform, so BullyCPP should work on any platform with Qt support and a C++11 compiler.

If you have Qt Creator installed, you can simply import the QMake project and click Build.
This is the recommended way to build for personal use on Windows and Mac.

On Linux and Mac OS X, you can build with Qt Creator as described above, or you can build manually.
Begin by cloning the repository:

    git clone github.com/thirtythreeforty/bullycpp.git
    cd bullycpp

Initialize QMake:

    qmake bullycpp.pro

Now build (you can pass `-j` if you have a multicore processor):

    make

The executable `BullyCPP` will be generated.

Redistributing
--------------
If you want to redistribute the application, you will need to use a statically linked Qt, which you must compile yourself.
(The precompiled binaries are statically linked with Qt 5.5.0.)
The `configure` line used to build Qt for the provided Windows precompiled versions is

    configure -opensource -confirm-license -static -release -openssl-linked -skip qtwebkit -platform win32-msvc2013 -nomake examples -nomake tests -no-style-fusion -mp -I C:\openssl\include -L C:\openssl\out32 -L "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib"

The Windows installer package is built using the WiX toolset and MSVC 2013.
To build the release installer package, you must have the above Qt toolchain and WiX in your `PATH`.
Open an MSVC command prompt and navigate to `installer/windows/`, then just run

    Build
    MakeInstaller

The first batch file will build `BullyCPP.exe`, and the second will assemble the installer.

Usage
-----
BullyCPP has both a command-line and a GUI mode.
Running it with no arguments will launch the GUI.

#### GUI mode
GUI Mode provides a visual interaction interface with BullyCPP.
Serial text is displayed in the large text area centered in the GUI. Typing directly in the serial text area will send individual characters as typed.

**Send Message:**
Used to send a string over the serial port. BullyCPP will automatically append a new-line character (\n) to the end of the string before sending. To omit this, hold down CONTROL when either clicking SEND or pressing ENTER (if enabled).

**Target Programming:**
Used to transmit a .hex file over serial. Select the program file with OPEN... and send the data via PROGRAM. Enable "MCLR before programming" to send a MCLR over serial before sending the program file.

**Use µC/PC Variables:**
Used to view variables with serial.

#### Command-line mode
For advanced usage, you may wish to run it without the interface.
You must use the `--no-gui` option, the device name, and the name of an Intel hex file (emitted by MPLAB) you want to flash:

    bullycpp --no-gui --device ttyUSB0 ~/target_code.hex

(You may need to use `./bullycpp` if you have not copied it to your path.)
This command will program the device and exit, providing progress information on the console.
The options used are explained further below.

BullyCPP has several options you may find of use.
These options affect the GUI mode as well, so you may find it helpful to create a shortcut to the program with all your preferred settings passed on the command line.
Again, note that you *must* pass `--no-gui` if you just want to program via the command line; all the options below do not imply it.
For an overview of available options, use the `--help` flag.

The most common option is `--mclr` or `-r` (for Reset).
If the serial device's RTS or DTR line is connected to the PIC's MCLR pin (as is the case on many beginners' development boards), this will reset the device before attempting to program it.
For example:

    bullycpp --mclr ~/target_code.hex

You may also want to adjust the serial port BullyCPP chooses.
The default device is `ttyUSB0`, which is fine for most Linux systems.
However, if you are running it on Windows (which uses `COMx` serial ports), or if your serial device is not the default one, you can use the `--device` or `-D` option.
For example:

    bullycpp --device ttyUSB1 ~/target_code.hex

Finally, BullyCPP defaults to a 230400 baud (bits/sec) connection.
If the device is configured to use a different speed, you can specify one with the `--baud` or `-b` option.
For example:

    bullycpp --baud 115200 ~/target_code.hex

Custom Device List
------------------
BullyCPP uses a device list to identify various Microchip parts.
This list is in the same format as the list used by the legacy Bully Bootloader tool.
The default list is packed into the program at compile time; it can be found [in the source tree](src/devices.txt).
If you need to use a device not present in the default list, you can supply your own list with the `--piclist` command-line option.
For example:

    bullycpp --piclist ~/custom_devices.txt

DataXfer
--------
BullyCPP can use the [µC/PC data transfer protocol](http://www.ece.msstate.edu/courses/ece3724/main_pic24/docs/data_xfer.html) to mux serial data with variable transfer.
To enable the protocol, select the µC/PC tab at the bottom of the BullyCPP window and click the Enable checkbox.
Note that if you are not using this protocol, you should leave the checkbox disabled, as the protocol involves in-band filtering of inbound and outbound data.

Updates
-------
By default, BullyCPP will check for updates when it is launched in GUI mode (assuming it has an Internet connection).
It uses the GitHub API to perform this check; no identifying information is sent.
If a new release is found, a dialog will appear with a link to the new release.
You can disable this behavior in the About dialog if you wish.

Update checks can also be disabled permanently at compile time by setting the `NO_UPDATE_CHECK` preprocessor variable to `1`.
This is especially useful for Linux builds that are distributed with a package manager that performs its own update checks.

Licensing
---------
BullyCPP is free software, released under the GNU General Public License version 3 or later, as published by the Free Software Foundation.
Modification and redistribution are permitted according to the terms of the GPL.
The license can be found in the `LICENSE` file.

[Pull requests](https://www.github.com/thirtythreeforty/bullycpp/pulls) and [bug reports](https://www.github.com/thirtythreeforty/bullycpp/issues) are welcome!

[badge-license]: https://img.shields.io/badge/license-GPL_3-green.svg?dummy
[license]: https://github.com/thirtythreeforty/bullycpp/blob/master/LICENSE
[badge-appveyor]: https://ci.appveyor.com/api/projects/status/fvcvqpw3me5l25pk/branch/master?svg=true
[appveyor]: https://ci.appveyor.com/project/thirtythreeforty/bullycpp
[badge-travis]: https://travis-ci.org/thirtythreeforty/bullycpp.svg?branch=master
[travis]: https://travis-ci.org/thirtythreeforty/bullycpp
[badge-releases]: https://img.shields.io/github/downloads/thirtythreeforty/bullycpp/total.svg
[releases]: https://github.com/thirtythreeforty/bullycpp/releases
