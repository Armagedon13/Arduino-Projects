/*
    Modbus.cpp - Source for Modbus Base Library
    Copyright (C) 2014 André Sarmento Barbosa
    Copyright (C) 2023 Pascal JEAN aka epsilonrt
*/
#include "Modbus.h"

//-------------------------------------------------------------------------------
Modbus::Modbus()  :
  _regs_head (0),
  _regs_last (0),
  _additional_data (0),
  _frame (nullptr),
  _len (0),
  _reply (0),
  _debug (nullptr) {
}

//-------------------------------------------------------------------------------
// private
TRegister *Modbus::searchRegister (word address) {
  TRegister *reg = _regs_head;
  //if there is no register configured, bail
  if (reg == 0) {
    return (0);
  }
  //scan through the linked list until the end of the list or the register is found.
  //return the pointer.
  do {
    if (reg->address == address) {
      return (reg);
    }
    reg = reg->next;
  }
  while (reg);
  return (0);
}

//-------------------------------------------------------------------------------
// private
void Modbus::addReg (word address, word value) {
  TRegister *newreg;

  newreg = (TRegister *) malloc (sizeof (TRegister));
  newreg->address = address;
  newreg->value   = value;
  newreg->edata   = 0;
  newreg->next    = 0;

  if (_regs_head == 0) {
    _regs_head = newreg;
    _regs_last = _regs_head;
  }
  else {
    //Assign the last register's next pointer to newreg.
    _regs_last->next = newreg;
    //then make temp the last register in the list.
    _regs_last = newreg;
  }
}

//-------------------------------------------------------------------------------
// private
bool Modbus::setReg (word address, word value) {
  TRegister *reg;
  //search for the register address
  reg = searchRegister (address);
  //if found then assign the register value to the new value.
  if (reg) {
    reg->value = value;
    return true;
  }
  else {
    return false;
  }
}

//-------------------------------------------------------------------------------
// private
word Modbus::reg (word address) {
  TRegister *reg;
  reg = searchRegister (address);
  if (reg) {
    return (reg->value);
  }
  else {
    return (0);
  }
}

//-------------------------------------------------------------------------------
// private
bool  Modbus::setRegBounds (word address, word min, word max) {
  TRegister *reg;

  reg = searchRegister (address);

  if (reg) {

    if (!reg->edata) {

      reg->edata = new TExtData;
    }
    if (reg->edata) {

      reg->edata->min = min;
      reg->edata->max = max;
      return true;
    }
  }
  return false;
}

//-------------------------------------------------------------------------------
// private
bool  Modbus::regOutOfBounds (word address, word value) {
  TRegister *reg;

  reg = searchRegister (address);

  if (reg) {

    if (reg->edata) {

      return (value < reg->edata->min) || (value > reg->edata->max);
    }
  }
  return false;
}

//-------------------------------------------------------------------------------
// protected
void Modbus::receivePDU (byte *frame) {
  byte fcode  = frame[0];
  word field1 = (word) frame[1] << 8 | (word) frame[2];
  word field2 = (word) frame[3] << 8 | (word) frame[4];

  switch (fcode) {

    case MB_FC_WRITE_REG:
      //field1 = reg, field2 = value
      writeSingleRegister (field1, field2);
      break;

    case MB_FC_WRITE_REGS:
      //field1 = startreg, field2 = status
      writeMultipleRegisters (frame, field1, field2, frame[5]);
      break;

    case MB_FC_READ_REGS:
      //field1 = startreg, field2 = numregs
      readRegisters (fcode, field1, field2);
      break;

      #ifndef USE_HOLDING_REGISTERS_ONLY
    case MB_FC_READ_INPUT_REGS:
      //field1 = startreg, field2 = numregs
      readRegisters (fcode, field1, field2);
      break;

    case MB_FC_READ_INPUT_STAT:
    case MB_FC_READ_COILS:
      //field1 = startreg, field2 = numregs
      readBits (fcode, field1, field2);
      break;

    case MB_FC_WRITE_COIL:
      //field1 = reg, field2 = status
      writeSingleCoil (field1, field2);
      break;

    case MB_FC_WRITE_COILS:
      //field1 = startreg, field2 = numoutputs
      writeMultipleCoils (frame, field1, field2, frame[5]);
      break;

    case MB_FC_REPORT_SERVER_ID:
      reportServerId();
      break;

      #endif

    default:
      exceptionResponse (fcode, MB_EX_ILLEGAL_FUNCTION);
  }
}

