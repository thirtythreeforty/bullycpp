#include "DataXferWrap.h"
#include "dataXfer.h"
#include <iostream>

DataXferWrap::DataXferWrap(IDataXferWrapCallbacks *callbacks)
{
    this->iDataXferWrapCallbacks = callbacks;
    initDataXfer();
}

void DataXferWrap::onDataIn(const std::string &bytes, const unsigned int currentMilliseconds)
{
    for (auto c: bytes) {
        char c_in = c;
        char c_out;
        uint u_index;
        const char* sz_error;

        // If an item was received, pass it on.
        if (receiveVar(c_in, &c_out, &u_index, currentMilliseconds, &sz_error)) {
            if (u_index == CHAR_RECEIVED_INDEX) {
                std::string s(1, c_out);
                this->iDataXferWrapCallbacks->displayRawData(s);
            } else {
                ASSERT(u_index < NUM_XFER_VARS);
                // We received a variable. Obtains its contents from xferVar.
                std::string name(xferVar[u_index].psz_name);
                std::string description(xferVar[u_index].psz_desc);
                bool isModifiable = isVarWriteable(u_index);

                // Format the value per the specified format string.
                std::string value;
                char s_value[1024];
                int i_ret = formatVar(u_index, s_value, sizeof(s_value));
                if (i_ret < 0) {
                    value = "Error: cannot format value.";
                } else {
                    value = s_value;
                }

                // Pass it to the GUI.
                std::cout << "u_index = " << u_index << ", name = " << name << ", description = " << description << ", value = " << value << ", isModifiable = " << isModifiable << "\n";
                this->iDataXferWrapCallbacks->variableUpdated(u_index, name, description,
                                                              value, isModifiable);
            }
        }
    }
}

// Accumulate output characters in a buffer.
static std::string outCharBuffer;
void outChar(uint8_t c) {
    outCharBuffer.append(1, c);
}

std::string& DataXferWrap::escapeDataOut(const std::string &typed)
{
    // Clear the output buffer, fill it with output charactesrs (outCharXfer will
    // call outChar above), then return that for transmission.
    outCharBuffer.clear();
    for (auto c: typed) {
        outCharXfer(c);
    }
    return outCharBuffer;
}

void DataXferWrap::variableEdited(const unsigned int u_index, const std::string &newValue)
{
    ASSERT(u_index < NUM_XFER_VARS);
    ASSERT(isVarWriteable(u_index));

    // Convert the data then send it.
    sscanf(reinterpret_cast<char*>(xferVar[u_index].pu8_data), xferVar[u_index].psz_format, newValue.data());
    sendVar(u_index);
}
