

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#include "MIDI.h"
#include "StateMachine.h"

// extern Musical_note_TO_LED_idx[128][2];

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 6  // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 16  // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 50  // Time (in milliseconds) to pause between pixels

// set this to the hardware serial port you wish to use
#define HWSERIAL Serial2

#define FRETBOARD_SAMPLING_PERIOD (3 * 1.0e6)   // [us]
#define MIN_TIME_BETWEEN_FINGERING (0.25 * 1.0e6)  // [us]
#define NUM_FRETS 5

double uS_per_tick;

StateMachine fsm;
int NOTE_IDX = 0;

#define fretClockPin 3
#define fretSimulusPin 4

#define E_stringPin 24
#define A_stringPin 25
#define D_stringPin 26
#define G_stringPin 27

#define fileTransmissionInterruptPin 23
#define strumInterruptPin 22
#define pauseInterruptPin 21
#define restartInterruptPin 20

void handleFileInterrupt() {
  fsm.update(digitalRead(fileTransmissionInterruptPin), false, false, false, false);
}

unsigned long long timeOfLastStrum = 0;
void handleStrumInterrupt() {
  fsm.update(false, digitalRead(strumInterruptPin), false, false, false);
  timeOfLastStrum = micros();
}

void handlePauseInterrupt() {
  fsm.update(false, false, false, digitalRead(pauseInterruptPin), false);
}

void handleRestartInterrupt() {
  fsm.update(false, false, false, false, digitalRead(restartInterruptPin));
}



uint8_t convertFretCoordinatesToNote(
  bool notePlayedOn_E_string,
  bool notePlayedOn_A_string,
  bool notePlayedOn_D_string,
  bool notePlayedOn_G_string,
  bool fret) {
  if (notePlayedOn_E_string || notePlayedOn_A_string || notePlayedOn_D_string || notePlayedOn_G_string) {
    return 0x36;  // TODO
  } else {
    return 0x0;
  }
}


void clearShiftRegister() {  // clock a 0 though the shift register 2 times to ensure it is cleared
  digitalWrite(fretSimulusPin, LOW);
  delay(50);
  for (int fret = 0; fret < 2 * NUM_FRETS; fret++) {
    digitalWrite(fretClockPin, LOW);
    delay(50);
    digitalWrite(fretClockPin, HIGH);
    delay(50);
    digitalWrite(fretClockPin, LOW);
    delay(50);
  }
}
void loadShiftRegister() {
  digitalWrite(fretSimulusPin, HIGH);
  delay(50);
  digitalWrite(fretClockPin, LOW);
  delay(50);
  digitalWrite(fretClockPin, HIGH);
  delay(50);
  digitalWrite(fretClockPin, LOW);
  delay(50);
  digitalWrite(fretSimulusPin, LOW);
  delay(50);
}

uint8_t notePlayed = 0;
void sampleFrets() {
  clearShiftRegister();
  // Serial.println("Cleared");
  loadShiftRegister();
  // Serial.println("Loaded");
  for (int fret = 0; fret < NUM_FRETS; fret++) {
    bool notePlayedOn_E_string = digitalRead(E_stringPin);
    bool notePlayedOn_A_string = digitalRead(A_stringPin);
    bool notePlayedOn_D_string = digitalRead(D_stringPin);
    bool notePlayedOn_G_string = digitalRead(G_stringPin);
    if (notePlayed = convertFretCoordinatesToNote(notePlayedOn_E_string, notePlayedOn_A_string, notePlayedOn_D_string, notePlayedOn_G_string, fret)) {
      // Serial.print("Detected press on fret ");
      // Serial.println(fret);
      // break;
    }
    digitalWrite(fretClockPin, LOW);
    delay(50);
    digitalWrite(fretClockPin, HIGH);
    delay(50);
    digitalWrite(fretClockPin, LOW);
    delay(50);
    // Serial.println("Clocked");
  }
}

void setup() {
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  Serial.begin(19200);
  HWSERIAL.begin(19200);

  pinMode(fretClockPin, OUTPUT);
  pinMode(fretSimulusPin, OUTPUT);

  pinMode(E_stringPin, INPUT);
  pinMode(A_stringPin, INPUT);
  pinMode(D_stringPin, INPUT);
  pinMode(G_stringPin, INPUT);

  pinMode(fileTransmissionInterruptPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(fileTransmissionInterruptPin), handleFileInterrupt, CHANGE);

  pinMode(strumInterruptPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(strumInterruptPin), handleStrumInterrupt, CHANGE);

  pinMode(pauseInterruptPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pauseInterruptPin), handlePauseInterrupt, CHANGE);

  pinMode(restartInterruptPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(restartInterruptPin), handleRestartInterrupt, CHANGE);

  analogReadResolution(8);

  assert(fsm.getState() == WAIT_TO_START);
}

