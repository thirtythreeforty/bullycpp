#include <iostream>
#include <string>
#include <thread>

#include <boost/program_options.hpp>

#include <QCoreApplication>

#include "PicBootloaderDriver.h"
#include "SerialPort.h"

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);

	// Parse options

	namespace po = boost::program_options;
	po::variables_map varmap;
	po::options_description desc(std::string("Usage: ") + argv[0] + " [OPTION]... HEXFILE\nOptions");
	try {
		po::options_description allopts;
		desc.add_options()
			("mclr,r", "Assert MCLR (RTS/DTR line) before programming target")
			("device,D", po::value<std::string>()->default_value("/dev/ttyUSB0"), "Serial device to use")
			("baud,b", po::value<int>()->default_value(230400), "Serial port speed")
			("piclist", po::value<std::vector<std::string>>()->default_value({"devices.txt"}, "devices.txt"), "PIC device file to read")
			("help", "Print this help message")
		;
		allopts.add(desc);
		allopts.add_options()
			("hexfile,F", po::value<std::string>())
		;
		po::positional_options_description p;
		p.add("hexfile", -1);
		po::store(
			po::command_line_parser(argc, argv)
				.options(desc)
				.options(allopts)
				.positional(p)
				.run(),
			varmap
		);
		po::notify(varmap);
	}
	catch(std::exception& e) {
		std::cerr << argv[0] << ": " << e.what() << std::endl;
		return 2;
	}

	// Do flash

	if(varmap.count("help") || !varmap.count("hexfile")) {
		std::cout << desc << std::endl;
		return 1;
	}

	try {
		std::cout << "Opening tty... ";

		SerialPort tty(varmap["device"].as<std::string>());
		tty.setSpeed(varmap["baud"].as<int>());
		tty.open();

		std::cout << "OK\nInitializing PicBootloaderDriver... ";

		bullycpp::PicBootloaderDriver bootloader(tty);
		for(const auto& deviceFile: varmap["piclist"].as<std::vector<std::string>>()) {
			bootloader.parseDeviceFile(deviceFile);
		}

		std::cout << "OK\n";

		if(varmap.count("mclr")) {
			bootloader.setMCLR(true);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			bootloader.setMCLR(false);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		if(bootloader.readDevice())
			bootloader.programHexFile(varmap["hexfile"].as<std::string>());
	}
	catch(std::logic_error e) {
		std::cout << argv[0] << ": Caught exception: " << e.what() << std::endl;
	}

	return 0;
}
