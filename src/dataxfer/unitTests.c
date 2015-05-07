// .. "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
//    All rights reserved.
//    (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
//    (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
//    (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
//
//    Permission to use, copy, modify, and distribute this software and its
//    documentation for any purpose, without fee, and without written agreement is
//    hereby granted, provided that the above copyright notice, the following
//    two paragraphs and the authors appear in all copies of this software.
//
//    IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
//    DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
//    OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
//    HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//    THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
//    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//    AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
//    ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
//    PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
//
//    Please maintain this header in its entirety when copying/modifying
//    these files.
//
// ************************************************************************
// unitTest.c - Implements unit tests for the microcontroller comm protocol
// ************************************************************************
// A very simple test runner, runAllTests(), executes the
// tests. ASSERT statements provide verification.

#include "dataXfer.h"
#include <string.h>
#include <stdio.h>

// Tests for the command-finding state machine
// ===========================================

// Run all normal chars through the machine
void findChar() {
  char c_in, c_out;

  for (c_in = 0; c_in < ESCAPED_CMD; c_in++) {
    CMD_OUTPUT cmdOutput = stepCommandFindMachine(c_in, &c_out);
    ASSERT(cmdOutput == OUTPUT_CMD_CHAR);
    ASSERT(c_in == c_out);
  }
  for (c_in = ESCAPED_CMD + 1; c_in <= ((char) 0xFF); c_in++) {
    CMD_OUTPUT cmdOutput = stepCommandFindMachine(c_in, &c_out);
    ASSERT(cmdOutput == OUTPUT_CMD_CHAR);
    ASSERT(c_in == c_out);
  }
}

// Run an escaped command through the machine.
void findEscapedCommandChar() {
  char c_out;

  CMD_OUTPUT cmdOutput = stepCommandFindMachine(CMD_TOKEN, &c_out);
  ASSERT(cmdOutput == OUTPUT_CMD_NONE);
  cmdOutput = stepCommandFindMachine(ESCAPED_CMD, &c_out);
  ASSERT(cmdOutput == OUTPUT_CMD_CHAR);
  ASSERT(c_out == CMD_TOKEN);
}

// Look for all the normal (unescaped) commands.
void findCommand() {
  char c_in, c_out;

  for (c_in = 0; c_in < ESCAPED_CMD; c_in++) {
    CMD_OUTPUT cmdOutput = stepCommandFindMachine(CMD_TOKEN, &c_out);
    ASSERT(cmdOutput == OUTPUT_CMD_NONE);
    cmdOutput = stepCommandFindMachine(c_in, &c_out);
    ASSERT(cmdOutput == OUTPUT_CMD_CMD);
    ASSERT(c_in == c_out);
  }
  for (c_in = ESCAPED_CMD + 1; c_in <= ((char) 0xFF); c_in++) {
    CMD_OUTPUT cmdOutput = stepCommandFindMachine(CMD_TOKEN, &c_out);
    ASSERT(cmdOutput == OUTPUT_CMD_NONE);
    cmdOutput = stepCommandFindMachine(c_in, &c_out);
    ASSERT(cmdOutput == OUTPUT_CMD_CMD);
    ASSERT(c_in == c_out);
  }
}

// Run an escaped command through the machine
void findEscapedCommand() {
  char c_out;

  CMD_OUTPUT cmdOutput = stepCommandFindMachine(CMD_TOKEN, &c_out);
  ASSERT(cmdOutput == OUTPUT_CMD_NONE);
  cmdOutput = stepCommandFindMachine(CMD_TOKEN, &c_out);
  ASSERT(cmdOutput == OUTPUT_CMD_NONE);
  cmdOutput = stepCommandFindMachine(ESCAPED_CMD, &c_out);
  ASSERT(cmdOutput == OUTPUT_CMD_CMD);
  ASSERT(c_out == CMD_TOKEN);
}

// Verify that the sequence CMD_TOKEN CMD_TOKEN CMD_TOKEN is recognized
// as a repeated wait.
void findRepeatedWait() {
  char c_out;

  CMD_OUTPUT cmdOutput = stepCommandFindMachine(CMD_TOKEN, &c_out);
  ASSERT(cmdOutput == OUTPUT_CMD_NONE);
  cmdOutput = stepCommandFindMachine(CMD_TOKEN, &c_out);
  ASSERT(cmdOutput == OUTPUT_CMD_NONE);
  cmdOutput = stepCommandFindMachine(CMD_TOKEN, &c_out);
  ASSERT(cmdOutput == OUTPUT_CMD_REPEATED_WAIT);
}

// **todo**: Cases still to unit test:
//
// - (repeated wait)
// - CMD_TOKEN CMD_TOKEN c (repeated command)
void findRepeatedCommand() {
  char c_out;

  CMD_OUTPUT cmdOutput = stepCommandFindMachine(CMD_TOKEN, &c_out);
  ASSERT(cmdOutput == OUTPUT_CMD_NONE);
  cmdOutput = stepCommandFindMachine(CMD_TOKEN, &c_out);
  ASSERT(cmdOutput == OUTPUT_CMD_NONE);
  cmdOutput = stepCommandFindMachine(0, &c_out);
  ASSERT(cmdOutput == OUTPUT_CMD_REPEATED_CMD);
  ASSERT(c_out == 0);
}


