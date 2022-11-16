/* Intitalizes GPIO buttons for use and runs a thread which check for button
   presses and does the relevant action such as playing a sound or switching
   beats                                                                     */

#ifndef BUTTONS_H
#define BUTTONS_H

void Buttons_initButtons(void);
void Buttons_startRunning(void);
void Buttons_cleanup(void);

#endif