/*
 * AlienHeadController controls the display's alien head
 * The alien head is mounted to a brushless DC motor, driven through an H-bridge
 * 
 * To use this controller, call setupAlienHeadController() once and
 * then call loopAlienHeadController() repeatedly
 * 
 * Connect (enablePin, control1Pin, control2Pin) to the H-bridge's
 * (ENA, IN1, IN2) or (ENB, IN3, IN4) and connect the H-bridge's
 * (OUT1, OUT2) or (OUT3, OUT4) accordingly to (MOT1, MOT2)
 */

#ifndef ALIENHEADCONTROLLER_H
#define ALIENHEADCONTROLLER_H

typedef struct {
  int enablePin;
  int control1Pin;
  int control2Pin;
} alienHeadControllerConfig;

void setupAlienHeadController(alienHeadControllerConfig conf);
void loopAlienHeadController();


// This isn't meant to be public, but is placed here to work around Arduino IDE build magic
// These may or may not be clockwise and counterclockwise, the point is that they're separate directions
typedef enum {
  CLOCKWISE,
  COUNTERCLOCKWISE
} motorDirection;

// This isn't meant to be public, but is placed here to work around Arduino IDE build magic
typedef enum {
  SLOW_TURN_CW,         // a slow clockwise turn 135 degrees, followed by a pause of random duration
  SLOW_TURN_CCW,        // a slow counterclockwise turn 135 degrees, followed by a pause of random duration
  DOUBLE_TAKE_CW,       // a quick clockwise turn 45 degrees, then a quick counterclockwise turn 45 degrees
  DOUBLE_TAKE_CCW,      // a quick counterclockwise turn 45 degrees, then a quick clockwise turn 45 degrees
  SPINNING_IN_CIRCLES,  // a random number of circle spins
} alienHeadState;

/* Motor speeds and state durations
 * Use hardcoded values here rather than calculating them because they're all highly sensitive to
 * the motor's orientation and load */
#define MOTOR_MIN_SPEED 0
#define MOTOR_SLOW_SPEED 50
#define MOTOR_FAST_SPEED 200
#define MOTOR_MAX_SPEED 255
#define NUM_LOOPS_SLOW_TURN 260
#define MIN_WAIT_SLOW_TURN_LOOPS 400
#define MAX_WAIT_SLOW_TURN_LOOPS 1000

#define NUM_LOOPS_FAST_TURN 60
#define NUM_LOOPS_FAST_TURN_DELAY 30
#define MIN_WAIT_DOUBLE_TAKE_LOOPS 1300
#define MAX_WAIT_DOUBLE_TAKE_LOOPS 2000

#define NUM_LOOPS_CIRCLE_SPIN 240
#define MAX_NUM_SEQUENTIAL_SPINS 3
#define MIN_WAIT_CIRCLE_SPIN_LOOPS 2000
#define MAX_WAIT_CIRCLE_SPIN_LOOPS 3000

#define MIN_STATES_BEFORE_DOUBLE_TAKE 5
#define MIN_STATES_BEFORE_CIRCLE_SPIN 10

#endif
