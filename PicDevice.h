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
		, processID(processID)
		, family(family)
		, configWord(configWord)
		, configPage((configWord / 1024) * 1024)
		, smallRAM(smallRAM)
	{}

public:
	std::string name;
	unsigned int id;

	Family family;
	unsigned int configPage;
	unsigned int configWord;
	unsigned int revision;
	unsigned int processID;
	bool smallRAM;
};

}

#endif
