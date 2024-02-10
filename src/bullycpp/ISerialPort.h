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

#ifndef ISERIALPORT_H
#define ISERIALPORT_H

#include <array>
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace bullycpp
{

class ISerialPort {
public:
	ISerialPort(ISerialPort& other) = delete;
	virtual ~ISerialPort() = default;

	virtual void setName(const std::string&) = 0;
	virtual void setName(const char*) = 0;

	virtual void setSpeed(const unsigned int) = 0;

	virtual void open() = 0;
	virtual void close() = 0;

	virtual void setDTR(bool) = 0;
	virtual void setRTS(bool) = 0;

	virtual std::vector<uint8_t> read(size_t) = 0;
	virtual void read(uint8_t*, size_t) = 0;
	virtual void read(std::vector<uint8_t>&) = 0;
	virtual unsigned char read() = 0;

	virtual void write(const uint8_t*, size_t) = 0;
	virtual void write(const std::vector<uint8_t>&) = 0;
	virtual void write(const unsigned char) = 0;

	virtual void clear() = 0;

protected:
	ISerialPort() = default;
};

template<size_t Size>
inline ISerialPort& operator>>(ISerialPort& in, std::array<uint8_t, Size>& array)
{
	in.read(&array[0], Size);
	return in;
}

inline ISerialPort& operator<<(ISerialPort& out, const std::vector<uint8_t>& array)
{
	out.write(&array[0], array.size());
	return out;
}

inline ISerialPort& operator>>(ISerialPort& in, std::vector<uint8_t>& array)
{
	in.read(&array[0], array.size());
	return in;
}

inline ISerialPort& operator<<(ISerialPort& out, const unsigned char c) {
	out.write(c);
	return out;
}

inline ISerialPort& operator>>(ISerialPort& in, unsigned char& c) {
	c = in.read();
	return in;
}

} // namespace

#endif
