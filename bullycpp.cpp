#include <iostream>
#include <string>
#include <thread>

#include <QCoreApplication>

#include "PicBootloaderDriver.h"
#include "SerialPort.h"

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	if(argc != 2) {
		std::cerr << "Please specify a hex file to flash." << std::endl;
		return 1;
	}

	try {
		std::cout << "Opening tty... ";

		SerialPort tty("/dev/ttyUSB0");
		tty.setSpeed(230400);
		tty.open();

		std::cout << "OK\nInitializing PicBootloaderDriver... ";

		bullycpp::PicBootloaderDriver bootloader(tty);
		bootloader.parseDeviceFile("devices.txt");

		std::cout << "OK\n";

		tty.setRTS(true);
		tty.setDTR(true);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		tty.setRTS(false);
		tty.setDTR(false);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		if(bootloader.readDevice())
			bootloader.programHexFile(argv[1]);
	}
	catch(std::logic_error e) {
		std::cout << "Caught exception: " << e.what() << std::endl;
	}

	return 0;
}
