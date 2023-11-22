

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#include "Constants.h"
#include "Fretboard.h"
#include "Notes.h"
#include "PINS.h"
#include "Parsing.h"
#include "StateMachine.h"
#include <cassert>

uint8_t noteFile[MAX_NOTE_FILE_SIZE];

Adafruit_NeoPixel pixels(NUMPIXELS, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

enum USER_MODE { TRAINING,
                 PERFORMANCE };

USER_MODE mode;

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
    notePlayed.fret_idx = i;
    for (int s = 0; s < 4; s++) {
      notePlayed.string = convert_string_idx_to_STRING(s);
      uint8_t LED_idx = get_LEDidx_from_note(notePlayed);
      pixels.setPixelColor(LED_idx, convert_Note_To_COLOR(notePlayed));
    }
    delay(100);
    pixels.show();
  }
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

  // TODO check if pi wants the system to be in TRAINING or PERFORMANCE mode

  while (fsm.getState() == WAIT_TO_START) {
    NUM_NOTES_FOUND = 0;
    NOTE_IDX = -1;
    delay(100);
    Serial.println("Waiting to start");
  }

  uint32_t bytePosition = 0;
  while (fsm.getState() == RECEIVING_SONG) {
    Serial.println("RECEIVING_SONG");

    // clearing the Note file buffer
    for (int i = 0; i < MAX_NOTE_FILE_SIZE; i++) {
      noteFile[i] = 0;
    }
    while (fsm.getState() == RECEIVING_SONG) {
      //   TODO uncommentme
      if (HWSERIAL.available() > 0) {
        char incomingByte = HWSERIAL.read();
        // Serial.print(bytePosition);
        // Serial.print("\t");
        // Serial.println(incomingByte, HEX);
        noteFile[bytePosition] = incomingByte;
        bytePosition++;
      }
    }
  }

  while (fsm.getState() == WAIT_FOR_STRUM) {
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

    auto nextPrintTime = micros();
    while (fsm.getState() == WAIT_FOR_STRUM) {
      if (nextPrintTime < micros()) {
        pixels.clear();
        pixels.show();
        Serial.println("Waiting for strum");
        nextPrintTime += 100e3;
      }
      samplePick();
      delay(10);
    }
  }

  unsigned long long nextFretboardSampleTime = 0;  // [us]
  bool first_time_in_user_experience = true;
  unsigned long long time_last_LED_was_turned_off = 0;  // [us]
  unsigned long long time_entered_user_experience = 0;  // [us]

  unsigned long long earliest_time_to_play_note = 0;  // [us]
  unsigned long long latest_time_to_play_note = 0;    // [us]

  unsigned long long current_note_starttime = 0;  // [us]
  unsigned long long next_note_starttime = 0;     // [us]

  while (fsm.getState() == USER_EXPERIENCE) {
    if (first_time_in_user_experience) {
      Serial.println("USER_EXPERIENCE");
      pixels.clear();  // Set all pixel colors to 'off'
      pixels.show();   // Send the updated pixel colors to the hardware.
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
          Serial.print("Turning LED ");
          Serial.print(LED_idx);
          Serial.print(" ON. Note ");
          Serial.print(NOTE_IDX);
          Serial.print(": ");
          printNote(expected_note);
          Serial.println("]");
          pixels.setPixelColor(LED_idx, convert_Note_To_COLOR(expected_note));
        }

        sampleFrets();
        samplePick();

        if (strum) {
          printNote(notePlayed);
          Serial.println();
        }
        if (strum and notePlayed.valid()) {
          Serial.print("Note index: ");
          Serial.println(NOTE_IDX, DEC);

          Serial.print("Played: ");
          printNote(notePlayed);
          Serial.println();

          Serial.print("Expected: ");
          printNote(expected_note);
          Serial.println();
        }

        bool strummed_correctly =
          (strum and (stringStrummed == expected_note.string));
        bool played_open_string =
          (notePlayed.fret_idx == 0 and expected_note.fret_idx == 0);
        if (strummed_correctly and (notePlayed == expected_note or played_open_string)) {  // move onto the next note
          Serial.print("Turning LED ");
          Serial.print(LED_idx);
          Serial.print(" OFF. Note ");
          Serial.print(NOTE_IDX);
          Serial.print(": ");
          printNote(expected_note);
          Serial.println("]");
          pixels.setPixelColor(LED_idx, pixels.Color(0, 0, 0));
          time_last_LED_was_turned_off = micros();
          NOTE_IDX++;
        }
      } else {  // PERFORMANCE MODE
                // TODO aggregate stats
        auto time_in_user_experience = micros() - time_entered_user_experience;

        if (latest_time_to_play_note < time_in_user_experience) {  // time to move onto next note
          NOTE_IDX++;
          earliest_time_to_play_note = latest_time_to_play_note;

          current_note_starttime = 1e3 * notes[NOTE_IDX].startTime;   // [us]
          next_note_starttime = 1e3 * notes[NOTE_IDX + 1].startTime;  // [us]

          if (NOTE_IDX < NUM_NOTES_FOUND - 1) {
            latest_time_to_play_note = (current_note_starttime + next_note_starttime) / 2;
          }

          Serial.print(time_in_user_experience / 1.0e6, 3);
          Serial.print("\t");
          Serial.print(earliest_time_to_play_note / 1.0e6, 3);
          Serial.print("\t");
          Serial.print(current_note_starttime / 1.0e6, 3);
          Serial.print(" (");
          Serial.print(NOTE_IDX);
          Serial.print(")\t");
          Serial.print(latest_time_to_play_note / 1.0e6, 3);
          Serial.print(" (");
          Serial.print(NOTE_IDX + 1);
          Serial.print(")\t");
          Serial.println(next_note_starttime / 1.0e6, 3);
        }





        // if (latest_time_to_play_note <= micros()) {  //move onto next note
        //   NOTE_IDX++;
        //   earliest_time_to_play_note = latest_time_to_play_note;
        // }

        NOTE_t expected_note = notes[NOTE_IDX];
        uint8_t LED_idx = get_LEDidx_from_note(expected_note);


        sampleFrets();
        samplePick();
        if (strum) {
          int delta_ms = ((signed long)time_in_user_experience - (signed long)current_note_starttime) / 1000;
          bool strummed_correctly =
            (strum and (stringStrummed == expected_note.string));
          bool played_open_string =
            (notePlayed.fret_idx == 0 and expected_note.fret_idx == 0);
          if (strummed_correctly and (notePlayed == expected_note or played_open_string)) {  // move onto the next note
            Serial.println("Correct -------- Correct -------- Correct -------- Correct");
          } else {
            Serial.println("------ Incorrect ------ Incorrect ------ Incorrect ------");
          }
          Serial.print("Delta: ");
          Serial.print(delta_ms / 1000.0);
          Serial.println(" s");
        }

        /*
        bool LED_already_ON = pixels.getPixelColor(LED_idx);
        if (current_note_starttime <= time_in_user_experience and time_in_user_experience < next_note_starttime - LED_OFF_TIME) {
          if (not LED_already_ON) {
            Serial.print("Turning LED ");
            Serial.print(LED_idx);
            Serial.print(" ON. Note ");
            Serial.print(NOTE_IDX);
            Serial.print(": ");
            printNote(expected_note);
            Serial.println("]");
            pixels.setPixelColor(LED_idx, convert_Note_To_COLOR(expected_note));
          }
        } else if (next_note_starttime - LED_OFF_TIME <= time_in_user_experience and time_in_user_experience < next_note_starttime) {
          if (LED_already_ON) {
            Serial.print("Turning LED ");
            Serial.print(LED_idx);
            Serial.print(" OFF. Note ");
            Serial.print(NOTE_IDX);
            Serial.print(": ");
            printNote(expected_note);
            Serial.println("]");
            pixels.setPixelColor(LED_idx, pixels.Color(0, 0, 0));
          }
        }
        */
      }
      pixels.show();  // Send the updated pixel colors to the hardware.
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
