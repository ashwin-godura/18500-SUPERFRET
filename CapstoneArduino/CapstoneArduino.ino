

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#include "Constants.h"
#include "Fretboard.h"
#include "Notes.h"
#include "PINS.h"
#include "Buzzer.h"
#include "Parsing.h"
#include "StateMachine.h"
#include <cassert>
#include <limits.h>

#define time_in_user_experience \
  (((long long)micros()) - time_entered_user_experience)
#define time_since_first_strum \
  ((long long)(((long long)micros()) - time_of_first_strum))

uint8_t noteFile[MAX_NOTE_FILE_SIZE];

Adafruit_NeoPixel pixels(NUMPIXELS, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

enum USER_MODE { TRAINING,
                 PERFORMANCE };

USER_MODE mode;

BUZZER_t buzzer;

double uS_per_tick;

StateMachine fsm;
int NOTE_IDX = -1;
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
    case PARSING_SONG:
      {
        Serial.println("FSM state: PARSING_SONG");
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

void handleFileInterrupt() {
  Serial.println("handleFileInterrupt");
  Serial.println(digitalRead(fileTransmissionInterruptPin));
  printState();
  fsm.update(digitalRead(fileTransmissionInterruptPin), false, false, false,
             false);
}

void handlePauseInterrupt() {
  Serial.println("handlePauseInterrupt");
  Serial.println(digitalRead(pauseInterruptPin));
  printState();
  fsm.update(false, false, false, digitalRead(pauseInterruptPin), false);
}

void handleRestartInterrupt() {
  Serial.println("handleRestartInterrupt");
  Serial.println(digitalRead(restartInterruptPin));
  printState();
  fsm.update(false, false, false, false, digitalRead(restartInterruptPin));
}

void setup() {
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  Serial.begin(115200);
  HWSERIAL.begin(115200);

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
  mode = PERFORMANCE;
  // mode = TRAINING;

  pixels.clear();
  for (int i = 0; i < NUM_FRETS; i++) {
    notePressed.fret_idx = i;
    for (int s = 0; s < 4; s++) {
      notePressed.string = convert_string_idx_to_STRING(s);
      uint8_t LED_idx = get_LEDidx_from_note(notePressed);
      pixels.setPixelColor(LED_idx, convert_Note_To_COLOR(notePressed));
    }
    pixels.show();
    delay(100);
  }
  delay(2000);
  for (int i = 255; i >= 100; i--) {
    pixels.setBrightness(i);
    pixels.show();
    delay(8);
  }
  pixels.setBrightness(255);

  buzzer = start_Buzzer(1000000, 50, 10000);  //Default buzzer setup to make Arduino compiler happy
}


void HWSerial_write_int(int32_t i) {
  Serial.println("Wrapper");
  HWSERIAL.write((i >> 24) & 0xff);
  HWSERIAL.write((i >> 16) & 0xff);
  HWSERIAL.write((i >> 8) & 0xff);
  HWSERIAL.write(i & 0xff);
}

unsigned long nextBuzzerTime = 0;
bool buzzer_state = false;
unsigned long buzzerONtime = 100'000;   // [us]
unsigned long buzzerOFFtime = 500'000;  // [us]
unsigned long nextPrintTime = 0;        // [us]
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

  // TODO check if pi wants the system to be in TRAINING or PERFORMANCE mode

  while (fsm.getState() == WAIT_TO_START) {
    if (nextPrintTime < micros()) {
      Serial.println("Waiting to start");
      nextPrintTime += 500e3;
      pixels.clear();
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(50, 10, 0));
      }
      pixels.show();
    }
    NUM_NOTES_FOUND = 0;
    NOTE_IDX = -1;
    turn_off_Buzzer(buzzer);
  }

  uint32_t bytePosition = 0;
  while (fsm.getState() == RECEIVING_SONG) {
    Serial.println("RECEIVING_SONG");

    // clearing the Note file buffer
    for (int i = 0; i < MAX_NOTE_FILE_SIZE; i++) {
      noteFile[i] = 0;
    }
    while (fsm.getState() == RECEIVING_SONG) {
      if (HWSERIAL.available() > 0) {
        char incomingByte = HWSERIAL.read();
        noteFile[bytePosition] = incomingByte;
        bytePosition++;
      }
      if (nextPrintTime < micros()) {
        Serial.println("q");
        nextPrintTime += 500e3;
      }
    }
  }

  while (fsm.getState() == PARSING_SONG) {
    for (int i = 0; i < bytePosition; i++) {
      Serial.print("Note file byte ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(noteFile[i], HEX);
      Serial.print(" ");
      Serial.println((char)(noteFile[i]));
    }

    Serial.println("Parsing Note file");
    // parsing the Note file
    parseNoteFile(noteFile);
    printNotes();
    fsm.setState(USER_EXPERIENCE);
  }

  unsigned long long nextFretboardSampleTime = 0;       // [us]
  unsigned long long time_last_LED_was_turned_off = 0;  // [us]

  bool first_time_in_user_experience = true;
  long long time_entered_user_experience = 0;  // [us]

  bool first_strum = true;
  long long time_of_first_strum = LONG_MAX;  // [us]

  long long earliest_time_to_play_note = 0;  // [us]
  long long latest_time_to_play_note = 0;    // [us]

  long long current_note_starttime = 0;  // [us]
  long long next_note_starttime = 0;     // [us]
  bool attempted_to_strum_note = false;
  while (fsm.getState() == USER_EXPERIENCE) {
    if (first_time_in_user_experience) {
      Serial.println("USER_EXPERIENCE");
      pixels.clear();  // Set all pixel colors to 'off'
      // light up the first note
      printNote(notes[0]);
      pixels.setPixelColor(get_LEDidx_from_note(notes[0]),
                           convert_Note_To_COLOR(notes[0]));
      pixels.show();  // Send the updated pixel colors to the hardware.
      nextFretboardSampleTime = micros();
      first_time_in_user_experience = false;
      time_last_LED_was_turned_off = 0;
      time_entered_user_experience = micros();
    }

    if (NOTE_IDX < NUM_NOTES_FOUND) {
      if (mode == TRAINING) {
        NOTE_IDX = max(0, NOTE_IDX);  // don't let note_idx be negative
        NOTE_t expected_note = notes[NOTE_IDX];
        uint8_t LED_idx = get_LEDidx_from_note(expected_note);

        bool LED_already_ON = pixels.getPixelColor(LED_idx);
        bool move_onto_next_note =
          (time_last_LED_was_turned_off + LED_OFF_TIME < micros());
        if (not LED_already_ON and move_onto_next_note) {
          pixels.setPixelColor(LED_idx, convert_Note_To_COLOR(expected_note));
        }

        sampleFrets();
        samplePick();

        if (strum) {
          printNote(notePressed);
          Serial.println();
        }
        if (strum and notePressed.valid()) {
          Serial.print("Note index: ");
          Serial.println(NOTE_IDX, DEC);

          Serial.print("Played: ");
          printNote(notePressed);
          Serial.println();

          Serial.print("Expected: ");
          printNote(expected_note);
          Serial.println();
        }
        if (strum) {
          bool strummed_correctly = stringStrummed == expected_note.string;
          bool played_open_string =
            (notePressed.fret_idx == 0 and expected_note.fret_idx == 0);
          if (strummed_correctly and (notePressed == expected_note or played_open_string)) {  // move onto the next note
            pixels.setPixelColor(LED_idx, pixels.Color(0, 0, 0));
            time_last_LED_was_turned_off = micros();
            NOTE_IDX++;
            HWSerial_write_int(1);
          } else {
            HWSerial_write_int(0);
          }
          Serial.println("STRUMMED");
        }
      } else {                                                     // PERFORMANCE MODE - TODO aggregate stats
        if (latest_time_to_play_note <= time_since_first_strum) {  // time to move onto next note
          if (0 <= NOTE_IDX and not attempted_to_strum_note) {
            Serial.print("------ LATE. Did not attempt strumming note ");
            Serial.print(NOTE_IDX);
            Serial.println(" LATE ------");

            long long delta_us = (signed long)time_since_first_strum - (signed long)current_note_starttime;
            int delta_ms = delta_us / 1000;
            int feedback = delta_ms & ~0x1;  //lsb is 0 to indicate incorrect
            HWSerial_write_int(feedback);
          }

          attempted_to_strum_note = false;
          NOTE_IDX++;
          earliest_time_to_play_note = latest_time_to_play_note;

          current_note_starttime = 1e3 * notes[NOTE_IDX].startTime;   // [us]
          next_note_starttime = 1e3 * notes[NOTE_IDX + 1].startTime;  // [us]

          Serial.println(next_note_starttime - current_note_starttime);
          if (NOTE_IDX < NUM_NOTES_FOUND) {
            latest_time_to_play_note =
              0.25 * current_note_starttime + 0.75 * next_note_starttime;
          }
        }

        if (not first_strum) {
          int LED_idx;
          long long time_since_first_strum_us = time_since_first_strum;
          
          run_Buzzer(buzzer, time_since_first_strum_us);
          
          if (current_note_starttime <= time_since_first_strum_us) {  // late portion of current note
            LED_idx = get_LEDidx_from_note(notes[NOTE_IDX]);
            bool LED_already_ON = pixels.getPixelColor(LED_idx);
            if (not LED_already_ON) {
              pixels.setPixelColor(LED_idx,
                                   convert_Note_To_COLOR(notes[NOTE_IDX]));
            }
          } else {  // early portion of current note
            assert(1 <= NOTE_IDX);
            LED_idx = get_LEDidx_from_note(notes[NOTE_IDX - 1]);
            bool LED_already_ON = pixels.getPixelColor(LED_idx);
            if (0 <= time_since_first_strum and time_since_first_strum <= current_note_starttime - LED_OFF_TIME) {  // before LED OFF margin
              if (not LED_already_ON) {
                pixels.setPixelColor(
                  LED_idx, convert_Note_To_COLOR(notes[NOTE_IDX - 1]));
              }
            } else {  //  within LED OFF time margin
              if (LED_already_ON) {
                pixels.setPixelColor(LED_idx, pixels.Color(0, 0, 0));
              }
            }
          }
        }

        sampleFrets();
        samplePick();
        if (strum) {
          if (first_strum) {
            time_of_first_strum = micros();
            first_strum = false;

            buzzer = start_Buzzer(500000, BUZZER_VOLUME, BUZZER_ON_TIME); //UPDATE LINE WITH ACTUAL TIME BETWEEN NOTES AND WITH ON TIME/VOLUME
            
          }
          long long delta_us = (signed long)time_since_first_strum - (signed long)current_note_starttime;

          bool played_note_correctly = false;

          NOTE_t note_played = { .fret_idx = notePressed.fret_idx,
                                 .string = stringStrummed,
                                 .startTime = time_since_first_strum / 1e3 };
          NOTE_t expected_note = notes[NOTE_IDX];
          if (not attempted_to_strum_note) {  // first strum
            Serial.print("Delta: ");
            Serial.print(delta_us / 1.0e6, 3);
            Serial.println(" s");
            bool strummed_correctly = (stringStrummed == expected_note.string);
            bool played_open_string =
              (notePressed.fret_idx == 0 and expected_note.fret_idx == 0);
            if (strummed_correctly and (notePressed == expected_note or played_open_string)) {  // move onto the next note
              played_note_correctly = true;
              Serial.println(
                "Correct -------- Correct -------- Correct -------- Correct");
            } else {
              Serial.println(
                "------ Incorrect ------ Incorrect ------ Incorrect ------");
            }
            int delta_ms = delta_us / 1000;
            int feedback = (delta_ms & ~0x1) | played_note_correctly;
            HWSerial_write_int(feedback);
          }

          attempted_to_strum_note = true;
        }
      }
    }
    pixels.show();  // Send the updated pixel colors to the hardware.

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
