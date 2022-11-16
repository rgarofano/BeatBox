#include "SoundPlayer.h"

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "AudioMixer.h"
#include "IntervalTimer.h"
#include "Sleep.h"

#define BASE_WAV_PATH "beatbox-wav-files/100051__menegass__gui-drum-bd-hard.wav"
#define SNARE_WAV_PATH "beatbox-wav-files/100059__menegass__gui-drum-snare-soft.wav"
#define HIHAT_WAV_PATH "beatbox-wav-files/100053__menegass__gui-drum-cc.wav"
#define DEFAULT_BPM 120
#define DEFAULT_VOL 80
#define MIN_BPM 40
#define MAX_BPM 300
#define MIN_VOL 0
#define MAX_VOL 100
#define PLAY_BASE 1
#define NO_BASE 0
#define PLAY_SNARE 1
#define NO_SNARE 0
#define PLAY_HIHAT 1
#define NO_HIHAT 0

static pthread_mutex_t soundPlayerMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t playBeatTid;
static wavedata_t baseSound = {};
static wavedata_t snareSound = {};
static wavedata_t hihatSound = {};
static bool shutdown = false;
static int mode = DEFAULT_MODE;
static int tempoBPM = DEFAULT_BPM;
static int volume = DEFAULT_VOL;

void SoundPlayer_init(void) {
  AudioMixer_init();
  AudioMixer_readWaveFileIntoMemory(BASE_WAV_PATH, &baseSound);
  AudioMixer_readWaveFileIntoMemory(SNARE_WAV_PATH, &snareSound);
  AudioMixer_readWaveFileIntoMemory(HIHAT_WAV_PATH, &hihatSound);
}

static long long calculateHalfBeatTime(void) {
  return (60.0 / (float)tempoBPM) * 0.5 * 1000;
}

void SoundPlayer_playBaseSoundNow(void) {
  pthread_mutex_lock(&soundPlayerMutex);
  {
    AudioMixer_setVolume(volume);
    AudioMixer_queueSound(&baseSound);
  }
  pthread_mutex_unlock(&soundPlayerMutex);
}

void SoundPlayer_playSnareSoundNow(void) {
  pthread_mutex_lock(&soundPlayerMutex);
  {
    AudioMixer_setVolume(volume);
    AudioMixer_queueSound(&snareSound);
  }
  pthread_mutex_unlock(&soundPlayerMutex);
}

void SoundPlayer_playHihatSoundNow(void) {
  pthread_mutex_lock(&soundPlayerMutex);
  {
    AudioMixer_setVolume(volume);
    AudioMixer_queueSound(&hihatSound);
  }
  pthread_mutex_unlock(&soundPlayerMutex);
}

static bool modeChanged(int expectedMode) {
  return SoundPlayer_getMode() != expectedMode;
}

static void queueNewEighthNote(bool playBaseSound, bool playSnareSound,
                               bool playHihatSound, int initialMode) {
  if (modeChanged(initialMode)) {
    return;
  }

  if (playBaseSound) {
    SoundPlayer_playBaseSoundNow();
  }

  if (playSnareSound) {
    SoundPlayer_playSnareSoundNow();
  }

  if (playHihatSound) {
    SoundPlayer_playHihatSoundNow();
  }

  int halfBeatTimeMs = calculateHalfBeatTime();
  Sleep_waitForMs(halfBeatTimeMs);
  Interval_markInterval(INTERVAL_BEAT_BOX);
}