void loop() {
  unsigned long long nextFretboardSampleTime = micros();
  while (true) {
    if (nextFretboardSampleTime <= micros()) {  // enough time elapsed since last sample
      sampleFrets();
      nextFretboardSampleTime += FRETBOARD_SAMPLING_PERIOD;
    }


    if (micros() - timeOfLastStrum < 0.1e6) {
      // Serial.print("Strummed note ");
      // Serial.println(notePlayed, HEX);
    }
  }



  while (fsm.getState() == WAIT_TO_START) {
    pixels.clear();  // Set all pixel colors to 'off'
    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(100);
    Serial.println("Waiting to start");
  }

  uint32_t bytePosition = 0;
  while (fsm.getState() == RECEIVING_SONG) {
    delay(100);
    Serial.println("RECEIVING_SONG");

    // clearing the MIDI file buffer
    // TODO uncommentme
    // for (int i = 0; i < MAX_MIDI_FILE_SIZE; i++) {
    //   MIDI_file[i] = 0;
    // }
    // while (true) {
    // TODO uncommentme
    // if (HWSERIAL.available() > 0) {
    //   char incomingByte = HWSERIAL.read();
    //   MIDI_file[bytePosition] = incomingByte;
    //   bytePosition++;
    // }
    // }
    NOTE_IDX = 0;
  }

  while (fsm.getState() == WAIT_FOR_STRUM) {
    Serial.println("Parsing MIDI file");
    // parsing the MIDI file
    parseMIDI();
    uS_per_tick = (double)MICROSECONDS_PER_BEAT / (double)TICKS_PER_QUARTER_NOTE;
    Serial.print("uS_per_tick: ");
    Serial.println(uS_per_tick, 10);
    printMIDI();

    while (fsm.getState() == WAIT_FOR_STRUM) {
      delay(100);
      Serial.println("Waiting for strum");
    }
  }

  while (fsm.getState() == USER_EXPERIENCE) {
    Serial.println("USER_EXPERIENCE");
    pixels.clear();  // Set all pixel colors to 'off'
    pixels.show();   // Send the updated pixel colors to the hardware.

    unsigned long long nextFretboardSampleTime = micros();

    while (NOTE_IDX < NUM_NOTES_FOUND and fsm.getState() == USER_EXPERIENCE) {
      auto us_duration = uS_per_tick * notes[NOTE_IDX].duration;

      unsigned long long delayStartTime = micros();
      unsigned long long delayTime = us_duration;
      while (micros() < delayStartTime + delayTime) {
        Serial.println("Sampling frets");
        if (nextFretboardSampleTime <= micros()) {  // enough time elapsed since last sample
          sampleFrets();
          if (notePlayed) {
            Serial.print("Detected note ");
            Serial.print(notePlayed, HEX);
            Serial.println(" played");
          }
          nextFretboardSampleTime += FRETBOARD_SAMPLING_PERIOD;
        }
      }

      // following code block takes 11 - 494 uS to run
      // auto start = micros();
      auto LED_idx = notes[NOTE_IDX].note % NUMPIXELS;
      if (notes[NOTE_IDX].ON) {
        Serial.print("Waiting ");
        Serial.print(us_duration);
        Serial.print(" uS before turning LED ");
        Serial.print(LED_idx);
        Serial.print(" ON (note ");
        Serial.print(notes[NOTE_IDX].note);
        Serial.println(")");

        pixels.setPixelColor(notes[NOTE_IDX].note % NUMPIXELS, pixels.Color(0, 255, 0));
      } else {
        Serial.print("Waiting ");
        Serial.print(us_duration);
        Serial.print(" uS before turning LED ");
        Serial.print(LED_idx);
        Serial.print(" OFF (note ");
        Serial.print(notes[NOTE_IDX].note);
        Serial.println(")");

        pixels.setPixelColor(notes[NOTE_IDX].note % NUMPIXELS, pixels.Color(0, 0, 0));
      }
      pixels.show();  // Send the updated pixel colors to the hardware.
      // auto end = micros();
      // Serial.println((end-start)/1.0e6, 10);

      NOTE_IDX++;
    }

    if (NUM_NOTES_FOUND <= NOTE_IDX) {
      fsm.update(false, false, true, false, false);
    }
  }

  while (fsm.getState() == PAUSED) {
    Serial.println("Paused");
    delay(100);
  }
}
