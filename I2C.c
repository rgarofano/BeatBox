#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "Terminal.h"

#define MAX_SIZE 1024

typedef struct {
    char* bus;
    int address;
    char* sclPin;
    char* sdaPin;
    int fileDesc;
} i2cDevice_t;

// define the required data for each I2C device
// file desriptor will be updated when initializing the bus
static i2cDevice_t i2cDevices[] = {
    {"/dev/i2c-1", 0x70, "P9_17", "P9_18", 0}
};

void I2C_configurePins(const int INDEX)
{
    const char* SCL_PIN = i2cDevices[INDEX].sclPin;
    char command[MAX_SIZE];
    snprintf(command, MAX_SIZE, "config-pin %s i2c", SCL_PIN);
    Terminal_runCommand(command);
    
    memset(command, 0, MAX_SIZE);

    const char* SDA_PIN = i2cDevices[INDEX].sdaPin;
    snprintf(command, MAX_SIZE, "config-pin %s i2c", SDA_PIN);
    Terminal_runCommand(command);
}

void I2C_initI2CBus(const int INDEX)
{
    int i2cFileDesc = open(i2cDevices[INDEX].bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, i2cDevices[INDEX].address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    i2cDevices[INDEX].fileDesc = i2cFileDesc;
}

void I2C_writeByteToI2CReg(const int INDEX, unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int bytesWritten = write(i2cDevices[INDEX].fileDesc, buff, 2);
    if (bytesWritten != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

void I2C_writeBytesToI2CReg(const int INDEX, unsigned char regAddr, unsigned char values[], const int ARRAY_LENGTH)
{
    const int BUFF_LENGTH = ARRAY_LENGTH + 1;
    unsigned char* buff = malloc((BUFF_LENGTH) * sizeof(*buff));
    buff[0] = regAddr;
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        buff[i + 1] = values[i];
    }
    int bytesWritten = write(i2cDevices[INDEX].fileDesc, buff, BUFF_LENGTH);
    if (bytesWritten != BUFF_LENGTH) {
        perror("I2C: Unable to write i2c register");
        exit(1);
    }
}

unsigned char I2C_readI2CReg(const int INDEX, unsigned char regAddr)
{
    // To read a register, must first write the address
    int bytesWritten = write(i2cDevices[INDEX].fileDesc, &regAddr, sizeof(regAddr));
    if (bytesWritten != sizeof(regAddr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }
    // Now read the value and return it
    char value = 0;
    int bytesRead = read(i2cDevices[INDEX].fileDesc, &value, sizeof(value));
    if (bytesRead != sizeof(value)) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
    return value;
}