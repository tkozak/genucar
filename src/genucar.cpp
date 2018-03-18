// include the library code:
#include <Arduino.h>
#include <LiquidCrystal.h>
//#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "CommandParser.h"

const int cmdListSize=5;
const String cmdList[]={"stop", "drive", "pclr", "prep", "mpuinit"};
const unsigned int cmdRequiredArgs[]={0, 0, 0, 1, 0};

const int mpu_int = 2;
CommandParser cmdParser = CommandParser(Serial, '\n', ' ');

MPU6050 mpu;
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
const uint16_t fifoCopy = 512;  // when to copy data from fifo (half of buffer size)
uint8_t packetBuffer[64]; // FIFO packet storage buffer

// orientation/motion vars
short quat[4];    // [w, x, y, z]         quaternion container
short accel[3];  // [x, y, z]            accel sensor measurements
short rate[3];         // gyro rate measurement
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

const size_t telemetrySize = 20;
byte telemetry[telemetrySize];

unsigned long sendT0, timems;
const unsigned long sendInterval=100;



// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

//volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpInterrupt() {
    //mpuInterrupt = true;
    mpuIntStatus = mpu.getIntStatus();
    // check for overflow (this should never happen unless our code is too inefficient)
    if (mpuIntStatus & 0x10)  {
        // reset so we can continue cleanly
        mpu.resetFIFO();
      //  Serial.println(F("FIFO overflow!"));
    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x01) {
        fifoCount = mpu.getFIFOCount();
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    //    Serial.print("Data ready, FIFO count: ");
    //    Serial.println(fifoCount);
        // wait until more data i sin the FIFO buffer
        if (fifoCount >= fifoCopy) {
          // read a packet from FIFO
          while (fifoCount >= packetSize) {
            mpu.getFIFOBytes(packetBuffer, packetSize);
            fifoCount -= packetSize;
            mpu.dmpGetQuaternion(quat, packetBuffer);
            mpu.dmpGetAccel(accel, packetBuffer);
            mpu.dmpGetGyro(rate, packetBuffer);
          }
        }
    }
}



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
      break;
    case(1): // drive
      break;
    case(4): //mpuinit
      /*
      Wire.beginTransmission(0x68);
      Wire.write(0x6B);
      Wire.write(0);
      Wire.endTransmission(true);
      delay(1000);*/

      Wire.beginTransmission(0x68);
      Wire.write(0x75);
      Serial.println(Wire.endTransmission(false));
      uint8_t byteread = Wire.requestFrom(0x68,1,true);
      int av = Wire.available();
      int address = Wire.read();
      Serial.println(byteread);
      Serial.println(av);
      Serial.println(address);
      break;
  }
}

void writeTelemetry() {
  memcpy(&telemetry[0], &rate, 6);
  memcpy(&telemetry[6], &accel, 6);
  memcpy(&telemetry[12], &accel, 8);
}


void setup() {

  Serial.begin(115200);

  Wire.begin();
  Wire.setClock(400000);

  Serial.println("Initializing I2C devices...");
  mpu.initialize();
  pinMode(mpu_int,INPUT);
  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  /*
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
  */

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {

      // enable Arduino interrupt detection
      Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
      attachInterrupt(digitalPinToInterrupt(mpu_int), dmpInterrupt, RISING);

      // turn on the DMP, now that it's ready
      Serial.println(F("Enabling DMP..."));
      mpu.setDMPEnabled(true);
      mpuIntStatus = mpu.getIntStatus();

      // set our DMP Ready flag so the main loop() function knows it's okay to use it
      Serial.println(F("DMP ready! Waiting for first interrupt..."));
      dmpReady = true;

      // get expected DMP packet size for later comparison
      packetSize = mpu.dmpGetFIFOPacketSize();
      Serial.print("Packet size: ");
      Serial.println(packetSize);
  } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      Serial.print(F("DMP Initialization failed (code "));
      Serial.print(devStatus);
      Serial.println(F(")"));
  }

  //miliseconds counter reset
  sendT0 = 0;

}

void loop() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // check for commands from serial interface
  while (cmdParser.readNextCommand()>0) {
    executeCommand(cmdParser.cmd, cmdParser.numArgs, cmdParser.args);
  }

  timems = millis();
  if (timems - sendT0 >= sendInterval) {
    sendT0 = timems;
    Serial.write(telemetry,telemetrySize);
  }

}
