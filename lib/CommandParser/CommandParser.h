/*
  CommandParser.h
  Library for parsing character stream into commands and command parameters. Commands and parameters
  are returned to user for execution in the master code.
  Created by T. Kozak, January 2018
*/

#ifndef CommandParser_h
#define CommandParser_h

#include <Arduino.h>

const unsigned int maxArguments=6;
const unsigned int cmdLength=16;
const unsigned int msgLength=64;

class CommandParser {
public:
  CommandParser(Stream& streamInstance, char endChar, char delimiter);
  int readNextCommand();
  String cmd;
  int args[maxArguments];
  unsigned int numArgs;
private:
  Stream* streamPtr;
  char _endChar, _delimiter;
  String msg;

};


#endif
