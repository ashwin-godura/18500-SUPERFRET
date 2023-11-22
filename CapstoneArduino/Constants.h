#ifndef _CAPSTONE_CONSTANTS_
#define _CAPSTONE_CONSTANTS_

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 60  // TODO CHANGE ME TO 56

#define MAX_NOTE_FILE_SIZE 10'000  // [bytes]
#define MAX_NOTES 1000             // [notes] // TODO hold more notes?

// set this to the hardware serial port you wish to use
#define HWSERIAL Serial5

#define FRETBOARD_SAMPLING_PERIOD (10 * 1.0e3)  // [us]
#define MIN_TIME_BETWEEN_STRUMS (0.1e6)         // [us]
#define NUM_FRETS 15                             // TODO CHANGE ME TO 15
#define DIGITAL_DELAY 5                         // [us]
#define LED_OFF_TIME 0.075e6                      // [us]
#define BUZZER_VOLUME 0                         //[50-255]

#endif  // _CAPSTONE_CONSTANTS_
