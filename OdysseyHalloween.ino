/**
 * Odyssey Halloween: A program to control a simple animatronic display in the back of my van for Halloween 2016
 */

// Include all headers here to work around Arduino IDE build magic
// Other files see their own functions only if mentioned in headers that are
// included in the main source file
#include "Arduino.h"
#include "AlienHeadController.h"
#include "LightController.h"
#include "FanController.h"

#define LOOP_TIME_MS 2


// Pin configurations
// Alien head controller needs to use PWM pin 9 or 10 - it changes the timer frequency for those pins' PWM,
// and any other pins would mess with Arduino's delay()
alienHeadControllerConfig headConfig = { .enablePin = 10, .control1Pin = 9, .control2Pin = 8 };
lightControllerConfig lightConfig = { .enablePin = 5, .control1Pin = 7, .control2Pin = 6 };
fanControllerConfig fanConfig = { .enablePin = 3 };

void setup() {
  setupAlienHeadController(headConfig);
  setupLightController(lightConfig);
  setupFanController(fanConfig);
}

void loop() {
  loopAlienHeadController();
  loopLightController();
  loopFanController();
  
  delay(LOOP_TIME_MS);
}
