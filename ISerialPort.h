#ifndef ISERIALPORT_H
#define ISERIALPORT_H

#include <array>

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
