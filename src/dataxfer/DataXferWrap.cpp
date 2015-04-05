#include "DataXferWrap.h"

DataXferWrap::DataXferWrap(IDataXferWrapCallbacks *callbacks)
{
}

void DataXferWrap::onDataIn(const std::string &bytes, const unsigned int currentMilliseconds)
{
}

void DataXferWrap::onDataOut(const std::string &typed)
{
}

void DataXferWrap::variableEdited(const unsigned int index, const std::string &newValue)
{
}