// Tests for the receive state machine
// ===================================

void sendData(uint8_t* pu8_data, uint u_len);


// Sending a normal char shold report that char received.
void sendOneNormalChar(
// The character to send. This character will NOT
// be esacaped -- an 0x55 will be sent as just an
// 0x55.
  char c_charToSend) {
  stepReceiveMachine(c_charToSend);
  ASSERT(isReceiveMachineChar());
  ASSERT(getReceiveMachineOutChar() == c_charToSend);
};


// Sending a letter should report that letter received
void sendLetter() {
  sendOneNormalChar('c');
}


// Sending the char 0x00 shold report that char receivd
void send0x00() {
  sendOneNormalChar(0x00);
}


// Sending the char 0xFF shold report that char receivd
void send0xFF() {
  sendOneNormalChar((char) 0xFF);
}


// Check sending an escaped command
void sendEscapedCommand() {
  uint8_t au8_data[] = { CMD_TOKEN, ESCAPED_CMD };
  sendData(au8_data, 2);

  ASSERT(isReceiveMachineChar());
  ASSERT(getReceiveMachineOutChar() == CMD_TOKEN);
}

// Set up the xferData structure for receiving
// some data.
void setupXferData(
  // Index of data to be received
  uint u_index,
  // Length (in bytes) of data to be received
  uint u_len) {

  // Check params
  ASSERT(u_index < NUM_XFER_VARS);
  ASSERT(u_len <= 256);

  // Set up structure.
  xferVar[u_index].u8_size = u_len - 1;  // Value is length-1
#ifdef MICROCONTROLLER
  // On a microcontroller, provide a statically-allocated buffer
  // to hold received data.
  static uint8_t au8_data[256];
  xferVar[u_index].pu8_data = au8_data;
#else
  // On the PC, dynamically allocate it; initDataXfer() will free this.
  xferVar[u_index].pu8_data = (uint8_t*) malloc(u_len*sizeof(uint8_t));
#endif
  assignBit(u_index, TRUE);
}

// Check sending a one-byte piece of data
// 0x00 == 000000 00 : index 0, length 0 (1 byte)
void sendOneByteData() {
  // Set up index 0 for 1 byte of data
  setupXferData(0, 1);
  uint8_t au8_data[] = { CMD_TOKEN, 0x00, 0x12 };
  sendData(au8_data, 3);

  ASSERT(isReceiveMachineData());
  ASSERT(getReceiveMachineIndex() == 0);
  ASSERT(xferVar[0].pu8_data[0] == au8_data[2]);
}


// Check sending a four-byte piece of data
void sendFourBytesData() {
  setupXferData(0, 4);
  // 0x03: index 0, length 3 (4 bytes); following are data bytes
  uint8_t au8_data[] = { CMD_TOKEN, 0x03, 0x00, 0x01, 0x02, 0x03 };
  sendData(au8_data, 6);

  ASSERT(isReceiveMachineData());
  ASSERT(getReceiveMachineIndex() == 0);
  uint i;
  for (i = 0; i < 4; i++)
    ASSERT(xferVar[0].pu8_data[i] == i);
}


// Check sending a four-byte piece of data which
// contains four CMD_TOKEN bytes.
// 0x00 == 000000 11 : index 0, length 3 (4 bytes)
void sendFourBytesCmdTokenData() {
  setupXferData(0, 4);
  // 0x03: index 0, length 3; following are data bytes
  uint8_t au8_data[] = { CMD_TOKEN, 0x03, CMD_TOKEN, ESCAPED_CMD,
                         CMD_TOKEN, ESCAPED_CMD, CMD_TOKEN, ESCAPED_CMD, CMD_TOKEN, ESCAPED_CMD
                       };
  sendData(au8_data, 10);

  ASSERT(isReceiveMachineData());
  ASSERT(getReceiveMachineIndex() == 0);
  uint i;
  for (i = 0; i < 4; i++)
    ASSERT(xferVar[0].pu8_data[i] == ((uint8_t) CMD_TOKEN));
}


// Send a repeated command and make sure both an error is reported
// and data can be received (error recovery works).
void sendRepeatedCommand() {
  // 0x00: index 0, length 1.
  uint8_t au8_data[] = { CMD_TOKEN, CMD_TOKEN, 0x00, 0xFF };
  // Send first three bytes and check for repeated command.
  setupXferData(0, 1);
  sendData(au8_data, 2);
  // Send third byte manually, since it reports an error
  stepReceiveMachine(au8_data[2]);
  ASSERT(getReceiveMachineError() == ERR_REPEATED_CMD);
  // Send last byte
  sendData(&au8_data[3], 1);

  ASSERT(isReceiveMachineData());
  ASSERT(getReceiveMachineIndex() == 0);
  ASSERT(xferVar[0].pu8_data[0] == au8_data[3]);
}


