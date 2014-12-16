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
uint8_t nthByte(T t, unsigned int nth)
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

template<typename T>
T parseHex(std::vector<uint8_t> vec, size_t offset)
{
	using namespace std;
	string str;
	str.resize(sizeof(T) * 2 + 1);
	copy(next(begin(vec), offset),
	     next(begin(vec), offset + sizeof(T) * 2),
	     begin(str));
	return stoi(str, 0, 16);
}

}

#endif
