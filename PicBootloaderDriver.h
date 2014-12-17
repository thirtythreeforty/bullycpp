#ifndef PICBOOTLOADERDRIVER_H
#define PICBOOTLOADERDRIVER_H

#include <string>
#include <boost/optional.hpp>

#include "ISerialPort.h"
#include "MemRow.h"
#include "PicDevice.h"

namespace bullycpp {

class PicBootloaderDriver {
public:
	enum Command : uint8_t {
		NACK         = 0x00,
		ACK          = 0x01,
		READ_PM      = 0x02,
		WRITE_PM     = 0x03,
		READ_EE      = 0x04,
		WRITE_EE     = 0x05,
		READ_CM      = 0x06,
		WRITE_CM     = 0x07,
		RESET        = 0x08,
		READ_ID      = 0x09,
		READ_VERSION = 0x11,
		POR_RESET    = 0x13
	};

public:
	PicBootloaderDriver(ISerialPort& port);
	const boost::optional<PicDevice>& readDevice();

	void programHexFile(const std::string& path);
	void programHexFile(std::ifstream& hexFile);
	void parseDeviceFile(const std::string& path);
	void parseDeviceFile(std::ifstream& deviceFile);

	void setConfigBitsEnabled(bool value) { configBitsEnabled = value; }
	bool getConfigBitsEnabled() { return configBitsEnabled; }
	const boost::optional<PicDevice>& getCurrentDevice() { return currentDevice; }
	void getVersion();

private:
	void parseDeviceLine(const std::string& deviceLine);
	bool checkAddressClash(const unsigned int address, const PicDevice::Family family);
	bool checkAddressClash(const unsigned int address, const uint16_t data, const PicDevice::Family family);
	bool checkAddressClash(const unsigned int address, const uint16_t data, const PicDevice::Family family,
	                       const unsigned int configPage, const unsigned int configWord);
	bool shouldSkipRow(const MemRow& thisRow, const PicDevice::Family family);

private:
	ISerialPort& port;

	bool configBitsEnabled;
	unsigned int firmwareVersion;

	std::vector<PicDevice> devices;
	boost::optional<PicDevice> currentDevice;

	static const uint32_t PROGRAM_START = 0xC00;
};

}

#endif
