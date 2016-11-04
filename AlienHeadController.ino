static alienHeadControllerConfig ahcConfig;

/**
 * Divides a given PWM pin frequency by a divisor
 * 
 * Be careful - dividing Arduino PWM frequencies has side effects on its
 * standard library's functions
 * 
 * See http://playground.arduino.cc/Code/PwmFrequency
 */
static void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

/**
 * Set up the alien head controller
 * 
 */
void setupAlienHeadController(alienHeadControllerConfig conf) {
  ahcConfig = conf;
  
  pinMode(ahcConfig.enablePin, OUTPUT);
  pinMode(ahcConfig.control1Pin, OUTPUT);
  pinMode(ahcConfig.control2Pin, OUTPUT);

  // Divide the enable pin PWM frequency so that we can control the motor more finely
  setPwmFrequency(ahcConfig.enablePin, 256);
  
  digitalWrite(ahcConfig.control1Pin, HIGH);
  digitalWrite(ahcConfig.control2Pin, LOW);
}


/**
 * Set the motor direction
 */
static void setMotorDirection(motorDirection dir) {
  switch(dir) {
    case CLOCKWISE:
      digitalWrite(ahcConfig.control1Pin, HIGH);
      digitalWrite(ahcConfig.control2Pin, LOW);
      break;
    case COUNTERCLOCKWISE:
      digitalWrite(ahcConfig.control1Pin, LOW);
      digitalWrite(ahcConfig.control2Pin, HIGH);
      break;
    default:
      break;
  }
}

/**
 * Set the motor speed
 */
static void setMotorSpeed(int spd) {
  analogWrite(ahcConfig.enablePin, spd);
}

/**
 * Halt the motor quickly, as opposed to just setting its speed to zero and
 * waiting for its inertia to die
 */
static void haltMotor() {
  digitalWrite(ahcConfig.control1Pin,LOW);
  digitalWrite(ahcConfig.control2Pin,LOW);
}

/**
 * Alien head controller state machine
 * This function expects to be called every couple milliseconds
 * More or less often than that will affect the range of motion of the head
 */
