#ifndef IDATAXFERWRAPCALLBACKS_H
#define IDATAXFERWRAPCALLBACKS_H

#include <string>

class IDataXferWrapCallbacks {
public:
    virtual void sendRawData(const std::string& bytes) = 0;
    virtual void displayRawData(const std::string& bytes) = 0;
    virtual void variableUpdated(const unsigned int index,
	                             const std::string& name,
	                             const std::string& description,
	                             const std::string& value,
	                             const bool modifiable) = 0;
};

#endif
