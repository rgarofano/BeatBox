#include <stdlib.h>
#include <stdbool.h>
#include "GPIO.h"

#define MODE "in"
#define BUTTON_PRESSED 1

void Buttons_initButtons(void)
{
    GPIO_configurePins();
    GPIO_setDirectionAllDevices(MODE);
}

bool Buttons_isButtonPressed(int INDEX)
{
    char* readData = GPIO_readValue(INDEX);
    int value = atoi(readData);
    return value == BUTTON_PRESSED;
}