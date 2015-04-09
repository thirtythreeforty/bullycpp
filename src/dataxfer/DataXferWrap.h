#ifndef DATAXFERWRAP_H
#define DATAXFERWRAP_H

#include <string>

#include "IDataXferWrapCallbacks.h"

class DataXferWrap {
public:
    DataXferWrap(IDataXferWrapCallbacks*);
    ~DataXferWrap();

    // When data arrives from the uC, let the dataXfer routines inspect it first to
    // interpret any variables and escaped characters. Inside this, calls to
    // displayRawData or variableUpdated provides processed results to the application.
    void onDataIn(
      // Bytes received from the serial port.
      const std::string& bytes,
      // The current time, in ms. This is used for timeouts when receiving a variable.
      // One concern: if several characters are received, this will be the worst-case
      // time; perhaps the necessary characters arrived immediately, then after a
      // timeout delay some additional characters. This would be incorrectly treated
      // as a timeout. However, we will assume that any characters received will be
      // passed here before the timeout, avoiding this potential issue.
      const unsigned int currentMilliseconds);

    // Before sending data, escape any characters if necessary.
    //
    // Return value: raw characters which can be sent over the serial port.
    std::string& escapeDataOut(
      // Characters from the user, which should be escapted before sending.
      const std::string& typed);

    // When a variable is edited, transform that into a series of raw characters to
    // send over the serial port via sendRawData.
    void variableEdited(
      // The index of the variable being edited. Refer to the data transfer protocol for
      // more dicussion on the concept of variable indicies.
      const unsigned int index,
      // New data to be sent. It must no exceed 256 bytes, per the limitations of
      // the data transfer protocol.
      const std::string& newValue);

protected:
    // Provide access to the callbacks which this wrapper will invoke.
    IDataXferWrapCallbacks *iDataXferWrapCallbacks;
};

#endif
