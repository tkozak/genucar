// include the library code:
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "CommandParser.h"

const int cmdListSize=4;
const String cmdList[]={"stop","drive","pclr","prep"};
const unsigned int cmdRequiredArgs[]={0, 0, 0, 1};

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 2, en = 10;
const int d4 = 4, d5 = 5, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
CommandParser cmdParser = CommandParser(Serial, '\n', ' ');
String cmd;


void executeCommand(String cmd, unsigned int numArgs, int args[]) {
  int cmdIx = -1;
  for (int i=0; i<cmdListSize; i++) { // search for command in list
    if (cmd==cmdList[i]) {
      cmdIx=i;
      break;
    }
  }
  if (cmdIx<0) return;
  if (numArgs<cmdRequiredArgs[cmdIx]) return;

  switch (cmdIx) {
    case(0): // stop
      lcd.setCursor(0, 0);
      lcd.print("stop            ");
      break;
    case(1): // drive
      lcd.setCursor(0, 0);
      lcd.print("drive           ");
      break;
  }

}


void setup() {

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Ready");
  Serial.begin(9600);

}

void loop() {
  // check for commands from serial interface
  while (cmdParser.readNextCommand()>0) {
    executeCommand(cmdParser.cmd, cmdParser.numArgs, cmdParser.args);
  }
  delay(10);

}
