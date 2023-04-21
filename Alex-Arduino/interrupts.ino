/*
   Setup and start codes for external interrupts and
   pullup resistors.
*/
// Enable pull up resistors on pins 2 and 3
void enablePullups()
{
  // Use bare-metal to enable the pull-up resistors on pins
  // 2 and 3. These are pins PD2 and PD3 respectively.
  // We set bits 2 and 3 in DDRD to 0 to make them inputs.
  DDRD &= ~(0b00001100);
  PORTD |= 0b00001100;
}

// Functions to be called by INT0 and INT1 ISRs.
void leftISR()
{
  if (dir == FORWARD)
  {
    leftForwardTicks++;
    forwardDist = (unsigned long) ((float) leftForwardTicks / COUNTS_PER_REV * WHEEL_CIRC);
  }

  else if (dir == BACKWARD)
  {
    leftReverseTicks++;
    reverseDist = (unsigned long) ((float) leftReverseTicks / COUNTS_PER_REV * WHEEL_CIRC);
  }
  else if (dir == LEFT) // when turning left, the left wheel will turn backwards
  {
    leftReverseTicksTurns++;
  }
  else if (dir == RIGHT) // when turning right, the left wheel will turn forwards
  {
    leftForwardTicksTurns++;
  }
}

void rightISR()
{
  if (dir == FORWARD)
  {
    rightForwardTicks++;
  }

  else if (dir == BACKWARD)
  {
    rightReverseTicks++;
  }
  else if (dir == LEFT) // when turning left, the right wheel will turn forwards
  {
    rightForwardTicksTurns++;
  }
  else if (dir == RIGHT) // when turning right, the right wheel will turn backwards
  {
    rightReverseTicksTurns++;
  }
}

// Set up the external interrupt pins INT0 and INT1
// for falling edge triggered. Use bare-metal.
void setupEINT()
{
  // enable INT0 and INT1
  EIMSK |= 0b00000011;
  // activate INT on falling edge
  EICRA |= 0b00001010;
}

// Implement the external interrupt ISRs below.
// INT0 ISR should call leftISR while INT1 ISR
// should call rightISR.
ISR(INT0_vect)
{
  leftISR();
}
ISR(INT1_vect)
{
  rightISR();
}
