#include <stdio.h>
#include <string.h>

#include "File.h"
#include "Terminal.h"

#define MAX_SIZE 1024
#define NUM_GPIO_DEVICES 4

typedef struct {
  char* bbgPinNumber;
  int linuxGPIONumber;
  char* directionPath;
  char* valuePath;
} gpioDevice_t;

static gpioDevice_t gpioDevices[NUM_GPIO_DEVICES] = {
    {"P8.15", 47, "/sys/class/gpio/gpio47/direction",
     "/sys/class/gpio/gpio47/value"},
    {"P8.16", 46, "/sys/class/gpio/gpio46/direction",
     "/sys/class/gpio/gpio46/value"},
    {"P8.17", 27, "/sys/class/gpio/gpio27/direction",
     "/sys/class/gpio/gpio27/value"},
    {"P8.18", 65, "/sys/class/gpio/gpio65/direction",
     "/sys/class/gpio/gpio65/value"}};

void GPIO_configurePins(void) {
  for (int i = 0; i < NUM_GPIO_DEVICES; i++) {
    const char* PIN = gpioDevices[i].bbgPinNumber;
    char command[MAX_SIZE];
    snprintf(command, MAX_SIZE, "config-pin %s gpio", PIN);
    Terminal_runCommand(command);
  }
}

void GPIO_configurePin(const int INDEX) {
  const char* PIN = gpioDevices[INDEX].bbgPinNumber;
  char command[MAX_SIZE];
  snprintf(command, MAX_SIZE, "config-pin %s gpio", PIN);
  Terminal_runCommand(command);
}

void GPIO_setDirectionAllDevices(char* mode) {
  for (int i = 0; i < NUM_GPIO_DEVICES; i++) {
    FILE* pFile =
        File_getFilePointer(gpioDevices[i].directionPath, "w", "direction");
    fprintf(pFile, "%s", mode);
    fclose(pFile);
  }
}

void GPIO_setDirection(const int INDEX, char* mode) {
  FILE* pFile =
      File_getFilePointer(gpioDevices[INDEX].directionPath, "w", "direction");
  fprintf(pFile, "%s", mode);
  fclose(pFile);
}

char* GPIO_readValue(const int INDEX) {
  char* valuePath = gpioDevices[INDEX].valuePath;
  FILE* pFile = File_getFilePointer(valuePath, "r", "value");
  char* value = File_extractFileContents(pFile);
  fclose(pFile);
  return value;
}