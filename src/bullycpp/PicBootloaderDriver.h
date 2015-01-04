/** This file is part of BullyCPP.
 **
 **     BullyCPP is free software: you can redistribute it and/or modify
 **     it under the terms of the GNU General Public License as published by
 **     the Free Software Foundation, either version 3 of the License, or
 **     (at your option) any later version.
 **
 **     BullyCPP is distributed in the hope that it will be useful,
 **     but WITHOUT ANY WARRANTY; without even the implied warranty of
 **     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **     GNU General Public License for more details.
 **
 **     You should have received a copy of the GNU General Public License
 **     along with BullyCPP.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef PICBOOTLOADERDRIVER_H
#define PICBOOTLOADERDRIVER_H

#include <cstdint>
#include <iostream>
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
	void programHexFile(std::istream& hexFile);
	void parseDeviceFile(const std::string& path);
	void parseDeviceFile(std::istream& deviceFile);

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
