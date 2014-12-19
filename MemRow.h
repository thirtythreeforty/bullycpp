#ifndef MEMROW_H
#define MEMROW_H

#include <cstdint>
#include <vector>

#include "ISerialPort.h"
#include "PicDevice.h"

namespace bullycpp {

class MemRow {
public:
	enum class MemType {
		Program,
		EEProm,
		Configuration
	};

	static const size_t PM30F_ROW_SIZE = 32;

	/* 64*8*3 is the size of an entire page (1536 bytes) for PM33F
	* This is too large for the PIC24HJ202 to keep in memory, so need to
	* cut down the row size to one half that size.
	*/
	static const size_t PM33F_ROW_SIZE_LARGE = 64 * 8;
	static const size_t PM33F_ROW_SIZE_SMALL = 64 * 4;
	static const size_t PIC24FK_ROW_SIZE = 32 * 4;
	static const size_t EE30F_ROW_SIZE = 16;

	static const size_t PM_SIZE = 1536; /* Max: 144KB/3/32=1536 PM rows for 30F. */
	static const size_t EE_SIZE = 128;  /* 4KB/2/16=128 EE rows */
	static const size_t CM_SIZE = 12;

public:
	MemRow(const MemType type, const uint32_t startAddress, const uint32_t rowNumber, const PicDevice::Family family, const uint32_t pm33f_rowsize);

	bool insertData(uint32_t address, uint16_t data);
	void formatData();
	void sendData(ISerialPort& port) const;
	bool readData(ISerialPort& port);

	uint8_t getByte(size_t index) const { return buffer[index]; }
	void setByte(size_t index, uint8_t data) { buffer[index] = data; }
	size_t getRowSize() const { return rowSize; }
	MemType getType() const { return type; }
	uint32_t getAddress() const { return address; }
	bool isEmpty() const { return empty; }
	uint32_t getRowNumber() const { return rowNumber; }

private:
	std::vector<uint16_t> data;
	std::vector<uint8_t> buffer;
	uint32_t address;
	bool empty;
	MemType type;
	uint32_t rowNumber;
	PicDevice::Family family;
	size_t rowSize;
};

}

#endif
