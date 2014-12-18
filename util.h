#ifndef UTIL_H
#define UTIL_H

#include <array>
#include <cstdint>
#include <string>
#include <sstream>
#include <vector>

namespace bullycpp {

template<unsigned int nth, typename T>
uint8_t nthByte(T t)
{
	return (t >> (8*nth)) & 0xFF;
}

template<typename T>
T parseHex(std::istream& s) {
	std::string str;
	str.resize(sizeof(T) * 2 + 1);
	s.read(&str[0], sizeof(T) * 2);
	return std::stoi(str, 0, 16);
}

}

#endif
