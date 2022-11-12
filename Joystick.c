#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "A2D.h"

#define JOYSTICK_X_AIN 2
#define JOYSTICK_Y_AIN 3
#define A2D_MAX_READING 4095
#define UPPER_THRESHOLD 0.5
#define LOWER_THRESHOLD -0.5


static double scaledVoltageReading(int voltageReading) {
    double relativeValue = A2D_MAX_READING / 2;
    return ((double) voltageReading / relativeValue) - 1;

}

bool Joystick_up(void) 
{
    int voltageReading = A2D_readVoltage(JOYSTICK_X_AIN);
    double relativeValue = -1 * scaledVoltageReading(voltageReading);
    return relativeValue > UPPER_THRESHOLD;
}

bool Joystick_down(void)
{
    int voltageReading = A2D_readVoltage(JOYSTICK_X_AIN);
    double relativeValue = -1 * scaledVoltageReading(voltageReading);
    return relativeValue < LOWER_THRESHOLD;
}

bool Joystick_left(void)
{
    int voltageReading = A2D_readVoltage(JOYSTICK_Y_AIN);
    double relativeValue = scaledVoltageReading(voltageReading);
    return relativeValue > UPPER_THRESHOLD;
}

bool Joystick_right(void)
{
    int voltageReading = A2D_readVoltage(JOYSTICK_Y_AIN);
    double relativeValue = scaledVoltageReading(voltageReading);
    return relativeValue < LOWER_THRESHOLD;
}