//-------------------------------------------------------------------------------
// private
void Modbus::exceptionResponse (const byte fcode, const byte excode) {

  _len = 2;
  _frame = (byte *) realloc (_frame, _len);
  _frame[0] = fcode + 0x80;
  _frame[1] = excode;

  _reply = MB_REPLY_NORMAL;
}

/* Func 06: Write Single Register
    Request
      Function code     1 Byte  0x06
      Register Address  2 Bytes 0x0000 to 0xFFFF
      Register Value    2 Bytes 0x0000 to 0xFFFF
    Response
      Function code     1 Byte  0x06
      Register Address  2 Bytes 0x0000 to 0xFFFF
      Register Value    2 Bytes 0x0000 to 0xFFFF
*/
//-------------------------------------------------------------------------------
// private
void Modbus::writeSingleRegister (const word reg, const word value) {

  if (hregOutOfBounds (reg, value)) {
    exceptionResponse (MB_FC_WRITE_REG, MB_EX_ILLEGAL_VALUE);
    return;
  }

  //Check Address and execute (reg exists?)
  if (!setHreg (reg, value)) {
    exceptionResponse (MB_FC_WRITE_REG, MB_EX_ILLEGAL_ADDRESS);
    return;
  }

  //Check for failure ? Why ???
  /*
    if (hreg (reg) != value) {
    exceptionResponse (MB_FC_WRITE_REG, MB_EX_SLAVE_FAILURE);
    return;
    }
  */
  _reply = MB_REPLY_ECHO; // reply with received frame
}

/* Func 16: Write Multiple registers
    Request
      Function code         1 Byte        0x10
      Starting Address      2 Bytes       0x0000 to 0xFFFF
      Quantity of Registers 2 Bytes       0x0001 to 0x007B
      Byte Count            1 Byte        2 x N*
      Registers Value       N* x 2 Bytes  value
       N = Quantity of Registers
    Response
      Function code         1 Byte        0x10
      Starting Address      2 Bytes       0x0000 to 0xFFFF
      Quantity of Registers 2 Bytes       1 to 123 (0x7B)
*/
//-------------------------------------------------------------------------------
// private
void Modbus::writeMultipleRegisters (const byte *frame, const word startreg, const word numoutputs, const byte bytecount) {
  //Check value
  if (numoutputs < 1 || numoutputs > 123 || bytecount != 2 * numoutputs) {
    exceptionResponse (MB_FC_WRITE_REGS, MB_EX_ILLEGAL_VALUE);
    return;
  }

  //Check Address (startreg...startreg + numregs - 1)
  for (word k = 0; k < numoutputs; k++) {
    if (!searchRegister (startreg + TRegister::HregOffset + k)) {
      exceptionResponse (MB_FC_WRITE_REGS, MB_EX_ILLEGAL_ADDRESS);
      return;
    }
  }

  for (word i = 0; i < numoutputs; i++) {

    word val = (word) frame[6 + i * 2] << 8 | (word) frame[7 + i * 2];
    if (hregOutOfBounds (startreg + i, val)) {

      exceptionResponse (MB_FC_WRITE_REG, MB_EX_ILLEGAL_VALUE);
      return;
    }
    setHreg (startreg + i, val);
  }

  //Clean frame buffer
  free (_frame);
  _len = 5;
  _frame = (byte *) malloc (_len);
  if (!_frame) {
    exceptionResponse (MB_FC_WRITE_REGS, MB_EX_SLAVE_FAILURE);
    return;
  }

  _frame[0] = MB_FC_WRITE_REGS;
  _frame[1] = startreg >> 8;
  _frame[2] = startreg & 0x00FF;
  _frame[3] = numoutputs >> 8;
  _frame[4] = numoutputs & 0x00FF;
  _reply = MB_REPLY_NORMAL;
}


