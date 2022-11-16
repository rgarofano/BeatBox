#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Buttons.h"
#include "IntervalTimer.h"
#include "LEDMatrix.h"
#include "Sleep.h"
#include "SoundPlayer.h"

static pthread_t statsTid;
static pthread_t shutdownTid;
static bool shutdown = false;

void* printStatistics(void* _arg) {
  while (!shutdown) {
    int mode = SoundPlayer_getMode();
    int tempo = SoundPlayer_getTempo();
    int volume = SoundPlayer_getVolume();

    Interval_statistics_t pLowStats = {};
    Interval_getStatisticsAndClear(INTERVAL_LOW_LEVEL_AUDIO, &pLowStats);
    Interval_statistics_t pBeatStats = {};
    Interval_getStatisticsAndClear(INTERVAL_BEAT_BOX, &pBeatStats);

    printf(
        "M%d %dbpm vol:%d  Low[%lf, %lf] avg %lf/%d  Beat[%lf, %lf] avg "
        "%lf/%d\n",
        mode, tempo, volume, pLowStats.minIntervalInMs,
        pLowStats.maxIntervalInMs, pLowStats.avgIntervalInMs,
        pLowStats.numSamples, pBeatStats.minIntervalInMs,
        pBeatStats.maxIntervalInMs, pBeatStats.avgIntervalInMs,
        pBeatStats.numSamples);

    Sleep_waitForMs(1000);
  }
  pthread_exit(NULL);
}

void* checkForShutdown(void* _arg) {
  while (!shutdown) {
    printf("Enter 'Q' to quit.\n");
    if (toupper(getchar()) == 'Q') {
      shutdown = true;
    }
  }
  pthread_exit(NULL);
}

void waitForShutdown(void) {
  pthread_create(&shutdownTid, NULL, checkForShutdown, NULL);
  pthread_join(shutdownTid, NULL);
}

void initialize(void) {
  Interval_init();
  SoundPlayer_init();
  Buttons_initButtons();
  LEDMatrix_initMatrix();
}

void startBeatbox(void) {
  SoundPlayer_startPlaying();
  Buttons_startRunning();
  LEDMatrix_startDisplay();
  pthread_create(&statsTid, NULL, printStatistics, NULL);
}

void safeShutdown(void) {
  LEDMatrix_cleanup();
  Buttons_cleanup();
  SoundPlayer_cleanup();
  Interval_cleanup();
}

int main(void) {
  initialize();

  startBeatbox();

  waitForShutdown();

  safeShutdown();

  return 0;
}