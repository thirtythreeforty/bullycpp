#include <iostream>
#include <string>
#include <thread>

#include <QCoreApplication>

#include "PicBootloaderDriver.h"
#include "SerialPort.h"

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);
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
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		bootloader.readDevice();

		bootloader.getVersion();
	}
	catch(std::logic_error e) {
		std::cout << "Caught exception: " << e.what() << std::endl;
	}

	return 0;
}
