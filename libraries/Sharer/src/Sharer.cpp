/**************************************************************************/
/*!
				███████╗██╗  ██╗ █████╗ ██████╗ ███████╗██████╗
				██╔════╝██║  ██║██╔══██╗██╔══██╗██╔════╝██╔══██╗
				███████╗███████║███████║██████╔╝█████╗  ██████╔╝
				╚════██║██╔══██║██╔══██║██╔══██╗██╔══╝  ██╔══██╗
				███████║██║  ██║██║  ██║██║  ██║███████╗██║  ██║
				╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝

	@file Sharer.cpp
	License: MIT (see LICENSE)

	Author: Rufus31415

	Website: https://rufus31415/github.io
	
	Sharer is a connector library that facilitates communication between an Arduino board and a desktop application (see Sharer.NET library).
	Features :
		- Desktop applicaion can Read/Write variable on Arduino
		- Remote call function on Arduino with arguments and retrieve returned value in the desktop application
		- Get board information
*/
/**************************************************************************/

#include "Sharer.h"

// Number of available bytes in the user message buffer
int SharerClass::available(void) {
	if (_userMessageTail > _userMessageHead) return _SHARER_USER_RECEIVE_BUFFER_SIZE + _userMessageHead - _userMessageTail;
	else return _userMessageHead - _userMessageTail;
}

// Peek a byte in the user message buffer
int SharerClass::peek(void) {
	if (_userMessageHead == _userMessageTail) return -1; // nothing to read
	else return _receiveBuffer[_userMessageTail];
}

// Read and pop a byte in the user message buffer (-1 if no adata available)
int SharerClass::read(void) {
	if (_userMessageHead == _userMessageTail) return -1; // nothing to read

	// get next tail index
	int next = _userMessageTail + 1; 
	if (next >= _SHARER_USER_RECEIVE_BUFFER_SIZE) next = 0;

	int data = _receiveBuffer[_userMessageTail];
	_userMessageTail = next;

	return data;
}

// Flush the user message buffer
void SharerClass::flush(void) {
	_userMessageHead = 0;
	_userMessageTail = 0;
}

// Send a byte to Desktop application. Should not be used inside a shared function because Sharer is already busy (-1 is then returned)
size_t SharerClass::write(uint8_t x) {
	if (SharerClass::_SharerReceiveState::Free == _receiveState) return  _parentStream->write(x);
	else return -1;
}


void SharerClass::init() {

}

// Init Sharer with default Serial communication
void SharerClass::init(unsigned long baud) {
	Serial.begin(baud);
	_parentStream = &Serial;
	init();
}

// Initialize Sharer with a custom stream like Serial2
void SharerClass::init(Stream* parentStream) {
	_parentStream = parentStream;
	init();
}

// Push a new value in user message circular buffer
void SharerClass::_storeNewValue(byte value) {
	int next = _getNextHeadIndex();

	// if buffer is full
	if (next < 0) return;

	_receiveBuffer[_userMessageHead] = value;
	_userMessageHead = next;
}


// Comment/Uncomment this for debug purposes
//#define DEBUG(msg, variable) 		Serial.print(msg); Serial.print(":"); Serial.println(variable);
#define DEBUG(msg, variable) 

// Send header of the Sharer protocole
void SharerClass::_sendHeader(_SharerSentCommand cmd) {
	DEBUG("_sendHeader", (byte)cmd);

	_parentStream->write(_SHARER_START_COMMAND_CHAR);
	_parentStream->write(_lastSentCommandId++);
	_parentStream->write((byte)cmd);
	_parentStream->write(_lastReceivedCommandId);
}

