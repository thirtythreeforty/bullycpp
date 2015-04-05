#ifndef DATAXFERWRAP_H
#define DATAXFERWRAP_H

#include <string>

#include "IDataXferWrapCallbacks.h"

class DataXferWrap {
public:
    DataXferWrap(IDataXferWrapCallbacks*);
    void onDataIn(const std::string& bytes, const unsigned int currentMilliseconds);
    void onDataOut(const std::string& typed);
    void variableEdited(const unsigned int index, const std::string& newValue);
};

#endif