// Send a command of CMD_TOKEN
void sendCommandCmdToken() {
  // Set up and send command
  uint u_index = getVarIndex(CMD_TOKEN);
  uint u_len = getVarLength(CMD_TOKEN);
  setupXferData(u_index, u_len);
  uint8_t au8_data[7] = { CMD_TOKEN, CMD_TOKEN, ESCAPED_CMD };
  uint u;
  for (u = 0; u < u_len; u++)
    au8_data[u + 3] = u;
  sendData(au8_data, 3 + u_len);

  // Check received data
  ASSERT(isReceiveMachineData());
  ASSERT(getReceiveMachineIndex() == u_index);
  for (u = 0; u < u_len; u++)
    ASSERT(xferVar[u_index].pu8_data[u] == u);
}


// Send a repeated command followed by a command of CMD_TOKEN
void sendRepeatedCommandCmdToken() {
  // Set up and send command
  uint u_index = getVarIndex(CMD_TOKEN);
  uint u_len = getVarLength(CMD_TOKEN);
  setupXferData(u_index, u_len);
  uint8_t au8_data[8] = { CMD_TOKEN, CMD_TOKEN, CMD_TOKEN, ESCAPED_CMD };
  uint u;
  for (u = 0; u < u_len; u++)
    au8_data[u + 4] = u;
  // Send first two bytes (no error yet)
  sendData(au8_data, 2);
  // Send 3rd command (should be a repeated command error)
  stepReceiveMachine(au8_data[2]);
  ASSERT(getReceiveMachineError() == ERR_REPEATED_CMD);
  // Send remaining data (ESCAPED_CMD followed by u_len data bytes)
  sendData(&au8_data[3], 1 + u_len);

  // Check received data
  ASSERT(isReceiveMachineData());
  ASSERT(getReceiveMachineIndex() == u_index);
  for (u = 0; u < u_len; u++)
    ASSERT(xferVar[u_index].pu8_data[u] == u);
}

// Test timeout detection.
void sendWithTimeout() {
  setupXferData(0, 4);
  // 0x03: index 0, length 3 (4 bytes); following are data bytes
  uint8_t au8_data[] = { CMD_TOKEN, 0x03, 0x00, 0x01, 0x02, 0x03 };

  // Create a timeout at each point in the transmission of the data
  uint u;
  for (u = 0; u < 6; u++) {
    // Start fresh each time
    resetReceiveMachine();
    // Send u-1 bytes with no timeout
    if (u > 0)
      sendData(au8_data, u);
    // Send the final byte with a timeout
    notifyOfTimeout();
    RECEIVE_ERROR re = stepReceiveMachine(au8_data[u]);
    // Except for the first byte, make sure a timeout is reported
    if (u > 0)
      ASSERT(re == ERR_TIMEOUT);
  }
}

// Test sending a second command before the first completes.
void sendInterruptedCommand() {
  setupXferData(0, 4);
  // CMD_TOKEN, 0x03: index 0, length 3 (4 bytes)
  uint8_t au8_data[] = { CMD_TOKEN, 0x03, 0x00, CMD_TOKEN, 0x03, 0x00, 0x01, 0x02, 0x03 };

  // Send the first command then interrupt it
  sendData(au8_data, 4);
  RECEIVE_ERROR re = stepReceiveMachine(au8_data[4]);
  ASSERT(re == ERR_INTERRUPTED_CMD);
  clearReceiveMachineError();

  // Make sure the second command competes
  sendData(au8_data + 5, 4);
  ASSERT(isReceiveMachineData());
  ASSERT(getReceiveMachineIndex() == 0);
  uint i;
  for (i = 0; i < 4; i++)
    ASSERT(xferVar[0].pu8_data[i] == i);
}

// Test sending data to an unspecified index
void sendToUnspecifiedIndex() {
  // CMD_TOKEN, 0x03: index 0, length 3 (4 bytes)
  uint8_t au8_data[] = { CMD_TOKEN, 0x03 };

  sendData(au8_data, 1);
  RECEIVE_ERROR re = stepReceiveMachine(au8_data[1]);
  ASSERT(re == ERR_UNSPECIFIED_INDEX);
}

// Test sending data to an index beyond the end of the variable
// storage area
void sendToHighIndex() {
  // This test only works if NUM_XFER_VARS is less than the max;
  // otherwise, there's no "beyond" index to test.
  ASSERT(NUM_XFER_VARS < MAX_NUM_XFER_VARS);

  // CMD_TOKEN, 0x03: index NUM_XFER_VARS, length 3 (4 bytes)
  uint8_t au8_data[] = { CMD_TOKEN, (NUM_XFER_VARS << 2) | 0x03 };
  sendData(au8_data, 1);
  RECEIVE_ERROR re = stepReceiveMachine(au8_data[1]);
  ASSERT(re == ERR_INDEX_TOO_HIGH);
}