static void playRockBeat(void) {
  queueNewEighthNote(PLAY_BASE, NO_SNARE, PLAY_HIHAT, ROCK);

  queueNewEighthNote(NO_BASE, NO_SNARE, PLAY_HIHAT, ROCK);

  queueNewEighthNote(NO_BASE, PLAY_SNARE, PLAY_HIHAT, ROCK);

  queueNewEighthNote(NO_BASE, NO_SNARE, PLAY_HIHAT, ROCK);

  queueNewEighthNote(PLAY_BASE, NO_SNARE, PLAY_HIHAT, ROCK);

  queueNewEighthNote(NO_BASE, NO_SNARE, PLAY_HIHAT, ROCK);

  queueNewEighthNote(NO_BASE, PLAY_SNARE, PLAY_HIHAT, ROCK);

  queueNewEighthNote(NO_BASE, NO_SNARE, PLAY_HIHAT, ROCK);
}

static void playCustomBeat(void) {
  queueNewEighthNote(PLAY_BASE, NO_SNARE, NO_HIHAT, CUSTOM);

  queueNewEighthNote(PLAY_BASE, PLAY_SNARE, NO_HIHAT, CUSTOM);

  queueNewEighthNote(NO_BASE, PLAY_SNARE, NO_HIHAT, CUSTOM);

  queueNewEighthNote(NO_BASE, PLAY_SNARE, PLAY_HIHAT, CUSTOM);

  queueNewEighthNote(NO_BASE, NO_SNARE, PLAY_HIHAT, CUSTOM);

  queueNewEighthNote(PLAY_BASE, PLAY_SNARE, PLAY_HIHAT, CUSTOM);
}

void* playBeatForCurrentMode(void* _arg) {
  while (!shutdown) {
    int currentMode = SoundPlayer_getMode();
    if (currentMode == ROCK) {
      playRockBeat();
    } else if (currentMode == CUSTOM) {
      playCustomBeat();
    }
  }

  pthread_exit(NULL);
}

void SoundPlayer_startPlaying(void) {
  pthread_create(&playBeatTid, NULL, playBeatForCurrentMode, NULL);
}

void SoundPlayer_setMode(const int MODE) {
  assert(MODE == NONE || MODE == ROCK || MODE == CUSTOM);
  pthread_mutex_lock(&soundPlayerMutex);
  { mode = MODE; }
  pthread_mutex_unlock(&soundPlayerMutex);
}

int SoundPlayer_getMode(void) {
  int currentMode;

  pthread_mutex_lock(&soundPlayerMutex);
  { currentMode = mode; }
  pthread_mutex_unlock(&soundPlayerMutex);

  return currentMode;
}

void SoundPlayer_setTempo(int bpm) {
  if (bpm < MIN_BPM) {
    bpm = MIN_BPM;
  } else if (bpm > MAX_BPM) {
    bpm = MAX_BPM;
  }

  pthread_mutex_lock(&soundPlayerMutex);
  { tempoBPM = bpm; }
  pthread_mutex_unlock(&soundPlayerMutex);
}

int SoundPlayer_getTempo(void) {
  int tempo;

  pthread_mutex_lock(&soundPlayerMutex);
  { tempo = tempoBPM; }
  pthread_mutex_unlock(&soundPlayerMutex);

  return tempo;
}

void SoundPlayer_setVolume(int vol) {
  if (vol < MIN_VOL) {
    vol = MIN_VOL;
  } else if (vol > MAX_VOL) {
    vol = MAX_VOL;
  }

  pthread_mutex_lock(&soundPlayerMutex);
  { volume = vol; }
  pthread_mutex_unlock(&soundPlayerMutex);
}

int SoundPlayer_getVolume(void) {
  int currentVol;

  pthread_mutex_lock(&soundPlayerMutex);
  { currentVol = volume; }
  pthread_mutex_unlock(&soundPlayerMutex);

  return currentVol;
}

void SoundPlayer_cleanup(void) {
  shutdown = true;
  pthread_mutex_unlock(&soundPlayerMutex);
  pthread_join(playBeatTid, NULL);
  AudioMixer_freeWaveFileData(&baseSound);
  AudioMixer_freeWaveFileData(&snareSound);
  AudioMixer_freeWaveFileData(&hihatSound);
  AudioMixer_cleanup();
}