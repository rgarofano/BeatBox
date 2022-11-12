#include <stdio.h>
#include <stdlib.h>
#include "Buttons.h"
#include "LEDMatrix.h"
#include "Sleep.h"

#define MODE_BUTTON_INDEX 0
#define BASE_BUTTON_INDEX 1
#define SNARE_BUTTON_INDEX 2
#define HIHAT_BUTTON_INDEX 3
#define MODE "in"

void initializeHardware(void)
{
    Buttons_initButtons();   
    LEDMatrix_initMatrix();
}

int main(void)
{
    initializeHardware();
    
    while(0) {
        printf("MODE: %d  BASE: %d  SNARE: %d  HIHAT: %d\n",
                Buttons_isButtonPressed(MODE_BUTTON_INDEX),
                Buttons_isButtonPressed(BASE_BUTTON_INDEX),
                Buttons_isButtonPressed(SNARE_BUTTON_INDEX),
                Buttons_isButtonPressed(HIHAT_BUTTON_INDEX));
        
        Sleep_waitForMs(100);
    }

    return 0;
}