/* Used to retireve the voltage reading in the BeagleBone analog inputs */

#ifndef A2D_H
#define A2D_H

int A2D_readVoltage(const int AIN_NUMBER);
float A2D_voltageFromReading(int voltageReading);

#endif