#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <ctype.h>
#include "SoundPlayer.h"
#include "Buttons.h"
#include "LEDMatrix.h"

void* checkForShutdown(void* _arg)
{
    bool shutdown = false;

    while(!shutdown) {
        printf("Enter 'Q' to quit.\n");
        if (toupper(getchar()) == 'Q') {
            shutdown = true;
        }
    }
    pthread_exit(NULL);
}

void waitForShutdown(void)
{
    pthread_t shutdownTid;
    pthread_create(&shutdownTid, NULL, checkForShutdown, NULL);
    pthread_join(shutdownTid, NULL);
}

void initializeHardware(void)
{
    SoundPlayer_init();
    Buttons_initButtons();   
    LEDMatrix_initMatrix();
}

void safeShutdown(void)
{
    LEDMatrix_cleanup();
    Buttons_cleanup();
    SoundPlayer_cleanup();
}

int main(void)
{
    initializeHardware();

    SoundPlayer_startPlaying();
    Buttons_startRunning();
    LEDMatrix_startDisplay();
    
    waitForShutdown();

    safeShutdown();

    return 0;
}