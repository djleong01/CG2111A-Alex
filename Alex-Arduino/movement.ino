// Convert percentages to PWM values
int pwmVal(float speed)
{
  if (speed < 0.0)
    speed = 0;

  if (speed > 100.0)
    speed = 100.0;

  return (int) ((speed / 100.0) * 255.0);
}

// Move Alex forward "dist" cm at speed "speed".
// "speed" is expressed as a percentage. E.g. 50 is
// move forward at half speed.
// Specifying a distance of 0 means Alex will
// continue moving forward indefinitely.
void forward(float dist, float speed)
{
  if (dist > 0)
    deltaDist = dist;
  else
    deltaDist = 99999;
  newDist = forwardDist + deltaDist;
  dir = FORWARD;
  int val = pwmVal(speed);

  // For now we will ignore dist and move
  // forward indefinitely. We will fix this
  // in Week 9.

  // LF = Left forward pin, LR = Left reverse pin
  // RF = Right forward pin, RR = Right reverse pin
  // This will be replaced later with bare-metal code.

  analogWrite(LF, val - 50);
  analogWrite(RF, val);
  analogWrite(LR, 0);
  analogWrite(RR, 0);
}

// Reverse Alex "dist" cm at speed "speed".
// "speed" is expressed as a percentage. E.g. 50 is
// reverse at half speed.
// Specifying a distance of 0 means Alex will
// continue reversing indefinitely.
void reverse(float dist, float speed)
{
  if (dist > 0)
    deltaDist = dist;
  else
    deltaDist = 99999;
  newDist = reverseDist + deltaDist;
  dir = BACKWARD;
  int val = pwmVal(speed);

  // For now we will ignore dist and
  // reverse indefinitely. We will fix this
  // in Week 9.
  // LF = Left forward pin, LR = Left reverse pin
  // RF = Right forward pin, RR = Right reverse pin
  // This will be replaced later with bare-metal code.
  analogWrite(LR, val - 35);
  analogWrite(RR, val);
  analogWrite(LF, 0);
  analogWrite(RF, 0);
}
// function to estimate no. of wheel ticks needed to turn an angle
unsigned long computeDeltaTicks(float ang)
{

  unsigned long ticks = (unsigned long)((ang * alexCirc * COUNTS_PER_REV) / (360.0 * WHEEL_CIRC));
  return ticks;
}
// Turn Alex left "ang" degrees at speed "speed".
// "speed" is expressed as a percentage. E.g. 50 is
// turn left at half speed.
// Specifying an angle of 0 degrees will cause Alex to
// turn left indefinitely.
void left(float ang, float speed)
{
  leftReverseTicksTurns = 0;
  int temp = speed;
  if (temp > 75)
    temp = 75;
  int val = pwmVal(temp);
  if (ang == 0)
    deltaTicks = 9999999;
  else
    deltaTicks = computeDeltaTicks(ang);
  targetTicks = (leftReverseTicksTurns + deltaTicks) - 20;
  dir = LEFT;
  // For now we will ignore ang. We will fix this in Week 9.
  // We will also replace this code with bare-metal later.
  // To turn left we reverse the left wheel and move
  // the right wheel forward.
  analogWrite(LR, val);
  analogWrite(RF, val);
  analogWrite(LF, 0);
  analogWrite(RR, 0);
}

// Turn Alex right "ang" degrees at speed "speed".
// "speed" is expressed as a percentage. E.g. 50 is
// turn left at half speed.
// Specifying an angle of 0 degrees will cause Alex to
// turn right indefinitely.
void right(float ang, float speed)
{
  rightReverseTicksTurns = 0;
  int temp = speed;
  if (temp > 75)
    temp = 75;
  int val = pwmVal(temp);
  if (ang == 0)
    deltaTicks = 9999999;
  else
    deltaTicks = computeDeltaTicks(ang);
  targetTicks = (rightReverseTicksTurns + deltaTicks) / 2;
  dir = RIGHT;
  // For now we will ignore ang. We will fix this in Week 9.
  // We will also replace this code with bare-metal later.
  // To turn right we reverse the right wheel and move
  // the left wheel forward.
  analogWrite(RR, val + 15);
  analogWrite(LF, val);
  analogWrite(LR, 0);
  analogWrite(RF, 0);
}

// Stop Alex. To replace with bare-metal code later.
void stop()
{
  dir = STOP;
  analogWrite(LF, 0);
  analogWrite(LR, 0);
  analogWrite(RF, 0);
  analogWrite(RR, 0);
}

void imu_front(float targetDeg, float startTime)
{
  int val = pwmVal(60); // set default target speed to 60
  while ((millis() - startTime) < 550)
  {
    float deg = getDegree();
    float error = deg - targetDeg;
    float turn = (error * 4); // where 4 is the proportional constant
    analogWrite(LF, val - turn);
    analogWrite(RF, val + turn);
    analogWrite(LR, 0);
    analogWrite(RR, 0);
  }
  // further compensate for the difference in motor friction by running ONLY
  // the right motor for 100ms longer
  analogWrite(RF, val);
  analogWrite(LF, 0);
  delay(100);
}

void imu_back(float targetDeg, float startTime)
{
  int val = pwmVal(60); // set default speed to 70
  while ((millis() - startTime) < 550)
  {
    float deg = getDegree();
    float error = deg - targetDeg;
    float turn = (error * 4); // where 4 is the proportional constant
    analogWrite(LR, val + turn);
    analogWrite(RR, val - turn);
    analogWrite(LF, 0);
    analogWrite(RF, 0);
  }
  // further compensate for the difference in motor friction by running ONLY
  // the right motor for 100ms longer
  analogWrite(LR, 0);
  analogWrite(RR, val);
  delay(100);
}

void imu_left(float targetAng, float speed)
{
  int val = pwmVal(speed);
  if ((initAng - getDegree()) > targetAng)
  {
    stop();
  }
  else
  {
    analogWrite(LR, val);
    analogWrite(RF, val);
    analogWrite(LF, 0);
    analogWrite(RR, 0);
  }
}

void imu_right(float targetAng, float speed)
{
  int val = pwmVal(speed);
  if ((getDegree() - initAng) > targetAng - 5.0)
  {
    stop();
  }
  else
  {
    analogWrite(RR, val);
    analogWrite(LF, val);
    analogWrite(LR, 0);
    analogWrite(RF, 0);
  }
}
