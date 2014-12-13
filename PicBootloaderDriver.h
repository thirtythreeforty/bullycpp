#ifndef PICBOOTLOADERDRIVER_H
#define PICBOOTLOADERDRIVER_H

#include <string>
#include <boost/optional.hpp>

#include "PicDevice.h"
#include "MemRow.h"

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
	PicBootloaderDriver(std::iostream& stream);
	const boost::optional<PicDevice>& readDevice();

	void programHexFile(const std::string& path);
	void parseDeviceLine(const std::string& deviceLine);

	void setConfigBitsEnabled(bool value) { configBitsEnabled = value; }
	bool getConfigBitsEnabled() { return configBitsEnabled; }

private:
	bool checkAddressClash(const unsigned int address, const PicDevice::Family family);
	bool checkAddressClash(const unsigned int address, const std::vector<unsigned char>& data, const unsigned int base, const PicDevice::Family family);
	bool checkAddressClash(const unsigned int address, const std::vector<unsigned char>& data, const unsigned int base, const PicDevice::Family family, const unsigned int configPage, const unsigned int configWord);
	bool shouldSkipRow(const MemRow& thisRow, const PicDevice::Family family);
	void getVersion();

private:
	std::iostream& stream;

	bool configBitsEnabled;
	unsigned int firmwareVersion;

	std::vector<PicDevice> devices;
	boost::optional<PicDevice> currentDevice;

	static const uint32_t PROGRAM_START = 0xC00;
};

}

#endif