// Test sending incorrectly-sized data to a variable.
void sendWithWrongSize() {
  setupXferData(0, 4);
  // CMD_TOKEN, 0x03: index 0, length 2 (3 bytes)
  uint8_t au8_data[] = { CMD_TOKEN, 0x02 };
  sendData(au8_data, 1);
  RECEIVE_ERROR re = stepReceiveMachine(au8_data[1]);
  ASSERT(re == ERR_VAR_SIZE_MISMATCH);
}

// Test sending long data to an unspecified index
void sendLongToUnspecifiedIndex() {
  // CMD_TOKEN, CMD_LONG_VAR, 0x03: index 0, length 3 (4 bytes)
  uint8_t au8_data[] = { CMD_TOKEN, CMD_LONG_VAR, 0x03 };

  sendData(au8_data, 2);
  RECEIVE_ERROR re = stepReceiveMachine(au8_data[2]);
  ASSERT(re == ERR_UNSPECIFIED_INDEX);
}

// Test sending long data to an index beyond the end of the variable
// storage area
void sendLongToHighIndex() {
  // This test only works if NUM_XFER_VARS is less than the max;
  // otherwise, there's no "beyond" index to test.
  ASSERT(NUM_XFER_VARS < MAX_NUM_XFER_VARS);

  // CMD_TOKEN, CMD_LONG_VAR, index NUM_XFER_VARS, length 3 (4 bytes)
  uint8_t au8_data[] = { CMD_TOKEN, CMD_LONG_VAR, NUM_XFER_VARS, 0x03 };
  sendData(au8_data, 2);
  RECEIVE_ERROR re = stepReceiveMachine(au8_data[2]);
  ASSERT(re == ERR_INDEX_TOO_HIGH);
}

// Test sending incorrectly-sized data to a long variable.
void sendLongWithWrongSize() {
  setupXferData(0, 4);
  // CMD_TOKEN, CMD_LONG_BAR, index 0, length 2 (3 bytes)
  uint8_t au8_data[] = { CMD_TOKEN, CMD_LONG_VAR, 0, 0x02 };
  sendData(au8_data, 3);
  RECEIVE_ERROR re = stepReceiveMachine(au8_data[3]);
  ASSERT(re == ERR_VAR_SIZE_MISMATCH);
}

// Test sending 256 bytes of data.
void sendLongData() {
  uint i;

  setupXferData(0, 256);
  // CMD_TOKEN, CMD_LONG_VAR, index 0, length 0xFF = 256 bytes
  uint8_t au8_data[256 + 5] = { CMD_TOKEN, CMD_LONG_VAR, 0, 0xFF };
  // Fill array with data, escaping the CMD_TOKEN.
  // Do horrible casts (to 8 bit, then to uint) to avoid sign-extension
  // problems that make this loop run too far (0xAA < 0, without a cast
  // is sign-extended to 0xFFFFFFAA, which is a *NOT* 0x00AA).
  for (i = 0; i <= ((uint) ((uint8_t) CMD_TOKEN)); i++)
    au8_data[i + 4] = i;
  au8_data[i + 4] = ESCAPED_CMD;
  for (; i < 256; i++)
    au8_data[i + 5] = i;
  // Send it, then check the data received
  sendData(au8_data, 256 + 5);
  for (i = 0; i < 256; i++)
    ASSERT(xferVar[0].pu8_data[i] == i);
}

#ifdef MICROCONTROLLER
// Test sending data to a read-only variable. Only applies to the
// microcontroller.
void sendReadOnly() {
  // Set up index 0 for 1 byte of data, read-only
  setupXferData(0, 1);
  assignBit(0, FALSE);
  uint8_t au8_data[] = { CMD_TOKEN, 0x00 };
  sendData(au8_data, 1);

  RECEIVE_ERROR re = stepReceiveMachine(au8_data[1]);
  ASSERT(re == ERR_READ_ONLY_VAR);
}


// Test sending a var spec
void sendVarSpecMicrocontroller() {
  // Set up index 0 for 1 byte of data, read-only
  setupXferData(0, 1);
  uint8_t au8_data[] = { CMD_TOKEN, CMD_SEND_ONLY };
  sendData(au8_data, 1);

  RECEIVE_ERROR re = stepReceiveMachine(au8_data[1]);
  ASSERT(re == ERR_MICROCONTROLLER_VAR_SPEC);
}
#endif

#ifndef MICROCONTROLLER
// Used to create strings with commands in them below.
#define CMD_TOKEN_STR "\xAA"
#define CMD_SEND_ONLY_STR "\xFE"
#define CMD_SEND_RECEIVE_VAR_STR "\xFF"

// Test sending a variable specification
void sendVarSpec() {
  //                                                   index, length, size,   format,   name,       description
  uint8_t au8_data[17] = CMD_TOKEN_STR CMD_SEND_ONLY_STR "\x00" "\x0C"  "\x03"  "%x\x00"  "test\x00"  "ing";
  sendData(au8_data, 17);
  ASSERT(isReceiveMachineSpec());
  ASSERT(xferVar[0].u8_size == 3);
  ASSERT(xferVar[0].pu8_data != NULL);   // Verify that some storage was allocated for this variable
  ASSERT(!isVarWriteable(0));
  ASSERT(strcmp(xferVar[0].psz_format, "%x") == 0);
  ASSERT(strcmp(xferVar[0].psz_name, "test") == 0);
  ASSERT(strcmp(xferVar[0].psz_desc, "ing") == 0);
}

