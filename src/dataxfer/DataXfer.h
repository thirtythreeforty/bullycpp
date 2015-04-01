#ifndef DATAXFER_H
#define DATAXFER_H

#include <string>

#include "IDataXferCallbacks.h"

class DataXfer {
public:
    DataXfer(IDataXferCallbacks*);
    void onDataIn(const std::string& bytes);
    std::string onDataOut(const std::string& typed);
    void variableEdited(const unsigned int index, const std::string& newValue, bool editable);
};

#endif
