#ifndef UTIL_H
#define UTIL_H

#include <array>
#include <cstdint>
#include <string>
#include <sstream>
#include <vector>

namespace bullycpp {

uint8_t GetAsciiHexDigit(uint8_t b);
void ToAsciiHex(uint8_t value, std::vector<uint8_t>& p, size_t offset);
uint8_t AsciiDigitToByte(uint8_t c);
uint8_t Hex2ToByte(const std::vector<uint8_t>& p, size_t offset);
uint16_t Hex4ToUint16(const std::vector<uint8_t>& p, size_t offset);
uint32_t Hex8ToUint32(const std::vector<uint8_t>& p, size_t offset);

}

template<size_t Size>
std::ostream& operator<<(std::ostream& out, const std::array<uint8_t, Size>& array)
{
	for(const auto& item: array)
		out << item;
	return out;
}

template<size_t Size>
std::istream& operator>>(std::istream& in, std::array<uint8_t, Size>& array)
{
	for(auto& item: array)
		in >> item;
	return in;
}

std::ostream& operator<<(std::ostream& out, const std::vector<uint8_t>& array)
{
	for(const auto& item: array)
		out << item;
	return out;
}

std::istream& operator>>(std::istream& in, std::vector<uint8_t>& array)
{
	for(auto& item: array)
		in >> item;
	return in;
}

template<unsigned int nth, typename T>
uint8_t nthByte(T t)
{
	return (t >> (8*nth)) & 0xFF;
}

template<typename T>
uint8_t nthByte(T t, unsigned int nth)
{
	return (t >> (8*nth)) & 0xFF;
}

template<typename T>
T parseBytes(std::istream& s) {
	T target;
	std::string str;

	str.resize(sizeof(T) / 8);
	s.read(&str[0], sizeof(T) / 8);
	std::istringstream(std::move(str)) >> std::hex >> target;
	return target;
}

#endif