// Test sending a variable specification, then sending a different spec
void resendVarSpec() {
  sendVarSpec();
  //                                                   index, length, size,   format,   name,       description
  uint8_t au8_data[17] = CMD_TOKEN_STR CMD_SEND_ONLY_STR "\x00" "\x0C"  "\xFF"  "%y\x00"  "book\x00"  "let";
  sendData(au8_data, 17);
  ASSERT(isReceiveMachineSpec());
  ASSERT(xferVar[0].u8_size == 255);
  ASSERT(xferVar[0].pu8_data != NULL);   // Verify that some storage was allocated for this variable
  ASSERT(!isVarWriteable(0));
  ASSERT(strcmp(xferVar[0].psz_format, "%y") == 0);
  ASSERT(strcmp(xferVar[0].psz_name, "book") == 0);
  ASSERT(strcmp(xferVar[0].psz_desc, "let") == 0);
}

// Test sending a variable specification
void sendWriteableVarSpec() {
  //                                                          index, length, size,   format,   name,       description
  uint8_t au8_data[17] = CMD_TOKEN_STR CMD_SEND_RECEIVE_VAR_STR "\x00" "\x0C"  "\x03"  "%x\x00"  "test\x00"  "ing";
  sendData(au8_data, 17);
  ASSERT(isReceiveMachineSpec());
  ASSERT(xferVar[0].u8_size == 3);
  ASSERT(xferVar[0].pu8_data != NULL);   // Verify that some storage was allocated for this variable
  ASSERT(isVarWriteable(0));
  ASSERT(strcmp(xferVar[0].psz_format, "%x") == 0);
  ASSERT(strcmp(xferVar[0].psz_name, "test") == 0);
  ASSERT(strcmp(xferVar[0].psz_desc, "ing") == 0);
}

// Test sending a variable specification with no strings
void sendEmptyVarSpec() {
  //                                                         index, length, size
  uint8_t au8_data[6] = CMD_TOKEN_STR CMD_SEND_RECEIVE_VAR_STR "\x00" "\x00"  "\x03";
  sendData(au8_data, 5);
  ASSERT(isReceiveMachineSpec());
  ASSERT(xferVar[0].u8_size == 3);
  ASSERT(xferVar[0].pu8_data != NULL);   // Verify that some storage was allocated for this variable
  ASSERT(isVarWriteable(0));
  ASSERT(strcmp(xferVar[0].psz_format, "") == 0);
  ASSERT(strcmp(xferVar[0].psz_name, "") == 0);
  ASSERT(strcmp(xferVar[0].psz_desc, "") == 0);
}

// Test sending a variable specification with only a format
void sendFormatOnlyVarSpec() {
  //                                                         index, length, size,  format
  uint8_t au8_data[8] = CMD_TOKEN_STR CMD_SEND_RECEIVE_VAR_STR "\x00" "\x03"  "\x03" "%x";
  sendData(au8_data, 8);
  ASSERT(isReceiveMachineSpec());
  ASSERT(xferVar[0].u8_size == 3);
  ASSERT(xferVar[0].pu8_data != NULL);   // Verify that some storage was allocated for this variable
  ASSERT(isVarWriteable(0));
  ASSERT(strcmp(xferVar[0].psz_format, "%x") == 0);
  ASSERT(strcmp(xferVar[0].psz_name, "") == 0);
  ASSERT(strcmp(xferVar[0].psz_desc, "") == 0);
}

// Test sending a variable specification with only a format
void sendNameOnlyVarSpec() {
  //                                                          index, length, size, format, name
  uint8_t au8_data[11] = CMD_TOKEN_STR CMD_SEND_RECEIVE_VAR_STR "\x00" "\x06" "\x03" "\x00"  "test";
  sendData(au8_data, 11);
  ASSERT(isReceiveMachineSpec());
  ASSERT(xferVar[0].u8_size == 3);
  ASSERT(xferVar[0].pu8_data != NULL);   // Verify that some storage was allocated for this variable
  ASSERT(isVarWriteable(0));
  ASSERT(strcmp(xferVar[0].psz_format, "") == 0);
  ASSERT(strcmp(xferVar[0].psz_name, "test") == 0);
  ASSERT(strcmp(xferVar[0].psz_desc, "") == 0);
}

// Test sending a spec followed by actual data
void sendVarSpecAndData() {
  sendVarSpec();

  // 0x03: index 0, length 3 (4 bytes); following are data bytes
  uint8_t au8_data[] = { CMD_TOKEN, 0x03, 0x00, 0x01, 0x02, 0x03 };
  sendData(au8_data, 6);

  ASSERT(isReceiveMachineData());
  ASSERT(getReceiveMachineIndex() == 0);
  for (uint i = 0; i < 4; i++)
    ASSERT(xferVar[0].pu8_data[i] == i);
}
#endif

