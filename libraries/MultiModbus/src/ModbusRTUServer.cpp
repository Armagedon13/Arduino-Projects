/*
  This file is part of the ArduinoModbus library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <errno.h>

extern "C" {
#include "libmodbus/modbus.h"
#include "libmodbus/modbus-rtu.h"
}

#include "ModbusRTUServer.h"


ModbusRTUServer::ModbusRTUServer(RS485& rs485) : _rs485(&rs485)
{
  _bAddrMapping = false;
}

ModbusRTUServer::~ModbusRTUServer()
{
}

int ModbusRTUServer::begin(int id, unsigned long baudrate, uint16_t config)
{
  modbus_t* mb = modbus_new_rtu(_rs485, baudrate, config);

  if (!ModbusServer::begin(mb, id)) {
    return 0;
  }

  modbus_connect(mb);

  return 1;
}

int ModbusRTUServer::begin(RS485& rs485, int id, unsigned long baudrate, uint16_t config)
{
  _rs485 = &rs485;
  return begin(id, baudrate, config);
}

int ModbusRTUServer::poll()
{
  uint8_t request[MODBUS_RTU_MAX_ADU_LENGTH];

  int requestLength = modbus_receive(_mb, request);

  if (requestLength > 0) {
    if (_bAddrMapping) {
      addressMapping(request);
    }
    modbus_reply(_mb, request, requestLength, &_mbMapping);
    return 1;
  }
  return 0;
}

void ModbusRTUServer::setAddrMapping(bool val) {
  _bAddrMapping = val;
}

void ModbusRTUServer::addressMapping(unsigned char request[])
{
}