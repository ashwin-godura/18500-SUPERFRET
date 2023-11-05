

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include "MIDI.h"
#include "StateMachine.h"

// extern Musical_note_TO_LED_idx[128][2];

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 6 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 16 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 50 // Time (in milliseconds) to pause between pixels

// set this to the hardware serial port you wish to use
#define HWSERIAL Serial2

#define FRETBOARD_SAMPLING_PERIOD (10 * 1.0e3) // [us]
#define MIN_TIME_BETWEEN_STRUMS (0.01e6)       // [us]
#define NUM_FRETS 15
#define DIGITAL_DELAY 5 // [us]

double uS_per_tick;

StateMachine fsm;
int NOTE_IDX = 0;

#define pickPin 2

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
  fsm.update(digitalRead(fileTransmissionInterruptPin), false, false, false,
             false);
}

// void handleStrumInterrupt() {
//   fsm.update(false, digitalRead(strumInterruptPin), false, false, false);
//   timeOfLastStrum = micros();
// }

void handlePauseInterrupt() {
  fsm.update(false, false, false, digitalRead(pauseInterruptPin), false);
}

void handleRestartInterrupt() {
  fsm.update(false, false, false, false, digitalRead(restartInterruptPin));
}

uint8_t convertFretCoordinatesToNote(bool notePlayedOn_E_string,
                                     bool notePlayedOn_A_string,
                                     bool notePlayedOn_D_string,
                                     bool notePlayedOn_G_string, uint8_t fret) {
  if (notePlayedOn_E_string) {
    // Serial.print("Detected press on E string, fret ");
    // Serial.println(fret);
    return 0xEE; // TODO
  } else if (notePlayedOn_A_string) {
    // Serial.print("Detected press on A string, fret ");
    // Serial.println(fret);
    return 0xAA; // TODO
  } else if (notePlayedOn_D_string) {
    // Serial.print("Detected press on D string, fret ");
    // Serial.println(fret);
    return 0xDD; // TODO
  } else if (notePlayedOn_G_string) {
    // Serial.print("Detected press on G string, fret ");
    // Serial.println(fret);
    return 0xFF; // TODO
  } else {
    return 0x0;
  }
}

void clearShiftRegister() { // clock a 0 though the shift register NUM_FRETS + 1
                            // times to ensure it is cleared
  digitalWrite(fretSimulusPin, LOW);
  for (int fret = 0; fret < NUM_FRETS + 1; fret++) {
    digitalWrite(fretClockPin, LOW);
    delayMicroseconds(DIGITAL_DELAY);
    digitalWrite(fretClockPin, HIGH);
    delayMicroseconds(DIGITAL_DELAY);
    digitalWrite(fretClockPin, LOW);
    delayMicroseconds(DIGITAL_DELAY);
  }
}
void loadShiftRegister() {
  digitalWrite(fretSimulusPin, HIGH);
  delayMicroseconds(DIGITAL_DELAY);
  digitalWrite(fretClockPin, LOW);
  delayMicroseconds(DIGITAL_DELAY);
  digitalWrite(fretClockPin, HIGH);
  delayMicroseconds(DIGITAL_DELAY);
  digitalWrite(fretClockPin, LOW);
  delayMicroseconds(DIGITAL_DELAY);
  digitalWrite(fretSimulusPin, LOW);
  delayMicroseconds(DIGITAL_DELAY);
}

uint8_t notePlayed = 0;
void sampleFrets() {
  clearShiftRegister();
  // Serial.println("Cleared");
  loadShiftRegister();
  // Serial.println("Loaded");
  notePlayed = 0;
  for (int fret = 0; fret < NUM_FRETS; fret++) {
    bool notePlayedOn_E_string = digitalRead(E_stringPin);
    bool notePlayedOn_A_string = digitalRead(A_stringPin);
    bool notePlayedOn_D_string = digitalRead(D_stringPin);
    bool notePlayedOn_G_string = digitalRead(G_stringPin);
    if (not notePlayed) {
      notePlayed = convertFretCoordinatesToNote(
          notePlayedOn_E_string, notePlayedOn_A_string, notePlayedOn_D_string,
          notePlayedOn_G_string, fret);
    }
    //  else {
    //   Serial.println("notePlayed");
    // }
    digitalWrite(fretClockPin, LOW);
    delayMicroseconds(DIGITAL_DELAY);
    digitalWrite(fretClockPin, HIGH);
    delayMicroseconds(DIGITAL_DELAY);
    digitalWrite(fretClockPin, LOW);
    delayMicroseconds(DIGITAL_DELAY);
    // Serial.println("Clocked");
  }
}

