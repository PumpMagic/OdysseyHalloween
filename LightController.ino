static lightControllerConfig lcConfig;

/**
 * Set up the light controller
 * We feed a constant polarity to the H-bridge and control the brightness using PWM
 */
void setupLightController(lightControllerConfig conf) {
  lcConfig = conf;
  
  pinMode(lcConfig.enablePin, OUTPUT);
  pinMode(lcConfig.control1Pin, OUTPUT);
  pinMode(lcConfig.control2Pin, OUTPUT);

  analogWrite(lcConfig.enablePin, 0);
  digitalWrite(lcConfig.control1Pin, HIGH);
  digitalWrite(lcConfig.control2Pin, LOW);
}

/**
 * Light controller state machine
 * This function expects to be called every couple milliseconds
 * More or less often than that will affect the speed of the light changes
 */
void loopLightController() {
  static lightState state = STATIC;
  static int brightness = 0;
  static int brightnessDelta = 0;
  static int loopsInCurrentState = 0;
  static int brightnessBeforeFlicker = 0;
  static int statesSinceFlickering = 0;
  static int statesInStatic = 0;

  // Get the next brightness delta
  // The brightness will change different amounts on each loop depending on the state
  switch (state) {
    case STATIC:
      // Do nothing
      brightnessDelta = 0;
      break;
    case PANNING_UP_QUICKLY:
      if (loopsInCurrentState % LOOPS_PER_DELTA_QUICK == 0) {
        brightnessDelta = 1;
      } else {
        brightnessDelta = 0;
      }
      break;
    case PANNING_UP_SLOWLY:
      if (loopsInCurrentState % LOOPS_PER_DELTA_SLOW == 0) {
        brightnessDelta = 1;
      } else {
        brightnessDelta = 0;
      }
      break;
    case PANNING_DOWN_QUICKLY:
      if (loopsInCurrentState % LOOPS_PER_DELTA_QUICK == 0) {
        brightnessDelta = -1;
      } else {
        brightnessDelta = 0;
      }
      break;
    case PANNING_DOWN_SLOWLY:
      if (loopsInCurrentState % LOOPS_PER_DELTA_SLOW == 0) {
        brightnessDelta = -1;
      } else {
        brightnessDelta = 0;
      }
      break;
    case FLICKERING:
      if (loopsInCurrentState < (lightStateDurations[FLICKERING] - 1)) {
        if (loopsInCurrentState % MIN_LOOPS_PER_FLICKER == 0) {
          switch (random(2)) {
            case 0:
              brightnessDelta = -1 * MAX_BRIGHTNESS;
              break;
            case 1:
              brightnessDelta = MAX_BRIGHTNESS;
              break;
          }
        }
      } else {
        // At the end of the flicker state, reset our brightness to whatever it was
        // before we started flickering
        brightness = brightnessBeforeFlicker;
        brightnessDelta = 0;
      }
      break;
  }

  // Change the brightness
  brightness += brightnessDelta;
  if (brightness < MIN_BRIGHTNESS) { brightness = MIN_BRIGHTNESS; }
  if (brightness > MAX_BRIGHTNESS) { brightness = MAX_BRIGHTNESS; }

  // Set the brightness
  analogWrite(lcConfig.enablePin, brightness);

  // Change states, if we've reached the end of the current one
  loopsInCurrentState += 1;
  if (loopsInCurrentState >= lightStateDurations[state]) {
    state = random(NUM_LIGHT_STATES);
    if (brightness <= (MAX_BRIGHTNESS / 2)) {
      // The light is dim
      // If we haven't stayed dim for at least some minimum, stay dim
      // Otherwise, either ramp up or flicker (if it's been a while since we flickered)
      
      if (statesInStatic < MIN_STATIC_STATES_BEFORE_RISE) {
        state = STATIC;
        statesInStatic += 1;
      } else {
        int randomNumber = (statesSinceFlickering >= MIN_STATES_BEFORE_FLICKER) ? random(3) : random(2);
        switch (randomNumber) {
          case 0:
          case 1:
            state = PANNING_UP_SLOWLY;
            break;
          case 2:
            state = FLICKERING;
            break;
        }
      }
    } else {
      // The light is bright
      // Either ramp down or flicker (if it's been a while since we flickered)
      int randomNumber = (statesSinceFlickering >= MIN_STATES_BEFORE_FLICKER) ? random(3) : random(2);
      switch (randomNumber) {
        case 0:
        case 1:
          state = PANNING_DOWN_SLOWLY;
          break;
        case 2:
          state = FLICKERING;
          break;
      }
    }
    
    if (state == FLICKERING) {
      brightnessBeforeFlicker = brightness;
      statesSinceFlickering = 0;
    } else {
      statesSinceFlickering += 1;
    }

    if (state != STATIC) {
      statesInStatic = 0;
    }
    
    loopsInCurrentState = 0;
  }
}

