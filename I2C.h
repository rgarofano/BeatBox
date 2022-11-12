/* Used to initialize an I2C device and perform read/write operations 
   with its registers                                                 */

#ifndef I2C_H
#define I2C_H

void I2C_configurePins(const int INDEX);
void I2C_initI2CBus(const int INDEX);
void I2C_writeByteToI2CReg(const int INDEX, unsigned char regAddr, unsigned char value);
void I2C_writeBytesToI2CReg(const int INDEX, unsigned char regAddr, unsigned char value[], const int ARRAY_LENGTH);
unsigned char I2C_readI2CReg(const int INDEX, unsigned char regAddr);

#endif