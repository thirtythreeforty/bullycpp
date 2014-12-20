BullyCPP
========
BullyCPP is a PC-side driver for the [Bully Bootloader](http://www.reesemicro.com/), a bootloader for the PIC24 and dsPIC33 series of microcontrollers.
The Bully Bootloader provides an excellent framework to download application code to these devices without needing to use an ICSP programmer such as a PicKit.
Instead, any UART serial adapter may be used.

Unfortunately, the stock/legacy Bully Bootloader driver program is a GUI .NET application written in C++/CLI, so it is constrained to the Windows platform.
It is difficult to use under Linux and Mac OS X.

In contrast, BullyCPP is written in C++11 using Qt 5, making it usable on all major platforms, especially Linux.
The code was initially translated from the open-source code of the legacy driver, but has been modified to use more standard C++ idioms and practices.

Building
--------
BullyCPP depends on Qt 5.2 or later.  Qt is cross-platform, so BullyCPP should work on any platform with Qt support and a C++11 compiler.

Begin by cloning the repository:

    git clone github.com/thirtythreeforty/bullycpp.git
    cd bullycpp

Initialize QMake:

    qmake

Now build (you can pass `-j` if you have a multicore processor):

    make

The executable `bullycpp` will be generated.

Usage
-----
BullyCPP has both a command-line and a GUI mode.
Running it with no arguments will launch the GUI.

For advanced usage, you may wish to run it without the interface.
Simply pass `--no-gui`, the device name, and the name of an Intel hex file (emitted by MPLAB) you want to flash:

    bullycpp --no-gui -D ttyUSB0 ~/target_code.hex

(You may need to use `./bullycpp` if you have not copied it to your path.)
The options used are explained further below.

BullyCPP has several options you may find of use.
These options affect the GUI mode as well, so you may find it helpful to create a shortcut to the program with all your preferred settings passed on the command line.
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

Licensing
---------
BullyCPP is free software, released under the GNU GPLv3 or later.
The license can be found in the LICENSE file.

[Pull requests](https://www.github.com/thirtythreeforty/bullycpp/pulls) and [bug reports](https://www.github.com/thirtythreeforty/bullycpp/issues) are welcome!
