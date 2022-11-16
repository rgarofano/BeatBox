#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "GPIO.h"
#include "Sleep.h"
#include "SoundPlayer.h"

#define MODE "in"
#define BUTTON_PRESSED 1
#define MODE_BUTTON_INDEX 0
#define BASE_BUTTON_INDEX 1
#define SNARE_BUTTON_INDEX 2
#define HIHAT_BUTTON_INDEX 3

static pthread_t buttonsTid;
static bool shutdown = false;

void Buttons_initButtons(void) {
  GPIO_configurePins();
  GPIO_setDirectionAllDevices(MODE);
}

static bool isButtonPressed(int INDEX) {
  char* readData = GPIO_readValue(INDEX);
  int value = atoi(readData);
  return value == BUTTON_PRESSED;
}

void* updateSoundPlayerOnButtonPress(void* _arg) {
  int modes[NUM_MODES] = {NONE, ROCK, CUSTOM};
  int modeIndex = DEFAULT_MODE;

  while (!shutdown) {
    if (isButtonPressed(MODE_BUTTON_INDEX)) {
      modeIndex = (modeIndex + 1) % NUM_MODES;
      int newMode = modes[modeIndex];
      SoundPlayer_setMode(newMode);
      // holding button should do nothing
      while (isButtonPressed(MODE_BUTTON_INDEX))
        ;

    } else if (isButtonPressed(BASE_BUTTON_INDEX)) {
      SoundPlayer_playBaseSoundNow();
      // holding button should do nothing
      while (isButtonPressed(BASE_BUTTON_INDEX))
        ;

    } else if (isButtonPressed(SNARE_BUTTON_INDEX)) {
      SoundPlayer_playSnareSoundNow();
      // holding button should do nothing
      while (isButtonPressed(SNARE_BUTTON_INDEX));

    } else if (isButtonPressed(HIHAT_BUTTON_INDEX)) {
      SoundPlayer_playHihatSoundNow();
      // holding button should do nothing
      while (isButtonPressed(HIHAT_BUTTON_INDEX));
    }
    Sleep_waitForMs(100);
  }
  pthread_exit(NULL);
}

void Buttons_startRunning(void) {
  pthread_create(&buttonsTid, NULL, updateSoundPlayerOnButtonPress, NULL);
}

void Buttons_cleanup(void) {
  shutdown = true;
  pthread_join(buttonsTid, NULL);
}