void loopAlienHeadController() {
  static alienHeadState state = SLOW_TURN_CW;
  static int numIdleLoops = MAX_WAIT_SLOW_TURN_LOOPS;
  static motorDirection lastDirectionTraveled = COUNTERCLOCKWISE;
  static int loopsInCurrentState = 0;
  static boolean needNewState = false;
  static int numCircles = 1;
  static int statesSinceDoubleTake = 0;
  static int statesSinceCircleSpin = 0;
  
  // Change the motor speed and/or direction, if the current state warrants it
  switch (state) {
    case SLOW_TURN_CW:
      if (loopsInCurrentState == 0) {
        setMotorDirection(CLOCKWISE);
        setMotorSpeed(MOTOR_SLOW_SPEED);
      } else if (loopsInCurrentState == NUM_LOOPS_SLOW_TURN) {
        haltMotor();
      } else if (loopsInCurrentState >= (NUM_LOOPS_SLOW_TURN + numIdleLoops)) {
        lastDirectionTraveled = CLOCKWISE;
        needNewState = true;
      }
      break;
    case SLOW_TURN_CCW:
      if (loopsInCurrentState == 0) {
        setMotorDirection(COUNTERCLOCKWISE);
        setMotorSpeed(MOTOR_SLOW_SPEED);
      } else if (loopsInCurrentState == NUM_LOOPS_SLOW_TURN) {
        haltMotor();
      } else if (loopsInCurrentState >= (NUM_LOOPS_SLOW_TURN + numIdleLoops)) {
        lastDirectionTraveled = COUNTERCLOCKWISE;
        needNewState = true;
      }
      break;
    case DOUBLE_TAKE_CW:
      if (loopsInCurrentState == 0) {
        setMotorDirection(CLOCKWISE);
        setMotorSpeed(MOTOR_FAST_SPEED);
      } else if (loopsInCurrentState == NUM_LOOPS_FAST_TURN) {
        haltMotor();
      } else if (loopsInCurrentState == (NUM_LOOPS_FAST_TURN + NUM_LOOPS_FAST_TURN_DELAY)) {
        setMotorDirection(COUNTERCLOCKWISE);
        setMotorSpeed(MOTOR_FAST_SPEED);
      } else if (loopsInCurrentState == (NUM_LOOPS_FAST_TURN + NUM_LOOPS_FAST_TURN_DELAY + NUM_LOOPS_FAST_TURN)) {
        haltMotor();
      } else if (loopsInCurrentState >= (NUM_LOOPS_FAST_TURN + NUM_LOOPS_FAST_TURN_DELAY + NUM_LOOPS_FAST_TURN + numIdleLoops)) {
        needNewState = true;
      }
      break;
    case DOUBLE_TAKE_CCW:
      if (loopsInCurrentState == 0) {
        setMotorDirection(COUNTERCLOCKWISE);
        setMotorSpeed(MOTOR_FAST_SPEED);
      } else if (loopsInCurrentState == NUM_LOOPS_FAST_TURN) {
        haltMotor();
      } else if (loopsInCurrentState == (NUM_LOOPS_FAST_TURN + NUM_LOOPS_FAST_TURN_DELAY)) {
        setMotorDirection(CLOCKWISE);
        setMotorSpeed(MOTOR_FAST_SPEED);
      } else if (loopsInCurrentState == (NUM_LOOPS_FAST_TURN + NUM_LOOPS_FAST_TURN_DELAY + NUM_LOOPS_FAST_TURN)) {
        haltMotor();
      } else if (loopsInCurrentState >= (NUM_LOOPS_FAST_TURN + NUM_LOOPS_FAST_TURN_DELAY + NUM_LOOPS_FAST_TURN + numIdleLoops)) {
        needNewState = true;
      }
      break;
    case SPINNING_IN_CIRCLES:
      if (loopsInCurrentState == 0) {
        switch (random(2)) {
          case 0:
            setMotorDirection(CLOCKWISE);
            break;
          case 1:
            setMotorDirection(COUNTERCLOCKWISE);
            break;
        }
        setMotorSpeed(MOTOR_MAX_SPEED);
      } else if (loopsInCurrentState == (NUM_LOOPS_CIRCLE_SPIN * numCircles)) {
        haltMotor();
      } else if (loopsInCurrentState >= ((NUM_LOOPS_CIRCLE_SPIN * numCircles) + numIdleLoops)) {
        needNewState = true;
      }
      break;
  }
  
  loopsInCurrentState += 1;

  // Change states, if we've reached the end of the current one
  if (needNewState) {
    // valid new states are:
    // SLOW_TURN_x (preferred)
    // DOUBLE_TAKE_x (if it's been a while since a double take)
    // SPINNING_IN_CIRCLES (if it's been a while since spinning in circles)
    if (statesSinceDoubleTake < MIN_STATES_BEFORE_DOUBLE_TAKE && statesSinceCircleSpin < MIN_STATES_BEFORE_CIRCLE_SPIN) {
      state = (lastDirectionTraveled == CLOCKWISE) ? SLOW_TURN_CCW : SLOW_TURN_CW;
    } else if (statesSinceDoubleTake >= MIN_STATES_BEFORE_DOUBLE_TAKE && statesSinceCircleSpin < MIN_STATES_BEFORE_CIRCLE_SPIN) {
      switch (random(4)) {
        case 0:
        case 1:
        case 2:
          state = (lastDirectionTraveled == CLOCKWISE) ? SLOW_TURN_CCW : SLOW_TURN_CW;
          break;
        case 3:
          state = (lastDirectionTraveled == CLOCKWISE) ? DOUBLE_TAKE_CCW : DOUBLE_TAKE_CW;
          break;
      }
    } else if (statesSinceDoubleTake < MIN_STATES_BEFORE_DOUBLE_TAKE && statesSinceCircleSpin >= MIN_STATES_BEFORE_CIRCLE_SPIN) {
      switch (random(4)) {
        case 0:
        case 1:
        case 2:
          state = (lastDirectionTraveled == CLOCKWISE) ? SLOW_TURN_CCW : SLOW_TURN_CW;
          break;
        case 3:
          state = SPINNING_IN_CIRCLES;
          break;
      }
    } else if (statesSinceDoubleTake >= MIN_STATES_BEFORE_DOUBLE_TAKE && statesSinceCircleSpin >= MIN_STATES_BEFORE_CIRCLE_SPIN) {
      switch (random(5)) {
        case 0:
        case 1:
        case 2:
          state = (lastDirectionTraveled == CLOCKWISE) ? SLOW_TURN_CCW : SLOW_TURN_CW;
          break;
        case 3:
          state = (lastDirectionTraveled == CLOCKWISE) ? DOUBLE_TAKE_CCW : DOUBLE_TAKE_CW;
          break;
        case 4:
          state = SPINNING_IN_CIRCLES;
          break;
      }
    }

    statesSinceDoubleTake += 1;
    statesSinceCircleSpin += 1;

    switch (state) {
      case SLOW_TURN_CW:
      case SLOW_TURN_CCW:
        numIdleLoops = MIN_WAIT_SLOW_TURN_LOOPS + random(MAX_WAIT_SLOW_TURN_LOOPS-MIN_WAIT_SLOW_TURN_LOOPS);
        break;
      case DOUBLE_TAKE_CW:
      case DOUBLE_TAKE_CCW:
        numIdleLoops = MIN_WAIT_DOUBLE_TAKE_LOOPS + random(MAX_WAIT_DOUBLE_TAKE_LOOPS-MIN_WAIT_DOUBLE_TAKE_LOOPS);
        statesSinceDoubleTake = 0;
        break;
      case SPINNING_IN_CIRCLES:
        numIdleLoops = MIN_WAIT_CIRCLE_SPIN_LOOPS + random(MAX_WAIT_CIRCLE_SPIN_LOOPS-MIN_WAIT_CIRCLE_SPIN_LOOPS);
        statesSinceCircleSpin = 0;
        break;
    }

    needNewState = false;
    loopsInCurrentState = 0;
    numCircles = random(MAX_NUM_SEQUENTIAL_SPINS) + 1;
  }
}

