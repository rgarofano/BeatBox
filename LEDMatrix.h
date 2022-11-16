/* Used to initialize and display two digit numbers on the LED Matrix.
   Runs a thread that updates the display based on joystick movements */

#ifndef LEDMATRIX_H
#define LEDMATRIX_H

void LEDMatrix_initMatrix(void);
void LEDMatrix_startDisplay(void);
void LEDMatrix_updateDisplayValues(int numDips, float maxVoltage, float minVoltage, float minTimeIntreval, float maxTimeIntreval);
void LEDMatrix_cleanup(void);

#endif