// Run a sequence of characters through the receive state machine,
// verifying that nothing is received until the final character
// and that no errors occurred.
void sendData(uint8_t* pu8_data, uint u_len) {
  while (u_len--) {
    RECEIVE_ERROR re = stepReceiveMachine(*pu8_data++);
    ASSERT(re == ERR_NONE);
    if (u_len)
      ASSERT(getReceiveMachineState() != STATE_RECV_START);
  }
}
//@}

// \name Tests for the specify and send functions
//@{

// The length of an array of characters used to check OUT_CHAR's usage.
static size_t st_outCharLen = 0;

// An index into the array of check characters.
static size_t st_outCharIndex = 0;

// A pointer to an array containing the expected characters to be output.
static uint8_t* au8_outCharData = NULL;

// Reset all the OUT_CHAR associated data (the variables above).
void clearOutChar() {
  st_outCharLen = 0;
  st_outCharIndex = 0;
  au8_outCharData = NULL;
}

// An outChar function which simply checks to see that the output character
// matches the expected string.
#ifdef __cplusplus
extern "C"
#endif
void testOutChar(uint8_t c) {
ASSERT(au8_outCharData != NULL);
ASSERT(st_outCharIndex < st_outCharLen);
ASSERT(au8_outCharData[st_outCharIndex++] == c);
}

/** Test support: ASSERT if an exception isn't thrown.
 *  \param code Code which when executed should cause a specific ASSERT.
 *  \param expectedMsg String the ASSERT should throw.
 *  This macro expects to test ASSERT statements of the form
 *  ASSERT("a string" && someCondition). The "a string" portion is always
 *  true, but also provides a hackish way to name an assert. To make
 *  testing easier, only the text inside the quotes is tested: the
 *  expectedString in this case is "a string".
 */
#ifdef __cplusplus
#define REQUIRE_ASSERT(code, expectedMsg)      \
  do {                          \
    BOOL didAssert = FALSE;   \
    try {                     \
      code;                 \
    } catch (char* psz_msg) { \
      didAssert = strncmp(psz_msg, expectedMsg, strlen(expectedMsg)) ? FALSE : TRUE;     \
    }                         \
    ASSERT(didAssert);        \
  } while (FALSE)
#else
// Do nothing, since C doesn't support exceptions
#define REQUIRE_ASSERT(code, expectedMsg) (void) 0
#endif

// Send to a index that's too high
void testSendIndexTooHigh() {
  REQUIRE_ASSERT(sendVar(NUM_XFER_VARS + 1), "sendVar:indexTooHigh");
}

// Send to an unconfigured index
void testSendIndexUnspecificed() {
  REQUIRE_ASSERT(sendVar(0), "sendVar:indexNotSpecified");
}

// Send to a read-only variable (PC only)
#ifndef MICROCONTROLLER
void testSendToReadOnly() {
  // Set up index 0 for 1 byte of data, read-only
  setupXferData(0, 1);
  assignBit(0, FALSE);
  // Expected transmission
  REQUIRE_ASSERT(sendVar(0), "sendVar:notWriteable");
}
#endif

// A macro to send a variable and check the resulting output
void checkSendVar(uint8_t u8_index, uint u_len, uint8_t* au8_data) {
  au8_outCharData = au8_data;
  st_outCharLen = u_len;
  sendVar(u8_index);
  ASSERT(st_outCharIndex == st_outCharLen);
}

// Send a one-byte variable
void testSendOneByteVar() {
  // Set up index 0 for 1 byte of data
  setupXferData(0, 1);
  // Assign data
  xferVar[0].pu8_data[0] = 0;
  // Expected transmission
  uint8_t au8_data[3] = { CMD_TOKEN, 0x00, 0x00 };
  checkSendVar(0, 3, au8_data);
}

// Send a one-byte variable that needs to be escaped
void testSendOneEscapedByteVar() {
  // Set up index 0 for 1 byte of data
  setupXferData(0, 1);
  // Assign data
  xferVar[0].pu8_data[0] = CMD_TOKEN;
  // Expected transmission
  uint8_t au8_data[4] = { CMD_TOKEN, 0, CMD_TOKEN, ESCAPED_CMD };
  checkSendVar(0, 4, au8_data);
}

// Send a four-byte variable
void testSendFourByteVar() {
  // Set up index 0 for 4 bytes of data
  setupXferData(0, 4);
  // Assign data
  uint u_i;
  for (u_i = 0; u_i < 4; u_i++)
    xferVar[0].pu8_data[u_i] = u_i;
  // Expected transmission
  uint8_t au8_data[6] = { CMD_TOKEN, 0x03, 0x00, 0x01, 0x02, 0x03 };
  checkSendVar(0, 6, au8_data);
}

