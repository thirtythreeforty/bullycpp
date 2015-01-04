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

#ifndef PICDEVICE_H
#define PICDEVICE_H

#include <string>

namespace bullycpp {

class PicDevice {
public:
	enum class Family {
		dsPIC30F,
		dsPIC33F,
		PIC24H,
		PIC24F,
		PIC24FK,
		dsPIC33E,
		PIC24E
	};

public:
	PicDevice(const std::string& name, const unsigned int id, const unsigned int processID, const Family family, const unsigned int configWord, const bool smallRAM)
		: name(name)
		, id(id)
		, family(family)
		, configWord(configWord)
		, configPage((configWord / 1024) * 1024)
		, processID(processID)
		, smallRAM(smallRAM)
	{}

public:
	std::string name;
	unsigned int id;

	Family family;
	unsigned int configWord;
	unsigned int configPage;
	unsigned int revision;
	unsigned int processID;
	bool smallRAM;
};

}

#endif
