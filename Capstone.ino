

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include "Constants.h"
#include "Fretboard.h"
#include "MIDI.h"
#include "Notes.h"
#include "PINS.h"
#include "StateMachine.h"

// extern Musical_note_TO_LED_idx[128][2];

Adafruit_NeoPixel pixels(NUMPIXELS, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

enum USER_MODE { TRAINING, CONTINUOUS };

USER_MODE mode;

double uS_per_tick;

StateMachine fsm;
int NOTE_IDX = 0;
void printState() {
  switch (fsm.getState()) {
  case WAIT_TO_START: {
    Serial.println("FSM state: WAIT_TO_START");
    break;
  }
  case RECEIVING_SONG: {
    Serial.println("FSM state: RECEIVING_SONG");
    break;
  }
  case WAIT_FOR_STRUM: {
    Serial.println("FSM state: WAIT_FOR_STRUM");
    break;
  }
  case USER_EXPERIENCE: {
    Serial.println("FSM state: USER_EXPERIENCE");
    break;
  }
  case PAUSED: {
    Serial.println("FSM state: PAUSED");
    break;
  }
  }
}

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

void setup() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
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
unsigned long buzzerONtime = 100'000;  // [us]
unsigned long buzzerOFFtime = 500'000; // [us]
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
      pixels.clear(); // Set all pixel colors to 'off'
      pixels.show();  // Send the updated pixel colors to the hardware.
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
        bool move_onto_next_note =
            (time_last_LED_was_turned_off + LED_OFF_TIME < micros());
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

        if (strum and
            notePlayed == expected_note.note) { // move onto the next note
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
        pixels.show(); // Send the updated pixel colors to the hardware.
      } else {         // CONTINUOUS MODE

        // TODO aggregate stats
        auto us_duration = uS_per_tick * expected_note.duration;
        unsigned long long delayStartTime = micros();
        unsigned long long delayTime = us_duration;

        while (micros() < delayStartTime + delayTime) {
          // sample fretbaord and pick
          if (nextFretboardSampleTime <=
              micros()) { // enough time elapsed since last sample
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
        pixels.show(); // Send the updated pixel colors to the hardware.
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