/*
  Func 03: Read Holding Registers
    Request
      Function code         1 Byte  0x03
      Starting Address      2 Bytes 0x0000 to 0xFFFF
      Quantity of Registers 2 Bytes 1 to 125 (0x7D)
    Response
      Function code         1 Byte  0x03
      Byte count            1 Byte  2 x N*
      Register value        N* x 2 Bytes
     N = Quantity of Registers
  Func 04: Read Input Registers
    Request
      Function code               1 Byte        0x04
      Starting Address            2 Bytes       0x0000 to 0xFFFF
      Quantity of Input Registers 2 Bytes       0x0001 to 0x007D
    Response
      Function code               1 Byte        0x04
      Byte count                  1 Byte        2 x N*
      Input Registers             N* x 2 Bytes
     N = Quantity of Input Registers
*/
//-------------------------------------------------------------------------------
// private
void Modbus::readRegisters (const byte fcode, const word startreg, const word numregs) {

  // Check value (numregs)
  if (numregs < 1 || numregs > 0x7D) {

    exceptionResponse (fcode, MB_EX_ILLEGAL_VALUE);
    return;
  }

  // calculate the query reply message length
  // for each register queried add 2 bytes
  _len = 2 + numregs * 2;

  _frame = (byte *) realloc (_frame, _len);
  if (!_frame) {

    exceptionResponse (fcode, MB_EX_SLAVE_FAILURE);
    return;
  }

  _frame[0] = fcode;
  _frame[1] = _len - 2;   //byte count

  const word offset = (fcode == MB_FC_READ_REGS) ? TRegister::HregOffset : TRegister::IregOffset;
  for (word i = 0; i < numregs; i++) {

    const word address = startreg + offset + i;
    if (searchRegister (address)) {

      // retrieve the value from the register bank for the current register
      word val = reg (address);

      // write the high byte of the register value
      _frame[2 + i * 2]  = val >> 8;
      // write the low byte of the register value
      _frame[3 + i * 2] = val & 0xFF;
    }
    else {

      exceptionResponse (fcode, MB_EX_ILLEGAL_ADDRESS);
      return;
    }
  }

  _reply = MB_REPLY_NORMAL;
}

//-------------------------------------------------------------------------------
// protected
void Modbus::debugMessage (bool reply) {

  if (_len && isDebug()) {
    char str[3];  // buffer for sprintf
    for (uint8_t i = 0; i < _len; i++) {

      sprintf (str, "%02X", _frame[i]); // str stores the string representation of the byte, in hex form
      _debug->write (!reply ? '[' : '<');
      _debug->print (str);
      _debug->write (!reply ? ']' : '>');
    }
    _debug->println ("");
  }
}

#ifndef USE_HOLDING_REGISTERS_ONLY
//-------------------------------------------------------------------------------
/* Func 01:  Read Coils
    Request
      Function code     1 Byte  0x01
      Starting Address  2 Bytes 0x0000 to 0xFFFF
      Quantity of coils 2 Bytes 1 to 2000 (0x7D0)
    Response
      Function code     1 Byte  0x01
      Byte count        1 Byte  N*
      Coil Status       n Byte  n = N or N+1
     N = Quantity of Outputs / 8, if the remainder is different of 0 -> N = N+1
   Func 02: Read Discrete Inputs
    Request
      Function code       1 Byte      0x02
      Starting Address    2 Bytes     0x0000 to 0xFFFF
      Quantity of Inputs  2 Bytes     1 to 2000 (0x7D0)
    Response
      Function code       1 Byte      0x02
      Byte count          1 Byte      N*
      Input Status        N* x 1 Byte
     N = Quantity of Inputs / 8, if the remainder is different of 0 -> N = N+1
*/
//-------------------------------------------------------------------------------
// private
void Modbus::readBits (const byte fcode, const word startreg, const word numregs) {

  //Check value (numregs)
  if (numregs < 0x0001 || numregs > 0x07D0) {
    
    exceptionResponse (fcode, MB_EX_ILLEGAL_VALUE);
    return;
  }

  // Determine the message length = function type, byte count and
  // for each group of 8 registers the message length increases by 1
  _len = 2 + numregs / 8;
  if (numregs % 8) {
    _len++;  // Add 1 to the message length for the partial byte.
  }

  _frame = (byte *) realloc (_frame, _len);
  if (!_frame) {

    exceptionResponse (fcode, MB_EX_SLAVE_FAILURE);
    return;
  }

  _frame[0] = fcode;
  _frame[1] = _len - 2; // byte count (_len - function code and byte count)

  const word offset = (fcode == MB_FC_READ_COILS) ? TRegister::CoilOffset : TRegister::IstsOffset;
  for (word i = 0; i < numregs; i++) {

    const word address = startreg + offset + i;
    if (searchRegister (address)) {

      word byteIndex = (i / 8) + 2;
      byte bitIndex = i % 8;
      if (reg (address) == 0xFF00) {

        bitSet (_frame[byteIndex], bitIndex);
      }
      else {

        bitClear (_frame[byteIndex], bitIndex);
      }
    }
    else {

      exceptionResponse (fcode, MB_EX_ILLEGAL_ADDRESS);
      return;
    }
  }

  _reply = MB_REPLY_NORMAL;
}


