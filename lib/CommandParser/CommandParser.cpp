#include <CommandParser.h>


CommandParser::CommandParser(Stream& streamInstance, char endChar, char delimiter) {
  streamPtr = &streamInstance;
//  streamPtr->setTimeout(timeout);
  _endChar = endChar;
  _delimiter = delimiter;
  cmd.reserve(cmdLength);
  msg.reserve(msgLength);
}

int CommandParser::readNextCommand() {
  int i0=0, i1=0, ilast;
  numArgs=0;
  cmd = "";
  if (streamPtr->available()>0) {
    msg = streamPtr->readStringUntil(_endChar); // all data read from stream until endChar
    msg.trim();
    ilast = msg.length(); // index of last character
    i1 = msg.indexOf(_delimiter,i0);  // find delimiter
    if (i1<0) i1=ilast; // if not found, set i1 to last char
    cmd = msg.substring(i0,i1);  // save first substring as command
    cmd.toLowerCase(); // transform to lowercase
    while (i1<ilast) {  // while not end of message
      i0 = i1 + 1;
      i1 = msg.indexOf(_delimiter,i0); // continue search for delimiter
      if (i1<0) i1=ilast;
      args[numArgs]=msg.substring(i0,i1).toInt(); // save mext substring as integer argument
      numArgs++;
    }    
    return numArgs+1;
  } else return 0;
}
