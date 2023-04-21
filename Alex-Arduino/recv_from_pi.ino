/*
   Alex's setup and run codes
*/

// Clears all our counters
void clearCounters()
{
  leftForwardTicks = 0;
  rightForwardTicks = 0;
  leftReverseTicks = 0;
  rightReverseTicks = 0;
  leftForwardTicksTurns = 0;
  rightForwardTicksTurns = 0;
  leftReverseTicksTurns = 0;
  rightReverseTicksTurns = 0;
  leftRevs = 0;
  rightRevs = 0;
  forwardDist = 0;
  reverseDist = 0;
}

// Clears one particular counter
void clearOneCounter(int which)
{
  clearCounters();
}
// Intialize Vincet's internal states

void initializeState()
{
  clearCounters();
}

void handleCommand(TPacket *command)
{
  switch (command->command)
  {
    // For movement commands, param[0] = distance, param[1] = speed.
    case COMMAND_FORWARD:
      sendOK();
      forward((float) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_REVERSE:
      sendOK();
      reverse((float) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_TURN_LEFT:
      sendOK();
      left((float) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_TURN_RIGHT:
      sendOK();
      right((float) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_STOP:
      sendOK();
      stop();
      break;
    case COMMAND_GET_STATS:
      sendStatus();
      break;
    case COMMAND_CLEAR_STATS:
      clearOneCounter(command->params[0]);
      sendOK();
      break;
    case COMMAND_IDENTIFY_OBJECT:
      getColour();
      sendColour();
      break;
    case COMMAND_IMU_FRONT:
      sendOK();
      dir = IMU_FRONT;
      break;
    case COMMAND_IMU_BACK:
      sendOK();
      dir = IMU_BACK;
      break;
    case COMMAND_IMU_LEFT:
      sendOK();
      initAng = getDegree();
      dir = IMU_LEFT;
      targetAng = (float)command->params[0];
      targetSpeed = (float)command->params[1];
      if (targetSpeed < 55.0)
        targetSpeed = 55.0;
      break;
    case COMMAND_IMU_RIGHT:
      sendOK();
      initAng = getDegree();
      dir = IMU_RIGHT;
      //      dir = IMU_RIGHT;
      //imu_right((float) command->params[0], (float) command->params[1]);
      //      sendOK();
      //      stop();
      targetAng = (float)command->params[0];
      targetSpeed = (float)command->params[1];
      if (targetSpeed < 55.0)
        targetSpeed = 55.0;
      break;
    case COMMAND_GET_IMU:
      sendIMU();
      break;
    default:
      sendBadCommand();
  }
}

void waitForHello()
{
  int exit = 0;

  while (!exit)
  {
    TPacket hello;
    TResult result;

    do
    {
      result = readPacket(&hello);
    } while (result == PACKET_INCOMPLETE);

    if (result == PACKET_OK)
    {
      if (hello.packetType == PACKET_TYPE_HELLO)
      {
        sendOK();
        exit = 1;
      }
      else
        sendBadResponse();
    }
    else if (result == PACKET_BAD)
    {
      sendBadPacket();
    }
    else if (result == PACKET_CHECKSUM_BAD)
      sendBadChecksum();
  } // !exit
}