/* Func 05: Write Single Coil
    Request
      Function code   1 Byte  0x05
      Output Address  2 Bytes 0x0000 to 0xFFFF
      Output Value    2 Bytes 0x0000 or 0xFF00
  Response
      Function code   1 Byte  0x05
      Output Address  2 Bytes 0x0000 to 0xFFFF
      Output Value    2 Bytes 0x0000 or 0xFF00
*/
//-------------------------------------------------------------------------------
// private
void Modbus::writeSingleCoil (const word reg, const word status) {
  //Check value (status)
  if (status != 0xFF00 && status != 0x0000) {
    exceptionResponse (MB_FC_WRITE_COIL, MB_EX_ILLEGAL_VALUE);
    return;
  }

  //Check Address and execute (reg exists?)
  if (!setCoil (reg, status != 0)) {
    exceptionResponse (MB_FC_WRITE_COIL, MB_EX_ILLEGAL_ADDRESS);
    return;
  }

  //Check for failure ?? Why ?
  /*
    if (coil (reg) != (bool) status) {
    exceptionResponse (MB_FC_WRITE_COIL, MB_EX_SLAVE_FAILURE);
    return;
    }
  */

  _reply = MB_REPLY_ECHO; // reply with received frame
}

/* Func 15: Write Multiple Coils
    Request
      Function code       1 Byte  0x0F
      Starting Address    2 Bytes 0x0000 to 0xFFFF
      Quantity of Outputs 2 Bytes 0x0001 to 0x07B0
      Byte Count          1 Byte  N* (Quantity of Outputs / 8, if the remainder is different of 0 -> N = N+1)
      Outputs Value       N* x 1 Byte
    Response
      Function code       1 Byte  0x0F
      Starting Address    2 Bytes 0x0000 to 0xFFFF
      Quantity of Outputs 2 Bytes 0x0001 to 0x07B0
*/
//-------------------------------------------------------------------------------
// private
void Modbus::writeMultipleCoils (const byte *frame, const word startreg, const word numoutputs, const byte bytecount) {

  // Check value
  word bytecount_calc = numoutputs / 8;
  if (numoutputs % 8) {

    bytecount_calc++;
  }

  if (numoutputs < 0x0001 || numoutputs > 0x07B0 || bytecount != bytecount_calc) {

    exceptionResponse (MB_FC_WRITE_COILS, MB_EX_ILLEGAL_VALUE);
    return;
  }

  for (word i = 0; i < numoutputs; i++) {

    const word address = startreg + i;
    if (searchRegister (address + TRegister::CoilOffset)) {

      word byteIndex = (i / 8) + 6;
      byte bitIndex = i % 8;
      setCoil (address, bitRead (frame[byteIndex], bitIndex));
    }
    else {

      exceptionResponse (MB_FC_WRITE_COILS, MB_EX_ILLEGAL_ADDRESS);
      return;
    }
  }

  _len = 5;
  _frame = (byte *) realloc (_frame, _len);
  if (!_frame) {

    exceptionResponse (MB_FC_WRITE_COILS, MB_EX_SLAVE_FAILURE);
    return;
  }

  _frame[0] = MB_FC_WRITE_COILS;
  _frame[1] = startreg >> 8;
  _frame[2] = startreg & 0x00FF;
  _frame[3] = numoutputs >> 8;
  _frame[4] = numoutputs & 0x00FF;

  _reply = MB_REPLY_NORMAL;
}

/*
  Func 17:  Report Server ID
    Request
      Function code         1 Byte            0x11
    Response
      Function code         1 Byte            0x11
      Byte Count            1 Byte
      Server ID             device specific
      Run Indicator Status  1 Byte            0x00 = OFF, 0xFF = ON
      Additional Data
*/
//-------------------------------------------------------------------------------
// private
void Modbus::reportServerId() {

  _len = 4;
  if (_additional_data) {

    _len += strlen (_additional_data);
  }

  _frame = (byte *) realloc (_frame, _len);
  if (!_frame) {

    exceptionResponse (MB_FC_REPORT_SERVER_ID, MB_EX_SLAVE_FAILURE);
    return;
  }

  _frame[0] = MB_FC_REPORT_SERVER_ID;
  _frame[1] = _len - 2;   //byte count
  _frame[2] = 0x00; // Server ID
  _frame[3] = 0xFF; // Run Indicator Status
  if (_additional_data) { // Additional Data

    strcpy ( (char *) &_frame[4], _additional_data);
  }
  _reply = MB_REPLY_NORMAL;
}

//-------------------------------------------------------------------------------
int Modbus::setAdditionalServerData (const char data[]) {

  free (_additional_data);
  if (data) {
    size_t l = strlen (data) + 1;

    _additional_data = (char *) malloc (l);
    if (_additional_data) {

      strcpy (_additional_data, data);
      return l;
    }
  }
  return 0;
}

//-------------------------------------------------------------------------------
#endif // USE_HOLDING_REGISTERS_ONLY not defined
