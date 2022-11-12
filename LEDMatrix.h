/* Used to initialize and display two digit numbers on the LED Matrix */

#ifndef LEDMATRIX_H
#define LEDMATRIX_H

void LEDMatrix_initMatrix(void);
void LEDMatrix_startDisplay(void);
void LEDMatrix_updateDisplayValues(int numDips, float maxVoltage, float minVoltage, float minTimeIntreval, float maxTimeIntreval);
void LEDMatrix_stopRunning(void);

#endif