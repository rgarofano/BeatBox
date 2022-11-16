/* Runs a thread that continuously plays sound for the selected mode.
   Stores parameters for tempo in beats per minute, volume from 0 to 
   100, and which mode is in use (0, 1, or 2). These parameters have 
   a default value but can be set by other modules based on joystick 
   movement. These parameters can be read by other modules such as
   the LED matrix which displays them. Allows other modules to play 
   a base, snare, or hihat sound instantaeously.                    */

#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#define NUM_MODES 3
#define NONE 0
#define ROCK 1
#define CUSTOM 2
#define DEFAULT_MODE ROCK

void SoundPlayer_init(void);
void SoundPlayer_startPlaying(void);
void SoundPlayer_playBaseSoundNow(void);
void SoundPlayer_playSnareSoundNow(void);
void SoundPlayer_playHihatSoundNow(void);
void SoundPlayer_setMode(const int MODE);
int SoundPlayer_getMode(void);
void SoundPlayer_setTempo(int bpm);
int SoundPlayer_getTempo(void);
void SoundPlayer_setVolume(int vol);
int SoundPlayer_getVolume(void);
void SoundPlayer_cleanup(void);

#endif