// Main function to call inside Arduino Loop() function
void SharerClass::run() {
	if (_parentStream == NULL) return;

	if (!_initDone) {
		_sendHeader(_SharerSentCommand::GetInfos);
		_printInfos();
		_initDone = true;
	}

	for (int i = 0; i < _SHARER_MAX_BYTE_READ_PER_RUN; i++)
	{
		int16_t receiveByte = _parentStream->read();

		if (receiveByte < 0) return;

		DEBUG("receiveByte", receiveByte)

		switch (_receiveState)
		{
		case SharerClass::_SharerReceiveState::Free:
			if (receiveByte == _SHARER_START_COMMAND_CHAR) { // if a new command arrives
				_receiveState = _SharerReceiveState::MessageID;

				DEBUG("_SHARER_START_COMMAND_CHAR", "");
			}
			else { // else, store value to buffer
				DEBUG("store", receiveByte);
				
				_storeNewValue(receiveByte);
			}
			break;
		case SharerClass::_SharerReceiveState::MessageID:
			DEBUG("MessageID", receiveByte);
			_lastReceivedCommandId = receiveByte;
			_receiveState = _SharerReceiveState::Commande;
			break;
		case SharerClass::_SharerReceiveState::Commande:
			DEBUG("Commande", receiveByte);
			_lastReceivedCommand = (_SharerReceivedCommand)receiveByte;
			_complexCommandStep = 0;
			_handleSimpleCommand();
			break;
		case SharerClass::_SharerReceiveState::ComplexCommandeBody:
			_handleComplexCommand(receiveByte);
			break;
		default:
			break;
		}
	}
}

// A command seems like a Sharer command but it was not, so store values in user message
void SharerClass::_rollBackCommand() {
	// the message was not a command, put data in buffer and go back to free state
	DEBUG("NOT A COMMAND", "");
	_storeNewValue(_SHARER_START_COMMAND_CHAR);
	_storeNewValue((byte)_lastReceivedCommandId);
	_storeNewValue((byte)_lastReceivedCommand);
	_receiveState = _SharerReceiveState::Free;
}

// Size of a Sharer type
int16_t  SharerClass::_sizeof(_SharerFunctionArgType type) {
	switch (type)
	{
	case _SharerFunctionArgType::Typeint8_t:
	case _SharerFunctionArgType::Typeuint8_t:
	case _SharerFunctionArgType::Typebool:
		return 1;

	case _SharerFunctionArgType::Typeint16_t:
	case _SharerFunctionArgType::Typeuint16_t:
		return 2;

	case _SharerFunctionArgType::Typeint32_t:
	case _SharerFunctionArgType::Typeuint32_t:
	case _SharerFunctionArgType::Typefloat:
		return 4;

	case _SharerFunctionArgType::Typeint64_t:
	case _SharerFunctionArgType::Typeuint64_t:
	case _SharerFunctionArgType::Typedouble:
		return 8;

	default:
		return 0;
	}
}

// Handle command without arguments to decode
void SharerClass::_handleSimpleCommand() {
		switch (_lastReceivedCommand)
		{
		case SharerClass::_SharerReceivedCommand::FunctionCount:
			DEBUG("FunctionCount", "");
			_sendHeader();
			_parentStream->write((uint8_t*)&functionList.count, 2);
			_endSend();
			break;

		case SharerClass::_SharerReceivedCommand::AllFunctionsPrototype:
			_sendHeader();
			_parentStream->write((uint8_t*)&functionList.count, 2);
			_printFunctionsPrototype();
			_endSend();
			break;
		case SharerClass::_SharerReceivedCommand::AllVariables:
			_sendHeader();
			_printVariablesDefinition();
			_endSend();
			break;
		case SharerClass::_SharerReceivedCommand::GetInfos:
			_sendHeader();
			_printInfos();
			_endSend();
			break;
		default:
			// else, it is a complex command with arguments to read
			_receiveState = _SharerReceiveState::ComplexCommandeBody;
			break;
		}
	}

// Print a string stored in flash
void SharerClass::_printp(const char *data)
	{
		while (pgm_read_byte(data) != 0x00) {
			_parentStream->write(pgm_read_byte(data++));
		}
		_parentStream->write((byte)0);
	}

