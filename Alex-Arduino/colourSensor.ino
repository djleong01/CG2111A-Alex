void setupColour()
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

void getColour()
{
  // get RED
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redFrequency = pulseIn(sensorOut, LOW);
  redColor = map(redFrequency, 137, 385, 0, 255);
  // Serial.print("R = ");
  // Serial.print(redColor);
  // get GREEN
  delay(100);
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  greenFrequency = pulseIn(sensorOut, LOW);
  greenColor = map(greenFrequency, 143, 428, 0, 255);
  // Serial.print(" G = ");
  // Serial.print(greenColor);
  delay(100);
  // get BLUE
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blueFrequency = pulseIn(sensorOut, LOW);
  blueColor = map(blueFrequency, 70, 288, 0, 255);
  // Serial.print(" B = ");
  // Serial.print(blueColor);
  delay(100);
  if ((redColor > 25 && redColor < 269) && (greenColor > 172 && greenColor < 279)
  && (blueColor > 190 && blueColor < 287) && (redColor < greenColor) &&
  (redColor < blueColor))
  {
    // RED
    colour = 1;
  }
  else if ((redColor > 253 && redColor < 362) && (greenColor > 131 && greenColor < 265)
  && (blueColor > 220 && blueColor < 286) && (redColor > greenColor) &&
  (greenColor < blueColor) &&((float)(blueColor / (float)greenColor)>1.1))
  {
    // green
    colour = 2;
  }
  else {
    colour = 3;
  }
}
