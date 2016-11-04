/*
 * FanController controls the Light's cooling fan
 * The light is mounted to a stock Intel CPU heatsink with a four-pin controller
 * 
 * Connect enablePin to the fan's control pin (typically the blue wire)
 */
 
#ifndef FANCONTROLLER_H
#define FANCONTROLLER_H

#define FAN_PULSE_WIDTH 75

typedef struct {
  int enablePin;
} fanControllerConfig;

void setupFanController(fanControllerConfig conf);
void loopFanController();

#endif
