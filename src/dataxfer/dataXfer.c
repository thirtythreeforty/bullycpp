#include "dataXfer.h"
#include <string.h>
#include <stdio.h>

/** \file
 *  \brief Implementation of the \ref index "uC data transfer protocol".
 */

// MS compiler workaround for snprintf.
#ifdef _MSC_VER
# define SNPRINTF sprintf_s
#else
# define SNPRINTF snprintf
#endif

void
initDataXfer() {
  resetReceiveMachine();
  clearReceiveStruct();
}


/// \name Send functions
//@{

void
outCharXfer(char c) {
  // If we're sending a character that needs escaping, then escape it.
  OUT_CHAR(c);
  if (c == CMD_TOKEN)
    OUT_CHAR(ESCAPED_CMD);
}

#if defined(MICROCONTROLLER) || defined(__DOXYGEN__) || defined(UNIT_TESTS)
void
specifyVar(uint u_varIndex, volatile void* pv_data, uint u_size,
           BOOL b_isWriteable, char* psz_format, char* psz_name,
           char* psz_desc) {
  uint u_len;

  // Make sure this variable exists
  ASSERTM("specifyVar:indexTooHigh", u_varIndex < NUM_XFER_VARS);
  // Make sure the data isn't NULL
  ASSERTM("specifyVar:nullData", pv_data != NULL);
  // Make sure the size is valid
  ASSERTM("specifyVar:invalidSize", (u_size > 0) && (u_size <= (UINT8_MAX + 1)));

  // Update data structure
  xferVar[u_varIndex].pu8_data = (uint8_t*) pv_data;
  xferVar[u_varIndex].u8_size = u_size - 1;
  assignBit(u_varIndex, b_isWriteable);

  // Send a command
  OUT_CHAR(CMD_TOKEN);

  // Send a specification: The spec code, index, then length
  outCharXfer(b_isWriteable ? CMD_SEND_RECEIVE_VAR : CMD_SEND_ONLY);
  outCharXfer(u_varIndex);
  // Include the space taken by the three NULL characters, minus one since a
  // length of 1 is sent as 0, plus one for the variable size byte.
  u_len = strlen(psz_format) + strlen(psz_name) + strlen(psz_desc) + 3 - 1 + 1;
  // Allow a maximum data length of 256. Cap at 255, since all lengths are
  // (actual length - 1).
  outCharXfer(u_len <= UINT8_MAX ? u_len : UINT8_MAX);

  // Send the size of this variable, minus 1 since a size of 1 is sent as a 0.
  outCharXfer(u_size - 1);

  // Send the strings; do not send more than 255 chars (the size just sent makes
  // the 256th character, the maximum length).
  u_len = 0;
  do {
    if (++u_len > UINT8_MAX) return;
    outCharXfer(*psz_format);
  } while (*psz_format++);
  do {
    if (++u_len > UINT8_MAX) return;
    outCharXfer(*psz_name);
  } while (*psz_name++);
  do {
    if (++u_len > UINT8_MAX) return;
    outCharXfer(*psz_desc);
  } while (*psz_desc++);
}
#endif

void
sendVar(uint u_varIndex) {
  XFER_VAR* pXferVar;
  uint8_t u8_size;
  uint8_t* pu8_data;
  // Make sure this variable exists
  ASSERTM("sendVar:indexTooHigh", u_varIndex < NUM_XFER_VARS);
  // Note: The MS C compiler flags the statement
  // XFER_VAR* pXferVar = xferVar + u_varIndex;
  // as an error. It's OK in MS C++. Apparently, the C compiler doesn't
  // support the newer C99 syntax. Therefore, u8_size and pu8_data are
  // also declared above.
  pXferVar = xferVar + u_varIndex;
  ASSERTM("sendVar:indexNotSpecified", pXferVar->pu8_data != NULL);
  // Make sure it's read/write (PC only)
#ifndef MICROCONTROLLER
  ASSERTM("sendVar:notWriteable", isVarWriteable(u_varIndex));
#endif

  // Send a command
  OUT_CHAR(CMD_TOKEN);

  // Send short/long var info
  u8_size = pXferVar->u8_size;
  if ((u8_size + 1) > SHORT_VAR_MAX_LEN) {
    // Send a long var: The long var code, index, then length
    outCharXfer(CMD_LONG_VAR);
    outCharXfer(u_varIndex);
    outCharXfer(u8_size);
  } else {
    // Send a short var
    outCharXfer((u_varIndex << VAR_SIZE_BITS) | u8_size);
  }

  // Send data
  pu8_data = pXferVar->pu8_data;
  do {
    outCharXfer(*pu8_data++);
  } while (u8_size--);
}

