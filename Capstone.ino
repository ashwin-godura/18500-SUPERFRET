

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#include "MIDI.h"
#include "StateMachine.h"

// extern Musical_note_TO_LED_idx[128][2];

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 2  // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 16  // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// set this to the hardware serial port you wish to use
#define HWSERIAL Serial2

#define FRETBOARD_SAMPLING_PERIOD (10 * 1.0e3)  // [us]
#define MIN_TIME_BETWEEN_STRUMS (0.2e6)         // [us]
#define NUM_FRETS 15
#define DIGITAL_DELAY 5     // [us]
#define LED_OFF_TIME 0.3e6  // [us]

enum USER_MODE { TRAINING,
                 CONTINUOUS };

USER_MODE mode;

double uS_per_tick;

StateMachine fsm;
int NOTE_IDX = 0;

#define pickPin 12

#define fretClockPin 11
#define fretSimulusPin 4

#define E_stringPin 24
#define A_stringPin 25
#define D_stringPin 26
#define G_stringPin 27

#define fileTransmissionInterruptPin 19
#define pauseInterruptPin 18
#define restartInterruptPin 17
#define UNUSED_INTERRUPT_4 15
#define UNUSED_INTERRUPT_5 14
#define UNUSED_INTERRUPT_6 13
#define UNUSED_INTERRUPT_7 40
#define UNUSED_INTERRUPT_8 39
#define UNUSED_INTERRUPT_9 38
#define UNUSED_INTERRUPT_10 37

#define BUZZER_PIN 28
#define BUZZER_VOLUME 0  //[50-255]

void handleFileInterrupt() {
  fsm.update(digitalRead(fileTransmissionInterruptPin), false, false, false,
             false);
}

void handlePauseInterrupt() {
  fsm.update(false, false, false, digitalRead(pauseInterruptPin), false);
}

void handleRestartInterrupt() {
  fsm.update(false, false, false, false, digitalRead(restartInterruptPin));
}

void printState() {
  switch (fsm.getState()) {
    case WAIT_TO_START:
      {
        Serial.println("FSM state: WAIT_TO_START");
        break;
      }
    case RECEIVING_SONG:
      {
        Serial.println("FSM state: RECEIVING_SONG");
        break;
      }
    case WAIT_FOR_STRUM:
      {
        Serial.println("FSM state: WAIT_FOR_STRUM");
        break;
      }
    case USER_EXPERIENCE:
      {
        Serial.println("FSM state: USER_EXPERIENCE");
        break;
      }
    case PAUSED:
      {
        Serial.println("FSM state: PAUSED");
        break;
      }
  }
}


uint8_t get_LEDidx_from_note(uint8_t note) {
  // TODO index into a look-up table
  return note % NUMPIXELS;
}

uint8_t convertFretCoordinatesToNote(bool notePlayedOn_E_string,
                                     bool notePlayedOn_A_string,
                                     bool notePlayedOn_D_string,
                                     bool notePlayedOn_G_string, uint8_t fret) {
  if (notePlayedOn_E_string) {
    // Serial.print("Detected press on E string, fret ");
    // Serial.println(fret);
    if (fret == 0) {
      return 0x43;  // verified
    } else if (fret == 1) {
      return 0x47;
    } else if (fret == 2) {
      return 0x4B;
    } else if (fret == 3) {
      return 0x00;
    } else if (fret == 4) {
      return 0x00;
    }
  } else if (notePlayedOn_A_string) {
    // Serial.print("Detected press on A string, fret ");
    // Serial.println(fret);
    if (fret == 0) {
      return 0x42;
    } else if (fret == 1) {
      return 0x46;
    } else if (fret == 2) {
      return 0x4A;
    } else if (fret == 3) {
      return 0x3E;
    } else if (fret == 4) {
      return 0x00;
    }
  } else if (notePlayedOn_D_string) {
    // Serial.print("Detected press on D string, fret ");
    // Serial.println(fret);
    if (fret == 0) {
      return 0x41;  // verified
    } else if (fret == 1) {
      return 0x45;
    } else if (fret == 2) {
      return 0x49;
    } else if (fret == 3) {
      return 0x3D;
    } else if (fret == 4) {
      return 0x00;
    }
  } else if (notePlayedOn_G_string) {
    // Serial.print("Detected press on G string, fret ");
    // Serial.println(fret);
    if (fret == 0) {
      return 0x40;
    } else if (fret == 1) {
      return 0x44;
    } else if (fret == 2) {
      return 0x48;
    } else if (fret == 3) {
      return 0x3C;  // verified
    } else if (fret == 4) {
      return 0x00;
    }
  } else {
    return 0x0;
  }
}