// Send a 256-byte variable
void testSend256ByteVar() {
  // Set up index 0 for 256 bytes of data
  setupXferData(0, 256);
  // Assign data
  uint u_i;
  for (u_i = 0; u_i < 256; u_i++)
    xferVar[0].pu8_data[u_i] = u_i;
  // Expected transmission
  uint8_t au8_data[261] = { CMD_TOKEN, CMD_LONG_VAR, 0x00, 0xFF };
  // Do horrible casts (to 8 bit, then to uint) to avoid sign-extension
  // problems that make this loop run too far (0xAA < 0, without a cast
  // is sign-extended to 0xFFFFFFAA, which is a *NOT* 0x00AA).
  for (u_i = 0; u_i <= ((uint) ((uint8_t) CMD_TOKEN)); u_i++)
    au8_data[u_i + 4] = u_i;
  au8_data[u_i + 4] = ESCAPED_CMD;
  for (; u_i < 256; u_i++)
    au8_data[u_i + 5] = u_i;
  checkSendVar(0, 261, au8_data);
}

// Specify an index that's too high
void testSpecifyIndexTooHigh() {
  // Dummy buffer to hold variable data. Initialize it to something to
  // avoid "unreferenced local variable" warnings.
  uint8_t au8_buf[1] = {0};
  REQUIRE_ASSERT(specifyVar(NUM_XFER_VARS + 1, au8_buf, 1, TRUE, "", "", ""),
                 "specifyVar:indexTooHigh");
}

// Specify with NULL data
void testSpecifyNullData() {
  REQUIRE_ASSERT(specifyVar(0, NULL, 1, TRUE, "", "", ""),
                 "specifyVar:nullData");
}

// Specify with an invalid size
void testSpecifyInvalidSize() {
    // Dummy buffer to hold variable data. Initialize it to something to
    // avoid "unreferenced local variable" warnings.
  uint8_t au8_buf[1] = {0};
  REQUIRE_ASSERT(specifyVar(0, au8_buf, 0, TRUE, "", "", ""),
                 "specifyVar:invalidSize");
  REQUIRE_ASSERT(specifyVar(0, au8_buf, 257, TRUE, "", "", ""),
                 "specifyVar:invalidSize");
}

// Minimally specify a variable
void testSpecifyMinimalVar() {
  // Dummy buffer to hold variable data
  uint8_t au8_buf[1];
  // Expected transmission
  uint8_t au8_data[5 + 3] = { CMD_TOKEN, CMD_SEND_RECEIVE_VAR, 0 /* u_varIndex */,
                              3 /* length of rest - 1 */, /* var size - 1 */ 0, /* data */ 0, 0, 0
                            };
  // Test it out
  au8_outCharData = au8_data;
  st_outCharLen = 8;
  specifyVar(0 /* u_varIndex */, au8_buf, 1 /* u_size */,
             TRUE /* b_isWriteable */, "", "", "");
  ASSERT(st_outCharIndex == st_outCharLen);
  // Make sure data structure was updated
  ASSERT(xferVar[0].pu8_data == au8_buf);
  ASSERT(xferVar[0].u8_size == 0);
  ASSERT(isVarWriteable(0));
}

// Test specifying a var with a format string which exceeds the max length.
// Also check a send-only variable.
void testSpecifyLongFormat() {
  // Dummy buffer to hold variable data
  uint8_t au8_buf[1];
  // Expected transmission
  uint8_t au8_data[5 + 255] = { CMD_TOKEN, CMD_SEND_ONLY, 0 /* u_varIndex */,
                                255 /* length of rest - 1 */, 0 /* var size - 1 */,
                                /* format string -- filled in below */
                              };
  uint u_i;
  for (u_i = 5; u_i < 5 + 255; u_i++)
    au8_data[u_i] = ' ';

  // Test it out
  au8_outCharData = au8_data;
  st_outCharLen = 4 + 256;
  specifyVar(0 /* u_varIndex */, au8_buf, 1 /* u_size */,
             FALSE /* b_isWriteable */, // a looong format string
             "                                                                                "
             "                                                                                "
             "                                                                                "
             "                                                                                "
             "                                                                                ",
             "" /* name */, "" /* description */);
  ASSERT(st_outCharIndex == st_outCharLen);
  // Make sure data structure was updated
  ASSERT(xferVar[0].pu8_data == au8_buf);
  ASSERT(xferVar[0].u8_size == 0);
  ASSERT(!isVarWriteable(0));
}

// Test specifying a var with a name string which exceeds the max length.
// Also check a send-only variable.
void testSpecifyLongName() {
  // Dummy buffer to hold variable data
  uint8_t au8_buf[1];
  // Expected transmission
  uint8_t au8_data[6 + 254] = { CMD_TOKEN, CMD_SEND_ONLY, 0 /* u_varIndex */,
                                255 /* length of rest - 1 */, 0 /* var size - 1 */,
				0 /* empty format string */,
				/* name -- filled in below */
                              };
  uint u_i;
  for (u_i = 6; u_i < 6 + 254; u_i++)
    au8_data[u_i] = ' ';

  // Test it out
  au8_outCharData = au8_data;
  st_outCharLen = 4 + 256;
  specifyVar(0 /* u_varIndex */, au8_buf, 1 /* u_size */,
             FALSE /* b_isWriteable */, "" /* format */, // a looong name string
             "                                                                                "
             "                                                                                "
             "                                                                                "
             "                                                                                "
             "                                                                                ",
             "" /* description */);
  ASSERT(st_outCharIndex == st_outCharLen);
  // Make sure data structure was updated
  ASSERT(xferVar[0].pu8_data == au8_buf);
  ASSERT(xferVar[0].u8_size == 0);
  ASSERT(!isVarWriteable(0));
}