//@}

#ifndef MICROCONTROLLER
int
formatVar(uint u_varIndex, char* psz_buf, size_t s_len) {
  XFER_VAR* pXferVar;
  int i_ret;
  uint u_size;
  uintmax_t um_buf;

  // Make sure this variable exists
  ASSERTM("formatVar:indexTooHigh", u_varIndex < NUM_XFER_VARS);
  // Note: The MS C compiler flags the statement
  // XFER_VAR* pXferVar = xferVar + u_varIndex;
  // as an error. It's OK in MS C++. Apparently, the C compiler doesn't
  // support the newer C99 syntax. Therefore, u8_size and pu8_data are
  // also declared above.
  pXferVar = xferVar + u_varIndex;
  ASSERTM("formatVar:indexNotSpecified", pXferVar->pu8_data != NULL);

  // Copy the data over to the largest available var for formatting.
  // This means strings won't work. How to fix this?
  u_size = pXferVar->u8_size + 1;
  ASSERT(u_size <= sizeof(um_buf));
  memcpy(&um_buf, pXferVar->pu8_data, u_size);

  // Copy the data over to the largest available var for formatting. Using a
  // #defined sprintf to work around MSVC's naming of that function.
  i_ret = SNPRINTF(psz_buf, s_len, pXferVar->psz_format, um_buf);
  return i_ret;
}
#endif

#ifdef MICROCONTROLLER

#include "pic24_clockfreq.h"
#include "pic24_util.h"
#include "pic24_serial.h"

uint
receiveVar(char* p_c) {
  // Receive the data by stepping the machine until it outputs
  // something
  do {
    // While there's no data, run the timeout counter
    RECEIVE_ERROR re;
    uint32_t u32_count = 0;
    while (!isCharReady()) {
      if (u32_count < RECEIVE_TIMEOUT)
        u32_count++;
      doHeartbeat();
    }

    // Step the machine
    *p_c = inChar();
    if (u32_count >= RECEIVE_TIMEOUT)
      notifyOfTimeout();
    re = stepReceiveMachine(*p_c);
    if (re != ERR_NONE) {
      outString("Data receive error: ");
      outString(getReceiveErrorString());
      outChar('\n');
    }
  } while (!isReceiveMachineChar() && !isReceiveMachineData());

  // Note that p_c already contains the received character, since it's
  // always the last thing received from inChar().
  return getReceiveMachineIndex();
}

char
inCharXfer() {
  char c;
  while (receiveVar(&c) != CHAR_RECEIVED_INDEX);
  return c;
}

#else
BOOL receiveVar(char c_in, char* pc_out, uint* pu_index,
                uint64_t u64_timeMs, const char** psz_error) {

  static uint64_t u64_timeLastMs = 0;
  uint64_t u64_timeDeltaMs;
  RECEIVE_ERROR re;

  // Check for 100 ms timeout.
  u64_timeDeltaMs = u64_timeMs - u64_timeLastMs;
  u64_timeLastMs = u64_timeMs;
  if (u64_timeDeltaMs > 100)
    notifyOfTimeout();

  // Step the machine
  re = stepReceiveMachine(c_in);
  if (re != ERR_NONE) {
    *psz_error = getReceiveErrorString();
    return FALSE;
  } else {
    *psz_error = NULL;
  }
  if (isReceiveMachineChar()) {
    *pc_out = getReceiveMachineOutChar();
    *pu_index = CHAR_RECEIVED_INDEX;
    return TRUE;
  }
  if (isReceiveMachineData()) {
    *pu_index = getReceiveMachineIndex();
    return TRUE;
  }
  return FALSE;
}
#endif
