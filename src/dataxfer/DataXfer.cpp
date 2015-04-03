#include "DataXfer.h"

DataXfer::DataXfer(IDataXferCallbacks *callbacks)
{
}

void DataXfer::onDataIn(const std::string &bytes, const unsigned int currentMilliseconds)
{
}

void DataXfer::onDataOut(const std::string &typed)
{
}

void DataXfer::variableEdited(const unsigned int index, const std::string &newValue)
{
}
