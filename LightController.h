/*
 * LightController controls the display's pulsing and flickering green light
 * The light is a green high-power LED, driven through an H-bridge for convenience
 * 
 * To use this controller, call setupLightController() once and
 * then call loopLightController() repeatedly
 * 
 * Connect (enablePin, control1Pin, control2Pin) to the H-bridge's (ENA, IN1, IN2) or (ENB, IN3, IN4)
 * and connect the H-bridge's (OUT1, OUT2) or (OUT3, OUT4) accordingly to (LED+, LED-)
 */

#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

typedef struct {
  int enablePin;
  int control1Pin;
  int control2Pin;
} lightControllerConfig;

void setupLightController(lightControllerConfig conf);
void loopLightController();

// This isn't meant to be public, but is placed here to work around Arduino IDE build magic
#define NUM_LIGHT_STATES 6
typedef enum {
  STATIC = 0,
  PANNING_UP_QUICKLY,
  PANNING_UP_SLOWLY,
  PANNING_DOWN_QUICKLY,
  PANNING_DOWN_SLOWLY,
  FLICKERING
} lightState;
static int lightStateDurations[] = { 1000, 600, 1200, 600, 1200, 600 };

// Configure how the state machine transitions between states
#define MIN_STATES_BEFORE_FLICKER 5
#define MIN_STATIC_STATES_BEFORE_RISE 1
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 150
#define LOOPS_PER_DELTA_QUICK 2
#define LOOPS_PER_DELTA_SLOW 6
#define MIN_LOOPS_PER_FLICKER 16

#endif
