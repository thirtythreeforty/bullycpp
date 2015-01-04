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
