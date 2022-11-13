#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <ctype.h>
#include "SoundPlayer.h"
#include "Buttons.h"
#include "LEDMatrix.h"
#include "IntervalTimer.h"
#include "Sleep.h"

static pthread_t statsTid;
static pthread_t shutdownTid;
static bool shutdown = false;

void* printStatistics(void* _arg)
{
    while(true) {
        Sleep_waitForMs(1000);
        
        if(shutdown) {
            break;
        }

        int mode = SoundPlayer_getMode();
        int tempo = SoundPlayer_getTempo();
        int volume = SoundPlayer_getVolume();

        Interval_statistics_t pLowStats = {};
        Interval_getStatisticsAndClear(INTERVAL_LOW_LEVEL_AUDIO, &pLowStats);
        Interval_statistics_t pBeatStats = {};
        Interval_getStatisticsAndClear(INTERVAL_BEAT_BOX, &pBeatStats);

        printf("M%d %dbpm vol:%d  Low[%lf, %lf] avg %lf/%d  Beat[%lf, %lf] avg %lf/%d\n",
                mode,
                tempo,
                volume,
                pLowStats.minIntervalInMs,
                pLowStats.maxIntervalInMs,
                pLowStats.avgIntervalInMs,
                pLowStats.numSamples,
                pBeatStats.minIntervalInMs,
                pBeatStats.maxIntervalInMs,
                pBeatStats.avgIntervalInMs,
                pBeatStats.numSamples
              );
    }
    pthread_exit(NULL);
}

void* checkForShutdown(void* _arg)
{
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
    pthread_create(&shutdownTid, NULL, checkForShutdown, NULL);
}

void initialize(void)
{
    SoundPlayer_init();
    Buttons_initButtons();   
    LEDMatrix_initMatrix();
}

void startBeatbox(void)
{
    SoundPlayer_startPlaying();
    Buttons_startRunning();
    LEDMatrix_startDisplay();
    pthread_create(&statsTid, NULL, printStatistics, NULL);
}

void safeShutdown(void)
{
    pthread_join(shutdownTid, NULL);
    pthread_join(statsTid, NULL);

    Interval_cleanup();
    LEDMatrix_cleanup();
    Buttons_cleanup();
    SoundPlayer_cleanup();
}

int main(void)
{
    Interval_init();
    initialize();

    startBeatbox();
    
    waitForShutdown();

    safeShutdown();

    return 0;
}