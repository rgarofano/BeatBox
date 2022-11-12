#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#define NUM_MODES 3
#define NONE 0
#define ROCK 1
#define CUSTOM 2
#define DEFAULT_MODE ROCK

void SoundPlayer_init(void);
void SoundPlayer_startPlaying(void);
long long SoundPlayer_playBaseSoundNow(void);
long long SoundPlayer_playSnareSoundNow(void);
long long SoundPlayer_playHihatSoundNow(void);
void SoundPlayer_setMode(const int MODE);
int SoundPlayer_getMode(void);
void SoundPlayer_setTempo(int bpm);
int SoundPlayer_getTempo(void);
void SoundPlayer_setVolume(int vol);
int SoundPlayer_getVolume(void);
void SoundPlayer_cleanup(void);

#endif