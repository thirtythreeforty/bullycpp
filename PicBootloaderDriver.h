#ifndef PICBOOTLOADERDRIVER_H
#define PICBOOTLOADERDRIVER_H

#include <cstdint>
#include <string>

#include "IProgressCallback.h"
#include "ISerialPort.h"
#include "MemRow.h"
#include "PicDevice.h"

namespace bullycpp {

class PicBootloaderDriver {
	using Status = IProgressCallback::Status;
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
	PicBootloaderDriver(ISerialPort& port, IProgressCallback* progressCallback = nullptr);
	const PicDevice* readDevice();

	void setMCLR(bool mclr);

	void programHexFile(const std::string& path);
	void programHexFile(std::ifstream& hexFile);
	void parseDeviceFile(const std::string& path);
	void parseDeviceFile(std::ifstream& deviceFile);

	void setConfigBitsEnabled(bool value) { configBitsEnabled = value; }
	bool getConfigBitsEnabled() { return configBitsEnabled; }
	const PicDevice* getCurrentDevice() { return currentDevice; }
	void getVersion();

private:
	void parseDeviceLine(const std::string& deviceLine);
	bool checkAddressClash(const unsigned int address, const PicDevice::Family family);
	bool checkAddressClash(const unsigned int address, const uint16_t data, const PicDevice::Family family);
	bool checkAddressClash(const unsigned int address, const uint16_t data, const PicDevice::Family family,
	                       const unsigned int configPage, const unsigned int configWord);
	bool shouldSkipRow(const MemRow& thisRow, const PicDevice::Family family);
	void giveProgress(IProgressCallback::Status status, int percent);

private:
	ISerialPort& port;
	IProgressCallback* progressCallback;

	bool configBitsEnabled;
	unsigned int firmwareVersion;

	std::vector<PicDevice> devices;
	PicDevice* currentDevice;

	static const uint32_t PROGRAM_START = 0xC00;
};

}

#endif
