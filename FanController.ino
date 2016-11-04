static fanControllerConfig fcConfig;

void setupFanController(fanControllerConfig conf) {
  fcConfig = conf;
  
  pinMode(fcConfig.enablePin, OUTPUT);

  analogWrite(fcConfig.enablePin, FAN_PULSE_WIDTH);
}

void loopFanController() {}

