/*
    Modbus.h - Header for Modbus Base Library
    Copyright (C) 2014 André Sarmento Barbosa
    Copyright (C) 2023 Pascal JEAN aka epsilonrt
*/
#include "Arduino.h"

#pragma once

#define MAX_REGS     32 // Unused !
#define MAX_FRAME   128 // Unused !
//#define USE_HOLDING_REGISTERS_ONLY

// Function Codes
enum {
  MB_FC_READ_COILS       = 0x01, ///< Read Coils (Output) Status 0xxxx
  MB_FC_READ_INPUT_STAT  = 0x02, ///< Read Input Status (Discrete Inputs) 1xxxx
  MB_FC_READ_REGS        = 0x03, ///< Read Holding Registers 4xxxx
  MB_FC_READ_INPUT_REGS  = 0x04, ///< Read Input Registers 3xxxx
  MB_FC_WRITE_COIL       = 0x05, ///< Write Single Coil (Output) 0xxxx
  MB_FC_WRITE_REG        = 0x06, ///< Preset Single Register 4xxxx
  MB_FC_WRITE_COILS      = 0x0F, ///< Write Multiple Coils (Outputs) 0xxxx
  MB_FC_WRITE_REGS       = 0x10, ///< Write block of contiguous registers 4xxxx
  MB_FC_REPORT_SERVER_ID = 0x11, ///< Report Server ID
};

// Exception Codes
enum {
  MB_EX_ILLEGAL_FUNCTION = 0x01, ///< Function Code not Supported
  MB_EX_ILLEGAL_ADDRESS  = 0x02, ///< Address not in Range
  MB_EX_ILLEGAL_VALUE    = 0x03, ///< Data Value not in Range
  MB_EX_SLAVE_FAILURE    = 0x04, ///< Slave Deive Fails to process request
};

// Reply Types
enum {
  MB_REPLY_OFF    = 0x01, ///< No reply
  MB_REPLY_ECHO   = 0x02, ///< Return the message from the bus master as an reply
  MB_REPLY_NORMAL = 0x03, ///< Reply with adding slave data
};

#ifndef __DOXYGEN__
struct TExtData {
  word min;
  word max;
};
struct TRegister {
  word address;
  word value;
  TExtData *edata;
  TRegister *next;
  enum {
    CoilOffset = 1,
    IstsOffset = 10001,
    IregOffset = 30001,
    HregOffset = 40001
  };
};
#endif

/**
   @class Modbus
   @brief Modbus base class
*/
class Modbus {
    #ifndef __DOXYGEN__
  private:
    TRegister *_regs_head;
    TRegister *_regs_last;
    char *_additional_data;

    void readRegisters (const byte fcode, const word startreg, const word numregs);
    void writeSingleRegister (const word reg, const word value);
    void writeMultipleRegisters (const byte *frame, const word startreg, const word numoutputs, const byte bytecount);
    void exceptionResponse (const byte fcode, const byte excode);

    #ifndef USE_HOLDING_REGISTERS_ONLY
    void readBits (const byte fcode, const word startreg, const word numregs);
    void writeSingleCoil (const word reg, const word status);
    void writeMultipleCoils (const byte *frame, const word startreg, const word numoutputs, const byte bytecount);
    #endif

    TRegister *searchRegister (word addr);

    void addReg (word address, word value = 0);
    bool setReg (word address, word value);
    word reg (word address);
    bool setRegBounds (word address, word min, word max);
    bool regOutOfBounds (word address, word value);

    // @deprecated
    inline bool Reg (word address, word value) {
      return setReg (address, value);
    }
    // @deprecated
    inline word Reg (word address) {
      return reg (address);
    }

  protected:
    byte *_frame;
    byte  _len;
    byte  _reply;
    Print *_debug;
    void receivePDU (byte *frame);
    void reportServerId();
    void debugMessage (bool reply = false);
    #endif

  public:
    /**
      @brief Default constructor
    */
    Modbus();

    /**
      @brief Add a holding register to the list
      @param offset register offset (PDU addressing: 0-9999)
      @param value default value
    */
    inline void addHreg (word offset, word value = 0) {
      this->addReg (offset + TRegister::HregOffset, value);
    }
    /**
      @brief Change the value of a holding register
      This value will be returned when bus read, the master can also modify it.
      @param offset register offset (PDU addressing: 0-9999)
      @param value new value
      @return true, false if register not found.
    */
    inline bool setHreg (word offset, word value) {
      return setReg (offset + TRegister::HregOffset, value);
    }
    /**
      @brief Return the value of a holding register
      @param offset register offset (PDU addressing: 0-9999)
      @return register value
    */
    inline word hreg (word offset) {
      return reg (offset + TRegister::HregOffset);
    }
    /**
      @brief Sets the bounds of a holding register

      @param offset register offset (PDU addressing: 0-9999)
      @param min min value
      @param max max value
      @return true, false if register not found.
    */
    inline bool setHregBounds (word offset, word min, word max) {
      return setRegBounds (offset + TRegister::HregOffset, min, max);
    }
    /**
      @brief Checks if the value is outside the bounds of a holging register

      @param offset register offset (PDU addressing: 0-9999)
      @param value value to check
      @return true if the value is outside the bounds, false otherwise
    */
    inline bool hregOutOfBounds (word offset, word value) {
      return regOutOfBounds (offset + TRegister::HregOffset, value);
    }
    ;
    /**
        @brief Change the value of a holding register
         This value will be returned when bus read, the master can also modify it.
         @deprecated removed in next major release, use setHreg() instead.
        @param offset register offset (PDU addressing: 0-9999)
        @param value new value
        @return true, false if register not found.
    */
    inline bool Hreg (word offset, word value) {
      return setHreg (offset, value);
    }
    /**
      @brief Return the value of a holding register
       @deprecated removed in next major release, use hreg() instead.
      @param offset register offset (PDU addressing: 0-9999)
      @return register value
    */
    inline word Hreg (word offset) {
      return hreg (offset);
    }

