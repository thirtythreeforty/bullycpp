#include "MemRow.h"

#include <array>
#include <stdexcept>

#include "PicBootloaderDriver.h"
#include "util.h"

namespace bullycpp {

MemRow::MemRow(const MemType type, const uint32_t startAddress, const uint32_t rowNumber, const PicDevice::Family family, const uint32_t pm33f_rowsize)
	: data(pm33f_rowsize * 2, 0xFFFF)
	, empty(true)
	, type(type)
	, rowNumber(rowNumber)
	, family(family)
{
	size_t bufferSize;

	if(this->type == MemType::Program) {
		if(this->family == PicDevice::Family::dsPIC30F) {
			this->rowSize = PM30F_ROW_SIZE;
		}
		else {
			this->rowSize = pm33f_rowsize;
		}
	}
	else {
		this->rowSize = pm33f_rowsize;
	}

	switch(this->type) {
	case MemType::Program:
		bufferSize = this->rowSize * 3;
		this->address = startAddress + this->rowNumber * this->rowSize * 2;
		break;
	case MemType::EEProm:
		bufferSize = this->rowSize * 2;
		this->address = startAddress + this->rowNumber * this->rowSize * 2;
		break;
	case MemType::Configuration:
		bufferSize = 3;
		this->address = startAddress + this->rowNumber * 2;
		break;
	}

	this->buffer.reserve(bufferSize);
	this->buffer.insert(std::begin(this->buffer), bufferSize, 0xFF);
}

bool MemRow::insertData(uint32_t address, std::istream& stream)
{
	if(address < this->address)
		return false;
	if(this->type == MemType::Program && address >= (this->address + this->rowSize * 2))
		return false;
	if(this->type == MemType::EEProm && address >= (this->address + this->rowSize * 2))
		return false;
	if(this->type == MemType::Configuration && address >= this->address + 2)
		return false;

	this->empty = false;

	this->data[address - this->address] = parseHex<uint16_t>(stream);
	return true;
}

void MemRow::formatData()
{
	if(this->empty)
		return;

	switch(this->type) {
	case MemType::Program:
		for(size_t i = 0; i < this->rowSize; ++i) {
			this->buffer[    i * 3] = nthByte<1>(this->data[i * 2]);
			this->buffer[1 + i * 3] = nthByte<0>(this->data[i * 2]);
			this->buffer[2 + i * 3] = nthByte<1>(this->data[i * 2 + 1]);
		}
		break;
	case MemType::Configuration:
		this->buffer[0] = nthByte<1>(this->data[0]);
		this->buffer[1] = nthByte<0>(this->data[0]);
		this->buffer[2] = nthByte<1>(this->data[1]);
		break;
	case MemType::EEProm:
		for(size_t i = 0; i < this->rowSize; ++i) {
			this->buffer[    i * 2] = nthByte<1>(this->data[i * 2]);
			this->buffer[1 + i * 2] = nthByte<0>(this->data[i * 2]);
		}
		break;
	}
}

void MemRow::sendData(ISerialPort& port) const
{
	typedef PicBootloaderDriver::Command Command;
	std::array<uint8_t, 4> buffer = {0, 0, 0, 0};

	if(this->empty && this->type != MemType::Configuration)
		return;

	// PIC24F config bits are in program memory.
	if(this->type == MemType::Configuration && this->family == PicDevice::Family::PIC24F)
		return;

	while(buffer[0] != Command::ACK) {
		switch(this->type) {
		case MemType::Program:
			buffer = {Command::WRITE_PM,
			          nthByte<0>(this->address),
			          nthByte<1>(this->address),
			          nthByte<2>(this->address)
			         };

			port << buffer << this->buffer;

			std::cout << "Mem Address: " << std::hex << this->address << std::endl;

			break;
		case MemType::EEProm:
			buffer = {Command::WRITE_EE,
			          nthByte<0>(this->address),
			          nthByte<1>(this->address),
			          nthByte<2>(this->address)
			         };

			port << buffer << this->buffer;

			break;
		case MemType::Configuration:
			buffer = {Command::WRITE_CM,
			          static_cast<uint8_t>(this->empty ? 1 : 0),
			          this->buffer[0],
			          this->buffer[1]
			         };

			port << buffer;
		}
		port >> buffer;
	}
}

bool MemRow::readData(ISerialPort& port)
{
	typedef PicBootloaderDriver::Command Command;

	if(this->empty && this->type != MemType::Configuration)
		return true;

	std::array<uint8_t, 4> buffer;

	if(this->type == MemType::Program) {
		buffer = {Command::READ_PM,
		          nthByte<0>(this->address),
		          nthByte<1>(this->address),
		          nthByte<2>(this->address)
		         };

		port << buffer;
		port >> this->buffer;

		std::cout << "Mem Address: " << std::hex << this->address << std::endl;

		return true;
	}

	return false;
}

}