// Test specifying a var with a description string which exceeds the max length.
// Also check a send-only variable.
void testSpecifyLongDesc() {
  // Dummy buffer to hold variable data
  uint8_t au8_buf[1];
  // Expected transmission
  uint8_t au8_data[7 + 253] = { CMD_TOKEN, CMD_SEND_ONLY, 0 /* u_varIndex */,
                                255 /* length of rest - 1 */, 0 /* var size - 1 */,
				/* empty format string */ 0, /* empty name string */ 0,
				/* description -- filled in below */
                              };
  uint u_i;
  for (u_i = 7; u_i < 7 + 253; u_i++)
    au8_data[u_i] = ' ';

  // Test it out
  au8_outCharData = au8_data;
  st_outCharLen = 4 + 256;
  specifyVar(0 /* u_varIndex */, au8_buf, 1 /* u_size */,
             FALSE /* b_isWriteable */, "" /* format */, "" /* name */,
	     // a looong description string
             "                                                                                "
             "                                                                                "
             "                                                                                "
             "                                                                                "
             "                                                                                ");
  ASSERT(st_outCharIndex == st_outCharLen);
  // Make sure data structure was updated
  ASSERT(xferVar[0].pu8_data == au8_buf);
  ASSERT(xferVar[0].u8_size == 0);
  ASSERT(!isVarWriteable(0));
}

// Send to a index that's too high
void testFormatIndexTooHigh() {
  // Dummy buffer to hold variable data. Initialize it to something to
  // avoid "unreferenced local variable" warnings.
  char psz_buf[200] = {0};
  REQUIRE_ASSERT(formatVar(NUM_XFER_VARS + 1, psz_buf), "formatVar:indexTooHigh");
}

// Send to an unconfigured index
void testFormatIndexUnspecificed() {
  // Dummy buffer to hold variable data. Initialize it to something to
  // avoid "unreferenced local variable" warnings.
  char psz_buf[200] = {0};
  REQUIRE_ASSERT(formatVar(0, psz_buf), "formatVar:indexNotSpecified");
}


// A list of functions which comprise tests to be run,
// terminated with a NULL.
void (*afp_testList[])() = {
  findChar,
  findEscapedCommandChar,
  findCommand,
  findEscapedCommand,
  findRepeatedWait,
  findRepeatedCommand,
  sendLetter,
  send0x00,
  send0xFF,
  sendEscapedCommand,
  sendOneByteData,
  sendFourBytesData,
  sendFourBytesCmdTokenData,
  sendRepeatedCommand,
  sendCommandCmdToken,
  sendRepeatedCommandCmdToken,
  sendWithTimeout,
  sendInterruptedCommand,
  sendToUnspecifiedIndex,
  sendToHighIndex,
  sendWithWrongSize,
  sendLongToUnspecifiedIndex,
  sendLongToHighIndex,
  sendLongWithWrongSize,
  sendLongData,
#ifdef MICROCONTROLLER
  testSpecifyLongFormat,
  testSpecifyLongName,
  testSpecifyLongDesc,
  sendReadOnly,
  sendVarSpecMicrocontroller,
  testSpecifyMinimalVar,
#else
  sendVarSpec,
  sendWriteableVarSpec,
  resendVarSpec,
  sendEmptyVarSpec,
  sendFormatOnlyVarSpec,
  sendNameOnlyVarSpec,
  sendVarSpecAndData,
  testSendToReadOnly,
  testFormatIndexTooHigh,
  testFormatIndexUnspecificed,
#endif
  testSendIndexTooHigh,
  testSendIndexUnspecificed,
  testSendOneByteVar,
  testSendOneEscapedByteVar,
  testSendFourByteVar,
  testSend256ByteVar,
  testSpecifyIndexTooHigh,
  testSpecifyNullData,
  testSpecifyInvalidSize,
  NULL
};


// Execute one test. This resets the state machines before a run to
// create a clean slate for every test.
void runTest(
// Index of test to run. NO BOUNDS CHECKING is performed
// on this index. Be careful.
  uint u_index) {
  initDataXfer();
  clearOutChar();
  (afp_testList[u_index])();  // Execute the specified test
}

// Run all the tests by executing everything in the list of tests.
void runAllTests() {
  uint u_index;
  for (u_index = 0; afp_testList[u_index] != NULL; u_index++) {
    printf("Running test %d...", u_index + 1);
    runTest(u_index);
    printf("success.\n");
  }
  // Free all memory used by the last test (for the PC; the microcontroller
  // doesn't dynamically allocate memory).
  initDataXfer();
  printf("All %d tests passed.\n", u_index);
}
