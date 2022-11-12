#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "I2C.h"
#include "Joystick.h"
#include "Sleep.h"
#include "SoundPlayer.h"
#include "AudioMixer.h"

#define NUM_BYTES 8
#define BUFF_SIZE 16
#define LED_MATRIX_IDX 0
#define LED_SETUP_REG 0x21
#define DISPLAY_SETUP_REG 0x81
#define DISPLAY_REG 0x00
#define WRITE_TURN_ON 0x00
#define FIRST_DIGIT_MASK 0b01110000
#define SECOND_DIGIT_MASK 0b10000011
#define FLOATING_POINT 0b00000100
#define VOLUME_OFFSET 5
#define TEMPO_OFFSET 5

typedef struct {
    unsigned char bytes[NUM_BYTES];
} bitPattern_t;

static bitPattern_t digits[10] = {
    // row   0x00  0x02  0x04  0x06  0x08  0x0A  0x0C  0x0E
    /* 0 */ {{0x00, 0xF3, 0xD2, 0xD2, 0xD2, 0xD2, 0xD2, 0xF3}},
    /* 1 */ {{0x00, 0xF3, 0x21, 0x21, 0x21, 0x21, 0x63, 0x21}},
    /* 2 */ {{0x00, 0xF3, 0x42, 0x63, 0xB1, 0x90, 0xD2, 0xF3}},
    /* 3 */ {{0x00, 0xF3, 0x90, 0x90, 0xF3, 0x90, 0x90, 0xF3}},
    /* 4 */ {{0x00, 0x90, 0x90, 0x90, 0xF3, 0xD2, 0xD2, 0x90}},
    /* 5 */ {{0x00, 0xF3, 0x90, 0x90, 0xF3, 0x42, 0x42, 0xF3}},
    /* 6 */ {{0x00, 0xF3, 0xD2, 0xD2, 0xF3, 0x42, 0x42, 0xF3}},
    /* 7 */ {{0x00, 0x21, 0x21, 0x21, 0x90, 0x90, 0x90, 0xF3}},
    /* 8 */ {{0x00, 0xF3, 0xD2, 0xD2, 0xF3, 0xD2, 0xD2, 0xF3}},
    /* 9 */ {{0x00, 0xF3, 0x90, 0x90, 0xF3, 0xD2, 0xD2, 0xF3}}
};

static unsigned char mBytes[NUM_BYTES] = {0x00, 0x50, 0x50, 0x50, 0x50, 0x70, 0x70, 0x50};

static pthread_t displayTid;
static bool shutdown = false;

void LEDMatrix_initMatrix(void)
{
    I2C_configurePins(LED_MATRIX_IDX);
    I2C_initI2CBus(LED_MATRIX_IDX);
    I2C_writeByteToI2CReg(LED_MATRIX_IDX, LED_SETUP_REG, WRITE_TURN_ON);
    I2C_writeByteToI2CReg(LED_MATRIX_IDX, DISPLAY_SETUP_REG, WRITE_TURN_ON);
}

static void displayDigits(unsigned char* buff, unsigned char* firstDigitBytes, unsigned char* secondDigitBytes)
{
    for (int i = 1; i < NUM_BYTES; i++) {
        unsigned char firstDigitBits = firstDigitBytes[i];
        unsigned char secondDigitBits = secondDigitBytes[i];
        firstDigitBits &= FIRST_DIGIT_MASK;
        secondDigitBits &= SECOND_DIGIT_MASK;
        buff[i * 2] = firstDigitBits + secondDigitBits;
    }

    // write the bytes to the I2C register to display them
    I2C_writeBytesToI2CReg(LED_MATRIX_IDX, DISPLAY_REG, buff, BUFF_SIZE);
}

static void displayInt(int number) 
{
    // error handling
    if (number < 0 || number > 99) {
        displayInt(99);
        return;
    }

    // determine which digits need to be displayed and get the bit patterns for them
    const int FIRST_DIGIT = number / 10;
    const int SECOND_DIGIT = number % 10;
    unsigned char* firstDigitBytes = digits[FIRST_DIGIT].bytes;   
    unsigned char* secondDigitBytes = digits[SECOND_DIGIT].bytes;

    // initialize byte array for the I2C register
    unsigned char buff[BUFF_SIZE] = {};
    displayDigits(buff, firstDigitBytes, secondDigitBytes);
}

static void displayMode()
{
    int mode = SoundPlayer_getMode();

    unsigned char* secondDigitBytes = digits[mode].bytes;

    unsigned char buff[BUFF_SIZE] = {};
    displayDigits(buff, mBytes, secondDigitBytes);
}

void* updateDisplayOnJoystickPress(void* _args)
{
    while(!shutdown) {
        if (Joystick_up()) {

            int currentVolume = SoundPlayer_getVolume();
            SoundPlayer_setVolume(currentVolume + VOLUME_OFFSET);
            int volume = SoundPlayer_getVolume();
            displayInt(volume);
            Sleep_waitForMs(500);

        } else if (Joystick_down()) {

            int currentVolume = SoundPlayer_getVolume();
            SoundPlayer_setVolume(currentVolume - VOLUME_OFFSET);
            int volume = SoundPlayer_getVolume();
            displayInt(volume);
            Sleep_waitForMs(500);

        } else if (Joystick_left()) {

            int currentTempo = SoundPlayer_getTempo();
            SoundPlayer_setTempo(currentTempo - TEMPO_OFFSET);
            int tempoBPM = SoundPlayer_getTempo();
            displayInt(tempoBPM);
            Sleep_waitForMs(500);

        } else if (Joystick_right()) {

            
            int currentTempo = SoundPlayer_getTempo();
            SoundPlayer_setTempo(currentTempo + TEMPO_OFFSET);
            int tempoBPM = SoundPlayer_getTempo();
            displayInt(tempoBPM);
            Sleep_waitForMs(500);

        } else {

            displayMode();
        }
    }
    pthread_exit(NULL);
}

void LEDMatrix_startDisplay(void)
{
    pthread_create(&displayTid, NULL, updateDisplayOnJoystickPress, NULL);
}

static void clearDisplay(void)
{
    unsigned char buff[BUFF_SIZE] = {0x00};
    I2C_writeBytesToI2CReg(LED_MATRIX_IDX, DISPLAY_REG, buff, BUFF_SIZE);
}

void LEDMatrix_cleanup(void)
{
    shutdown = true;
    pthread_join(displayTid, NULL);
    clearDisplay();
}