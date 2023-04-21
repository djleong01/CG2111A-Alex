#include <serialize.h>

#include "packet.h"
#include "constants.h"
#include <stdarg.h>
#include <math.h>
#include <Adafruit_FXAS21002C.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_FXAS21002C gyro = Adafruit_FXAS21002C(0x0021002C);

float gyroZ;
float currentTime, elapsedTime, previousTime;
float gyroErrorZ;
float degree, rad;

typedef enum
{
  STOP = 0,
  FORWARD = 1,
  BACKWARD = 2,
  LEFT = 3,
  RIGHT = 4,
  IMU_FRONT = 5,
  IMU_BACK = 6,
  IMU_LEFT = 7,
  IMU_RIGHT = 8
}  TDirection;

volatile TDirection dir = STOP;


/*
   Alex's configuration constants
*/

// Number of ticks per revolution from the
// wheel encoder.

#define COUNTS_PER_REV      175

// Wheel circumference in cm.
// We will use this to calculate forward/backward distance traveled
// by taking revs * WHEEL_CIRC

#define WHEEL_CIRC          20.4

// Motor control pins. You need to adjust these till
// Alex moves in the correct direction
#define LF                  10   // Left forward pin
#define LR                  11  // Left reverse pin
#define RF                  6  // Right forward pin
#define RR                  5  // Right reverse pin


// define colour sensor pins
#define S0 7
#define S1 8
#define S2 9
#define S3 12
#define sensorOut 13

int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

int redColor = 0;
int greenColor = 0;
int blueColor = 0;

int colour = 0;

# define PI 3.141592654

// to be adjusted again
#define ALEX_LENGTH   16
#define ALEX_BREADTH   6

float alexDiagonal = 0.0;
float alexCirc = 0.0; //alex's turning circumference
/*
      Alex's State Variables
*/

// Store the ticks from Alex's left and
// right encoders.
volatile unsigned long leftForwardTicks;
volatile unsigned long rightForwardTicks;
volatile unsigned long leftReverseTicks;
volatile unsigned long rightReverseTicks;

volatile unsigned long leftForwardTicksTurns;
volatile unsigned long rightForwardTicksTurns;
volatile unsigned long leftReverseTicksTurns;
volatile unsigned long rightReverseTicksTurns;

// Store the revolutions on Alex's left
// and right wheels
volatile unsigned long leftRevs;
volatile unsigned long rightRevs;
// Forward and backward distance traveled
volatile unsigned long forwardDist;
volatile unsigned long reverseDist;

// Variables to keep track of whether we have moved a commanded dist
unsigned long deltaDist;
unsigned long newDist;
unsigned long deltaTicks;
unsigned long targetTicks;

float initAng = 0;
float targetAng = 0;
float targetSpeed = 0;
float lastError = 0;
/*
   Alex Communication Routines.
*/

TResult readPacket(TPacket *packet)
{
  // Reads in data from the serial port and
  // deserializes it.Returns deserialized
  // data in "packet".

  char buffer[PACKET_SIZE];
  int len;

  len = readSerial(buffer);

  if (len == 0)
    return PACKET_INCOMPLETE;
  else
    return deserialize(buffer, len, packet);

}


void setup() {
  // put your setup code here, to run once:
  alexDiagonal = sqrt((ALEX_LENGTH * ALEX_LENGTH) + (ALEX_BREADTH * ALEX_BREADTH));
  alexCirc = PI * alexDiagonal;
  cli();
  setupEINT();
  setupSerial();
  startSerial();
  setupMotors();
  startMotors();
  enablePullups();
  initializeState();
  setupColour();
  sei();
  if (!gyro.begin()) {
    // There was a problem detecting the FXAS21002C ... check your connections
    //Serial.println("Ooops, no FXAS21002C detected ... Check your wiring!");
    while (1);
  }
  /* Set gyro range. (optional, default is 250 dps) */
  // gyro.setRange(GYRO_RANGE_2000DPS);

  /* Display some basic information on this sensor */
  //displaySensorDetails();
  currentTime = micros();
}

void handlePacket(TPacket *packet)
{
  switch (packet->packetType)
  {
    case PACKET_TYPE_COMMAND:
      handleCommand(packet);
      break;

    case PACKET_TYPE_RESPONSE:
      break;

    case PACKET_TYPE_ERROR:
      break;

    case PACKET_TYPE_MESSAGE:
      break;

    case PACKET_TYPE_HELLO:
      break;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  TPacket recvPacket; // This holds commands from the P
  TResult result = readPacket(&recvPacket);

  if (result == PACKET_OK)
    handlePacket(&recvPacket);
  else if (result == PACKET_BAD)
  {
    sendBadPacket();
  }
  else if (result == PACKET_CHECKSUM_BAD)
  {
    sendBadChecksum();
  }
  if (dir == IMU_FRONT)
  {
    imu_front(getDegree(), millis());
    stop();
  }
  else if (dir == IMU_BACK)
  {
    imu_back(getDegree(), millis());
    stop();
  }
  else if (dir == IMU_LEFT)
  {
    imu_left(targetAng, targetSpeed);
  }
  else if (dir == IMU_RIGHT)
  {
    imu_right(targetAng, targetSpeed);
  }
  if (deltaDist > 0)
  {
    if (dir == FORWARD)
    {
      if (forwardDist > newDist)
      {
        deltaDist = 0;
        newDist = 0;
        stop();
      }
    }
    else if (dir == BACKWARD)
    {
      if (reverseDist > newDist)
      {
        deltaDist = 0;
        newDist = 0;
        stop();
      }
    }
    else if (dir == STOP)
    {
      deltaDist = 0;
      newDist = 0;
      stop();
    }
  }
  if (deltaTicks > 0)
  {
    if (dir == LEFT)
    {
      if (leftReverseTicksTurns >= targetTicks)
      {
        deltaTicks = 0;
        targetTicks = 0;
        stop();
      }
    }
    else if (dir == RIGHT)
    {
      if (rightReverseTicksTurns >= targetTicks)
      {
        deltaTicks = 0;
        targetTicks = 0;
        stop();
      }
    }
    else if (dir == STOP)
    {
      deltaTicks = 0;
      targetTicks = 0;
      stop();
    }
  }
}

