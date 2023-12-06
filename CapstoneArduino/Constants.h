#ifndef _CAPSTONE_CONSTANTS_
#define _CAPSTONE_CONSTANTS_

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 60 // TODO CHANGE ME TO 56

#define MAX_NOTE_FILE_SIZE 100'000 // [bytes]
#define MAX_NOTES 10'000           // [notes] // TODO hold more notes?

// set this to the hardware serial port you wish to use
#define HWSERIAL Serial5

#define NUM_FRETS 15
#define DIGITAL_DELAY_us 5              // [us]
#define FRETBOARD_SAMPLING_PERIOD_ms 10 // [ms]
#define MIN_TIME_BETWEEN_STRUMS_ms 100  // [ms]
#define LED_OFF_TIME_ms 75              // [ms]
#define BUZZER_VOLUME 100               // [50-255]
#define BUZZER_ON_TIME_ms 30            // [ms]
#define BUZZER_TIME_OFFSET_ms 0         // [ms] Higher means beep comes earlier

#endif // _CAPSTONE_CONSTANTS_
