void calcError() {
  int count = 0;
  while (count < 200) {
    // To read from the gyroscope,  first call the
    // readGyro() function. When it exits, it'll update the
    // gx, gy, and gz variables with the most current data
    sensors_event_t event;
    gyro.getEvent(&event);
    gyroZ = event.gyro.z;
    gyroErrorZ += gyroZ;
    count ++;
  }
  gyroErrorZ /= 200;
}

void displaySensorDetails(void) {
  sensor_t sensor;
  gyro.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(sensor.name);
  Serial.print("Driver Ver:   ");
  Serial.println(sensor.version);
  Serial.print("Unique ID:    0x");
  Serial.println(sensor.sensor_id, HEX);
  Serial.print("Max Value:    ");
  Serial.print(sensor.max_value);
  Serial.println(" rad/s");
  Serial.print("Min Value:    ");
  Serial.print(sensor.min_value);
  Serial.println(" rad/s");
  Serial.print("Resolution:   ");
  Serial.print(sensor.resolution);
  Serial.println(" rad/s");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

float getDegree()
{
  // create sensor event to obtain a new updated set of data
  sensors_event_t event;
  gyro.getEvent(&event);
  // get the gyroscope reading in the Z-axis (angular velocity in rad/s)
  gyroZ = event.gyro.z;
  // account for very minor fluctations/offset in compass data
  if (gyroZ < 0.02 && gyroZ > -0.02)
  {
    gyroZ = 0.00;
  }
  previousTime = currentTime;
  currentTime = micros();
  elapsedTime = (currentTime - previousTime) / 1000000;
  // multiply by the elapsed time to obtain Alex's heading in radians
  rad += gyroZ * elapsedTime;
  degree = rad * (180 / PI); // convert heading in rad to deg
  return degree; // return Alex's heading
}