int E_stringPin_count;
int A_stringPin_count;
int D_stringPin_count;
int G_stringPin_count;
unsigned long long timeOfLastStrum = 0;
bool prevStrumDetection = false;
bool strum = false;
void samplePick() {
  E_stringPin_count = 0;
  A_stringPin_count = 0;
  D_stringPin_count = 0;
  G_stringPin_count = 0;
  bool currStrumDetection = false;
  //
  pinMode(pickPin, OUTPUT);
  digitalWrite(pickPin, HIGH);
  delayMicroseconds(DIGITAL_DELAY);
  for (int i = 0; i < 5; i++) {
    E_stringPin_count += digitalRead(E_stringPin);
    A_stringPin_count += digitalRead(A_stringPin);
    D_stringPin_count += digitalRead(D_stringPin);
    G_stringPin_count += digitalRead(G_stringPin);
    delayMicroseconds(10);
  }
  digitalWrite(pickPin, LOW);
  pinMode(pickPin, INPUT);
  //
  if (E_stringPin_count || A_stringPin_count || D_stringPin_count ||
      G_stringPin_count) {
    currStrumDetection = true;
  }
  // if (E_stringPin_count) {
  //   Serial.println("Strum on E string");
  // } else if (A_stringPin_count) {
  //   Serial.println("Strum on A string");
  // } else if (D_stringPin_count) {
  //   Serial.println("Strum on D string");
  // } else if (G_stringPin_count) {
  //   Serial.println("Strum on G string");
  // }
  strum = prevStrumDetection and not currStrumDetection;
  if (strum) {
    fsm.update(false, digitalRead(strumInterruptPin), false, false, false);
    timeOfLastStrum = micros();
    // Serial.println("Strummed");
  }
  prevStrumDetection = currStrumDetection;
  return false;
}

void setup() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  Serial.begin(19200);
  HWSERIAL.begin(19200);

  pinMode(pickPin, INPUT);

  pinMode(fretClockPin, OUTPUT);
  pinMode(fretSimulusPin, OUTPUT);

  pinMode(E_stringPin, INPUT);
  pinMode(A_stringPin, INPUT);
  pinMode(D_stringPin, INPUT);
  pinMode(G_stringPin, INPUT);

  pinMode(fileTransmissionInterruptPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(fileTransmissionInterruptPin),
                  handleFileInterrupt, CHANGE);

  // pinMode(strumInterruptPin, INPUT_PULLDOWN);
  // attachInterrupt(digitalPinToInterrupt(strumInterruptPin),
  // handleStrumInterrupt, CHANGE);

  pinMode(pauseInterruptPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pauseInterruptPin),
                  handlePauseInterrupt, CHANGE);

  pinMode(restartInterruptPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(restartInterruptPin),
                  handleRestartInterrupt, CHANGE);

  analogReadResolution(8);

  assert(fsm.getState() == WAIT_TO_START);
}

void loop() {
  unsigned long long nextFretboardSampleTime = micros();
  while (true) {
    bool prevStrumStatus = false;
    if (nextFretboardSampleTime <=
        micros()) { // enough time elapsed since last sample
      // auto start = micros();
      sampleFrets();
      samplePick();
      // auto end = micros();
      // Serial.println((end - start) / 1000.0);
      // Serial.println(notePlayed);

      // Serial.print(notePlayed);
      // Serial.print('\t');
      // Serial.print(strum);
      // Serial.print('\t');
      // Serial.print((micros() - timeOfLastStrum) / 1e6);
      // Serial.print('\t');
      // Serial.println(MIN_TIME_BETWEEN_STRUMS / 1e6);

      if (notePlayed and strum and
          ((micros() - timeOfLastStrum) < MIN_TIME_BETWEEN_STRUMS)) {
        Serial.print("Strummed note ");
        Serial.println(notePlayed, HEX);
      }
      nextFretboardSampleTime += FRETBOARD_SAMPLING_PERIOD;
    }
  }

  while (fsm.getState() == WAIT_TO_START) {
    pixels.clear(); // Set all pixel colors to 'off'
    pixels.show();  // Send the updated pixel colors to the hardware.

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
    uS_per_tick =
        (double)MICROSECONDS_PER_BEAT / (double)TICKS_PER_QUARTER_NOTE;
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
    pixels.clear(); // Set all pixel colors to 'off'
    pixels.show();  // Send the updated pixel colors to the hardware.

    unsigned long long nextFretboardSampleTime = micros();

    while (NOTE_IDX < NUM_NOTES_FOUND and fsm.getState() == USER_EXPERIENCE) {
      auto us_duration = uS_per_tick * notes[NOTE_IDX].duration;

      unsigned long long delayStartTime = micros();
      unsigned long long delayTime = us_duration;
      while (micros() < delayStartTime + delayTime) {
        if (nextFretboardSampleTime <=
            micros()) { // enough time elapsed since last sample
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

        pixels.setPixelColor(notes[NOTE_IDX].note % NUMPIXELS,
                             pixels.Color(0, 255, 0));
      } else {
        Serial.print("Waiting ");
        Serial.print(us_duration);
        Serial.print(" uS before turning LED ");
        Serial.print(LED_idx);
        Serial.print(" OFF (note ");
        Serial.print(notes[NOTE_IDX].note);
        Serial.println(")");

        pixels.setPixelColor(notes[NOTE_IDX].note % NUMPIXELS,
                             pixels.Color(0, 0, 0));
      }
      pixels.show(); // Send the updated pixel colors to the hardware.
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