    /**
       @brief Enable debug mode
    */
    inline void setDebug (Print &print) {
      _debug = &print;
      _debug->println ("Modbus: debug enabled....");
    }

    /**
       @brief Returns true if debug mode is enabled
    */
    inline bool isDebug () {
      return _debug != nullptr;
    }

    /**
       @brief Print a debug message, only if debug mode is enabled
       @param msg message to print
    */
    inline void debug (const char *msg) {
      if (_debug) {
        _debug->println (msg);
      }
    }

    #ifndef USE_HOLDING_REGISTERS_ONLY

    /**
      @brief Add a coil
      @param offset coil offset (PDU addressing: 0-9999)
      @param value default value
    */
    void addCoil (word offset, bool value = false) {
      this->addReg (offset + TRegister::CoilOffset, value ? 0xFF00 : 0x0000);
    }
    /**
      @brief Add a discrete input
      @param offset input offset (PDU addressing: 0-9999)
      @param value default value
    */
    inline void addIsts (word offset, bool value = false) {
      this->addReg (offset + TRegister::IstsOffset, value ? 0xFF00 : 0x0000);
    }
    /**
      @brief Add a input register
      @param offset register offset (PDU addressing: 0-9999)
      @param value default value
    */
    inline void addIreg (word offset, word value = 0) {
      this->addReg (offset + TRegister::IregOffset, value);
    }
    /**
      @brief Change the value of a coil
       This value will be returned when bus read, the master can also modify it.
      @param offset register offset (PDU addressing: 0-9999)
      @param value new value
      @return true, false if coil not found.
    */
    inline bool setCoil (word offset, bool value) {
      return setReg (offset + TRegister::CoilOffset, value ? 0xFF00 : 0x0000);
    }
    /**
      @brief Change the value of a discrete input
       This value will be returned when bus read,.
      @param offset input offset (PDU addressing: 0-9999)
      @param value new value
      @return true, false if input not found.
    */
    inline bool setIsts (word offset, bool value) {
      return setReg (offset + TRegister::IstsOffset, value ? 0xFF00 : 0x0000);
    }
    /**
      @brief Change the value of an input register
       This value will be returned when bus read.
      @param offset register offset (PDU addressing: 0-9999)
      @param value new value
      @return true, false if register not found.
    */
    inline bool setIreg (word offset, word value) {
      return setReg (offset + TRegister::IregOffset, value);
    }
    /**
      @brief Return the value of a coil
      @param offset register offset (PDU addressing: 0-9999)
      @return coil value
    */
    inline bool coil (word offset) {
      return (reg (offset + TRegister::CoilOffset) == 0xFF00);
    }
    /**
      @brief Return the value of a discrete input
      @param offset input offset (PDU addressing: 0-9999)
      @return input value
    */
    inline bool ists (word offset) {
      return (reg (offset + TRegister::IstsOffset) == 0xFF00);
    }
    /**
      @brief Return the value of an input register
      @param offset register offset (PDU addressing: 0-9999)
      @return register value
    */
    inline word ireg (word offset) {
      return reg (offset + TRegister::IregOffset);
    }
    /**
      @brief Change the value of a coil
       This value will be returned when bus read, the master can also modify it.
       @deprecated removed in next major release, use setCoil() instead.
      @param offset register offset (PDU addressing: 0-9999)
      @param value new value
      @return true, false if coil not found.
    */
    inline bool Coil (word offset, bool value) {
      return setCoil (offset, value);
    }
    /**
      @brief Change the value of a discrete input
       This value will be returned when bus read,.
       @deprecated removed in next major release, use setIsts() instead.
      @param offset input offset (PDU addressing: 0-9999)
      @param value new value
      @return true, false if input not found.
    */
    inline bool Ists (word offset, bool value) {
      return setIsts (offset, value);
    }
    /**
      @brief Change the value of an input register
       This value will be returned when bus read.
       @deprecated removed in next major release, use setIreg() instead.
      @param offset register offset (PDU addressing: 0-9999)
      @param value new value
      @return true, false if register not found.
    */
    inline bool Ireg (word offset, word value) {
      return setIreg (offset, value);
    }
    /**
      @brief Return the value of a coil
       @deprecated removed in next major release, use coil() instead.
      @param offset register offset (PDU addressing: 0-9999)
      @return coil value
    */
    inline bool Coil (word offset) {
      return coil (offset);
    }
    /**
      @brief Return the value of a discrete input
       @deprecated removed in next major release, use ists() instead.
      @param offset input offset (PDU addressing: 0-9999)
      @return input value
    */
    inline bool Ists (word offset) {
      return ists (offset);
    }
    /**
      @brief Return the value of an input register
       @deprecated removed in next major release, use ireg() instead.
      @param offset register offset (PDU addressing: 0-9999)
      @return register value
    */
    inline word Ireg (word offset) {
      return ireg (offset);
    }
    /**
      @brief Sets additional Data for Report Server ID function
      @param data data string
      @return the number of chars gets from data (249 max)
    */
    int setAdditionalServerData (const char data[]);
    #endif
};