void clearShiftRegister() {  // clock a 0 though the shift register NUM_FRETS + 1
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
    if (not notePlayed) {  // if already sensed a note, don't read again and potentially overwrite it.
      notePlayed = convertFretCoordinatesToNote(
        notePlayedOn_E_string, notePlayedOn_A_string, notePlayedOn_D_string,
        notePlayedOn_G_string, fret);
    } else {
      // Serial.println(notePlayed, HEX);
    }
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
  strum = false;
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
  if (E_stringPin_count || A_stringPin_count || D_stringPin_count || G_stringPin_count) {
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

  if (prevStrumDetection and not currStrumDetection and MIN_TIME_BETWEEN_STRUMS < (micros() - timeOfLastStrum)) {
    Serial.println("Strummed");
    strum = true;
    fsm.update(false, true, false, false, false);
    timeOfLastStrum = micros();
  }

  prevStrumDetection = currStrumDetection;
}

void setup() {
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  Serial.begin(115200);
  HWSERIAL.begin(19200);

  pinMode(pickPin, INPUT);

  pinMode(fretClockPin, OUTPUT);
  pinMode(fretSimulusPin, OUTPUT);

  pinMode(E_stringPin, INPUT);
  pinMode(A_stringPin, INPUT);
  pinMode(D_stringPin, INPUT);
  pinMode(G_stringPin, INPUT);

  // declaring unused interrupt pins as input becuase
  // they are connected to the PI so we don't accidentally
  // backdrive the PI
  pinMode(UNUSED_INTERRUPT_4, INPUT);
  pinMode(UNUSED_INTERRUPT_5, INPUT);
  pinMode(UNUSED_INTERRUPT_6, INPUT);
  pinMode(UNUSED_INTERRUPT_7, INPUT);
  pinMode(UNUSED_INTERRUPT_8, INPUT);
  pinMode(UNUSED_INTERRUPT_9, INPUT);
  pinMode(UNUSED_INTERRUPT_10, INPUT);

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

  pinMode(BUZZER_PIN, OUTPUT);
  analogWriteFrequency(BUZZER_PIN, 100'000);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  assert(fsm.getState() == WAIT_TO_START);
  mode == TRAINING;
}

unsigned long nextBuzzerTime = 0;
bool buzzer_state = false;
unsigned long buzzerONtime = 100'000;   // [us]
unsigned long buzzerOFFtime = 500'000;  // [us]
void loop() {
  //   while (1) {
  //     if (nextBuzzerTime <= micros()) {
  //       if (buzzer_state) {
  //         analogWrite(BUZZER_PIN, BUZZER_VOLUME);
  //         nextBuzzerTime += buzzerONtime;
  //       } else {
  //         analogWrite(BUZZER_PIN, 0);
  //         nextBuzzerTime += buzzerOFFtime;
  //       }
  //       buzzer_state = !buzzer_state;
  //     }
  //   }

  // TODO check if pi wants the system to be in TRAINING or CONTINUOUS mode

  /*
  unsigned long long nextFretboardSampleTime = micros();
  while (true) {
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
  */

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

    auto nextPrintTime = micros();
    while (fsm.getState() == WAIT_FOR_STRUM) {
      if (nextPrintTime < micros()) {
        Serial.println("Waiting for strum");
        nextPrintTime += 100e3;
      }
      samplePick();
      delay(10);
    }
    NOTE_IDX = 0;
  }

  unsigned long long nextFretboardSampleTime = 0;
  bool first_time_in_user_experience = true;
  unsigned long long time_last_LED_was_turned_off = 0;
  while (fsm.getState() == USER_EXPERIENCE) {
    if (first_time_in_user_experience) {
      Serial.println("USER_EXPERIENCE");
      pixels.clear();  // Set all pixel colors to 'off'
      pixels.show();   // Send the updated pixel colors to the hardware.
      nextFretboardSampleTime = micros();
      first_time_in_user_experience = false;
      time_last_LED_was_turned_off = 0;
    }

    NOTE_t expected_note;
    uint8_t LED_idx;
    if (NOTE_IDX < NUM_NOTES_FOUND) {
      expected_note = notes[NOTE_IDX];
      LED_idx = get_LEDidx_from_note(expected_note.note);

      if (mode == TRAINING) {
        while (not expected_note.ON and NOTE_IDX < NUM_NOTES_FOUND) {
          NOTE_IDX++;
          expected_note = notes[NOTE_IDX];
          LED_idx = get_LEDidx_from_note(expected_note.note);
        }

        assert(expected_note.note != 0x0);

        bool LED_already_ON = pixels.getPixelColor(LED_idx);
        bool move_onto_next_note = (time_last_LED_was_turned_off + LED_OFF_TIME < micros());
        if (not LED_already_ON and move_onto_next_note) {
          Serial.print("Turning LED ");
          Serial.print(LED_idx);
          Serial.print(" ON (note ");
          Serial.print(expected_note.note);
          Serial.print(" index ");
          Serial.print(NOTE_IDX);
          Serial.println(")");
          pixels.setPixelColor(LED_idx, pixels.Color(0, 255, 0));
        }

        sampleFrets();
        samplePick();

        if (strum and notePlayed) {
          Serial.print(NOTE_IDX, DEC);
          Serial.print("\t");
          Serial.print(notePlayed, HEX);
          Serial.print("\t");
          Serial.println(expected_note.note, HEX);
        }

        if (strum and notePlayed == expected_note.note) {  // move onto the next note
          Serial.print("Turning LED ");
          Serial.print(LED_idx);
          Serial.print(" OFF (note ");
          Serial.print(expected_note.note);
          Serial.print(" index ");
          Serial.print(NOTE_IDX);
          Serial.println(")");
          pixels.setPixelColor(LED_idx, pixels.Color(0, 0, 0));
          time_last_LED_was_turned_off = micros();
          NOTE_IDX++;
        }
        pixels.show();  // Send the updated pixel colors to the hardware.
      } else {          // CONTINUOUS MODE

        // TODO aggregate stats
        auto us_duration = uS_per_tick * expected_note.duration;
        unsigned long long delayStartTime = micros();
        unsigned long long delayTime = us_duration;

        while (micros() < delayStartTime + delayTime) {
          // sample fretbaord and pick
          if (nextFretboardSampleTime <= micros()) {  // enough time elapsed since last sample
            sampleFrets();
            samplePick();
            if (notePlayed and strum) {
              Serial.print("Detected note ");
              Serial.print(notePlayed, HEX);
              Serial.println(" played");
            }
            nextFretboardSampleTime += FRETBOARD_SAMPLING_PERIOD;
          }
        }

        // following code block takes 11 - 494 uS to run
        // auto start = micros();
        if (expected_note.ON) {
          Serial.print("Waiting ");
          Serial.print(us_duration);
          Serial.print(" uS before turning LED ");
          Serial.print(LED_idx);
          Serial.print(" ON (note ");
          Serial.print(expected_note.note);
          Serial.println(")");

          pixels.setPixelColor(LED_idx, pixels.Color(0, 255, 0));
        } else {
          Serial.print("Waiting ");
          Serial.print(us_duration);
          Serial.print(" uS before turning LED ");
          Serial.print(LED_idx);
          Serial.print(" OFF (note ");
          Serial.print(expected_note.note);
          Serial.println(")");

          pixels.setPixelColor(LED_idx, pixels.Color(0, 0, 0));
        }
        pixels.show();  // Send the updated pixel colors to the hardware.
        // auto end = micros();
        // Serial.println((end-start)/1.0e6, 10);

        NOTE_IDX++;
      }
    }

    if (NOTE_IDX >= NUM_NOTES_FOUND) {
      Serial.println("Done with User Experience");
      fsm.update(false, false, true, false, false);
    }
  }

  while (fsm.getState() == PAUSED) {
    Serial.println("Paused");
    delay(100);
    samplePick();
  }
}
