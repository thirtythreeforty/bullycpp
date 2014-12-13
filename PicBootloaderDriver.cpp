#include "PicBootloaderDriver.h"

#include <array>
#include <fstream>
#include <map>
#include <stdexcept>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include "util.h"

namespace bullycpp {

PicBootloaderDriver::PicBootloaderDriver(std::iostream& stream)
	: currentDevice()
	, configBitsEnabled(true)
	, firmwareVersion(0)
	, stream(stream)
{}

const boost::optional<PicDevice>& PicBootloaderDriver::readDevice()
{
	std::array<uint8_t, 8> inputData;
	uint16_t deviceID, revision, processID;

	this->stream << Command::READ_ID;
	this->stream >> inputData;

	deviceID = (inputData[1] << 8) | inputData[0];
	processID = inputData[5] >> 4;
	revision = (inputData[5] << 8) | inputData[4];

	for(const auto& device: this->devices) {
		if(device.id == deviceID && device.processID == processID) {
			// found device
			currentDevice = device;
			currentDevice->revision = revision;
			return currentDevice;
		}
	}

	currentDevice.reset();
	std::cerr << "Device ID: " << std::hex << deviceID << "\n"
	          << "Process ID: " << std::hex << processID << "\n"
	          << "Refusing to program unknown device.  Check device or baud rate."
	          << std::endl;

	return currentDevice;
}

bool PicBootloaderDriver::shouldSkipRow(const MemRow& thisRow, const PicDevice::Family family)
{
	uint32_t i = thisRow.getAddress();

	if(this->firmwareVersion >= 3 && i < PROGRAM_START)
		// Do not write any rows below program start
		return true;

	if(family == PicDevice::Family::PIC24F ||
	   family == PicDevice::Family::PIC24E ||
	   family == PicDevice::Family::dsPIC33E) {
		// Check if this is the config bit page
		if(i >= currentDevice->configPage && !thisRow.isEmpty()) {
			std::cout << "Skipping memory row " << std::hex << i << " on config bit page" << std::endl;

			return true;
		}
	}
	return false;
}

void PicBootloaderDriver::getVersion() {
	uint8_t majorVersion;

	std::cout << "Reading firmware version...\n";
	this->stream << Command::READ_VERSION;
	this->stream >> majorVersion;

	if(majorVersion == Command::NACK) {
		// Old bootloader
		this->firmwareVersion = 0;

		if(!currentDevice)
			throw new std::logic_error("Device not read!");
		this->configBitsEnabled = (currentDevice->family == PicDevice::Family::PIC24H ||
		                           currentDevice->family == PicDevice::Family::PIC24FK ||
		                           currentDevice->family == PicDevice::Family::dsPIC33F);

		std::cerr << "Detected firmware version 0: Config bits always written for PIC24H,\n"
		          << "but not for PIC24F, PIC24E, or dsPIC33E (last page of program\n"
		          << "memory skipped for these devices).\n"
		          << "Update to the latest firmware to change this behavior." << std::endl;
		return;
	}

	this->firmwareVersion = majorVersion;

	std::array<uint8_t, 2> minorVersion;
	this->stream >> minorVersion;

	std::cout << "Firmware version: " << majorVersion << minorVersion[0]
	          << ", config bits programming ";
	if(this->configBitsEnabled)
		std::cout << "enabled.";
	else
		std::cout << "disabled.";
	std::cout << std::endl;

	if(this->firmwareVersion >= 3) {
		std::cout << "Firmware v3.0 or later detected.\n"
		          << "No pages below location " << PROGRAM_START << " will be written." << std::endl;
	}
}

void PicBootloaderDriver::programHexFile(const std::string& path)
{
	std::vector<uint8_t> buffer;
	int32_t extAddr = 0;
	std::vector<MemRow> ppMemory;
	std::vector<MemRow> ppMemoryVerify;
	uint32_t pm33f_rowsize;

	getVersion();

	const PicDevice::Family family = this->currentDevice->family;
	if(family == PicDevice::Family::PIC24FK)
		pm33f_rowsize = MemRow::PIC24FK_ROW_SIZE;
	else if(this->currentDevice->smallRAM)
		pm33f_rowsize = MemRow::PM33F_ROW_SIZE_SMALL;
	else
		pm33f_rowsize = MemRow::PM33F_ROW_SIZE_LARGE;

	ppMemory.reserve(MemRow::PM_SIZE + MemRow::EE_SIZE + MemRow::CM_SIZE);

	for(unsigned int row = 0; row < MemRow::PM_SIZE; ++row)
		ppMemory.emplace_back(MemRow::MemType::Program, 0x000000, row, family, pm33f_rowsize);
	for(unsigned int row = 0; row < MemRow::EE_SIZE; ++row)
		ppMemory.emplace_back(MemRow::MemType::EEProm, 0x7FF000, row, family, pm33f_rowsize);
	for(unsigned int row = 0; row < MemRow::CM_SIZE; ++row)
		ppMemory.emplace_back(MemRow::MemType::Configuration, 0xF80000, row, family, pm33f_rowsize);

	std::cerr << "Reading hex file..." << std::endl;

	std::ifstream hexFile(path);
	if(!hexFile) {
		std::cerr << "Error while opening hex file." << std::endl;
		return;
	}

	while(hexFile.good()) {
	}
}

void PicBootloaderDriver::parseDeviceLine(const std::string& deviceLine)
{
	using std::string;
	using std::stoi;
	using std::get;

	std::vector<string> parts;
	parts.reserve(6);
	boost::split(parts, deviceLine, boost::is_any_of(","));
	if(parts.size() != 6) {
		std::cerr << "Bad device line: " << deviceLine << std::endl;
		return;
	}

	const string& devName = parts[0];
	const uint32_t devID = stoi(parts[1]);
	const uint32_t PID = stoi(parts[2]);
	const string& famName = parts[3];
	const uint32_t configPage = stoi(parts[4]);
	const bool smallRAM = (stoi(parts[5]) != 0);

	static const std::map<string, PicDevice::Family> map = {
		{"dsPIC30F", PicDevice::Family::dsPIC30F},
		{"dsPIC33F", PicDevice::Family::dsPIC33F},
		{"PIC24H",   PicDevice::Family::PIC24H},
		{"PIC24F",   PicDevice::Family::PIC24F},
		{"PIC24FK",  PicDevice::Family::PIC24FK},
		{"PIC24E",   PicDevice::Family::PIC24E},
		{"dsPIC33E", PicDevice::Family::dsPIC33E}
	};
	const auto familyIter = map.find(famName);
	if(familyIter == std::end(map)) {
		std::cerr << "Unrecognized device: " << famName << std::endl;
		return;
	}

	this->devices.emplace_back(devName, devID, PID, get<1>(*familyIter), configPage, smallRAM);
}

}