// Decode and execute a command with arguments
void SharerClass::_handleComplexCommand(byte receiveByte) {
		static byte stack[6]; // a stack for complex commands to store values

#define STACK_OBJ_ID (*((int16_t*)stack)) // usefull for commands that have to retain function ID or variable ID
#define STACK_OBJ_COUNT (*((int16_t*)&stack[2])) // usefull for commands that have to retain a count int in index 2 and 3

		switch (_lastReceivedCommand)
		{
		case SharerClass::_SharerReceivedCommand::FunctionPrototype:
			DEBUG("FunctionPrototype", "");


			// receive first byte of int function id
			if (_complexCommandStep == 0) {
				stack[0] = receiveByte; // store low byte
				_complexCommandStep++;
			}
			else { // second part
				stack[1] = receiveByte; // store high byte

				_sendHeader();

				// if the asked function id is in the function list
				if (STACK_OBJ_ID < functionList.count) {
					_printFunctionPrototype(STACK_OBJ_ID);
				}
				else {
					_parentStream->write(0xff);
				}

				_endSend();
			}
			break;
		case SharerClass::_SharerReceivedCommand::CallFunction:
			DEBUG("CallFunction", "");

			// Stack :
			// [0][1] int that stores the function id
			// [2] : id of the current parsed argument
			// [3] : pointer offset of the current argument

			switch (_complexCommandStep)
			{
			case 0:// read function id high byte
				stack[0] = receiveByte; // store low byte
				_complexCommandStep++;
				break;

			case 1: // read function id low byte
				stack[1] = receiveByte;

				// raise error if id is out of range
				if (STACK_OBJ_ID >= functionList.count) {
					_sendHeader();
					_parentStream->write((byte)_SharerCallFunctionStatus::FunctionIdOutOfRange);
					_endSend();
				}
				else {
					if (functionList.functions[STACK_OBJ_ID].argumentCount == 0) {
						// call function and answer
						_callFunctionAndAnswer(&functionList.functions[STACK_OBJ_ID]);
					}
					else {
						stack[2] = 0;
						stack[3] = 0;
						_complexCommandStep++;
					}
				}
				break;

			case 2: // Read arguments
				_SharerFunction* fnc;
				fnc = &functionList.functions[STACK_OBJ_ID]; // pointer to the struct that describes the function

				const _SharerFunctionArgument* arg;
				arg = &fnc->Arguments[stack[2]]; // pointer to the current argument

				int16_t size;
				size = _sizeof(arg->value.type);


				// if the argument is known
				if (size > 0) {
					// write argument byte
					*(byte*)((int)(arg->value.pointer) + (int)stack[3]) = receiveByte;

					stack[3]++;

					// Check for next argument
					if (stack[3] >= size) {
						stack[3] = 0;
						stack[2]++;

						// if all argument has been written, call function and send answer
						if (stack[2] >= fnc->argumentCount) {
							_callFunctionAndAnswer(fnc);
						}
					}
				}
				else {
					// send unknown type return status
					_sendHeader();
					_parentStream->write((byte)_SharerCallFunctionStatus::UnknownType);
					_parentStream->write(stack[2]);
					_parentStream->write((byte)arg->value.type);
					_endSend();
				}

				break;
			default:
				break;
			}

			break;
		case SharerClass::_SharerReceivedCommand::ReadVariables:
		case SharerClass::_SharerReceivedCommand::WriteVariables:
			// Stack :
			// [0][1] int that stores the variable id
			// [2][3] int that stores the variable count
			// [4] offset of the current write
			// [5] size of the current variable

			switch (_complexCommandStep) {
			case 0:
				stack[2] = receiveByte; // store variable number to read low byte
				_complexCommandStep++;
				break;
			case 1:
				stack[3] = receiveByte; // store variable number to read high byte

				_sendHeader();

				if (STACK_OBJ_COUNT > 0) {
					_complexCommandStep++;
				}
				else { // if no functions to read
					_endSend();
				}
				break;
			case 2:
				stack[0] = receiveByte; // store variable id to read low byte
				_complexCommandStep++;
				break;
			case 3:
			{
				stack[1] = receiveByte; // store variable id to read low byte

				int16_t variableId = STACK_OBJ_ID;

				if (variableId >= 0 && variableId < variableList.count) {
					int16_t varSize = _sizeof(variableList.variables[variableId].value.type);

					// if the returned type is known
					if (varSize > 0) {
						if (_lastReceivedCommand == SharerClass::_SharerReceivedCommand::ReadVariables) { // for reading send value
							_parentStream->write((byte)_SharerReadVariableStatus::OK);
							for (int i = 0; i < varSize; i++) {
								_parentStream->write(*(byte*)((int)(variableList.variables[variableId].value.pointer) + i));
							}
							_complexCommandStep--;
						}
						else { // for writing, go to next step 
							_complexCommandStep++;
							stack[4] = 0;
							stack[5] = varSize;
						}
					}
					else {
						_parentStream->write((byte)_SharerReadVariableStatus::UnknownType);
						_complexCommandStep--;
					}
				}
				else {
					_parentStream->write((byte)_SharerReadVariableStatus::VariableIdOutOfRange);
					_complexCommandStep--;
				}

				if (_complexCommandStep != 4) {
					// remained number of variable to read
					int16_t remain = --STACK_OBJ_COUNT;
					
					if (remain <= 0) {
						_endSend();
					}
				}
			}
				break;
			case 4:
				*(byte*)((int)(variableList.variables[STACK_OBJ_ID].value.pointer) + stack[4]) = receiveByte;

				stack[4]++;

				if (stack[4] >= stack[5]) {
					// remained number of variable to read
					int16_t remain = --STACK_OBJ_COUNT;

					_parentStream->write((byte)_SharerReadVariableStatus::OK);

					// if nothing else to read, terminate message
					if (remain <= 0) {
						_endSend();
					}
					else {
						_complexCommandStep = 2;
					}
				}
			default:
				break;
			}
			break;
		default:
			_rollBackCommand();
			break;
#undef STACK_OBJ_ID
#undef STACK_OBJ_COUNT
		}

	}

	// Print all function description
	void SharerClass::_printFunctionsPrototype() {
		for (int16_t i = 0; i < functionList.count; i++) {
			_printFunctionPrototype(i);
		}
	}

	// print board info
	void SharerClass::_printInfos() {
		byte data[4];

		_parentStream->write((byte)SHARER_VERSION_MAJOR);
		_parentStream->write((byte)SHARER_VERSION_MINOR);
		_parentStream->write((byte)SHARER_VERSION_FIX);
		_printp(PSTR(SHARER_BOARD));

		*(long*)data = SHARER_F_CPU;
		_parentStream->write(data, 4);

		*(int*)data = SHARER_GCC_VERSION;
		_parentStream->write(data, 2);

		*(long*)data = SHARER___cplusplus;
		_parentStream->write(data, 4);

		_parentStream->write((byte*)&functionList.count, 2);

		*(int*)data = _SHARER_MAX_FUNCTION_COUNT;
		_parentStream->write(data, 2);

		_parentStream->write((byte*)&variableList.count, 2);

		*(int*)data = _SHARER_MAX_VARIABLE_COUNT;
		_parentStream->write(data, 2);
	}

	// Print all variables description
	void SharerClass::_printVariablesDefinition() {
		_parentStream->write((uint8_t*)&variableList.count, 2);
		for (int i = 0; i < variableList.count; i++) {
			_parentStream->write((byte)variableList.variables[i].value.type); // print variable type
			_printp(variableList.variables[i].name); // print variable name
		}

	}

	// Print description of shared function at indice id
	void SharerClass::_printFunctionPrototype(int16_t id) {
		auto fnc = &functionList.functions[id];
		_parentStream->write(fnc->argumentCount);
		_parentStream->write((byte)fnc->returnValue.type);
		_printp(fnc->name);
		for (int i = 0; i < fnc->argumentCount; i++) {
			_parentStream->write((byte)fnc->Arguments[i].value.type);
			_printp(fnc->Arguments[i].name);
		}
	}

	// Execute a function and reply
	void SharerClass::_callFunctionAndAnswer(_SharerFunction * fnc) {
		int16_t retSize = _sizeof(fnc->returnValue.type);

		// if the return type is unknown
		if ((retSize <= 0) && (fnc->returnValue.type != _SharerFunctionArgType::TypeVoid)) {
			_sendHeader();
			_parentStream->write((byte)_SharerCallFunctionStatus::UnknownType);
			_parentStream->write((byte)0xff); // the id for return type is 0xff
			_parentStream->write((byte)fnc->returnValue.type);
			_endSend();
		}
		else {
			// call function
			(*fnc->pointer)();

			_sendHeader();
			// send OK status
			_parentStream->write((byte)_SharerCallFunctionStatus::OK);

			// send return value for none void function
			if (fnc->returnValue.type != _SharerFunctionArgType::TypeVoid) {
				for (int i = 0; i < retSize; i++) {
					_parentStream->write(*(byte*)((int)(fnc->returnValue.pointer) + i));
				}
			}

			_endSend();
		}
	}

// Sharer global instance
SharerClass Sharer;

