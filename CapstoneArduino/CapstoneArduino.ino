

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#include "Buzzer.h"
#include "Constants.h"
#include "Fretboard.h"
#include "Notes.h"
#include "PINS.h"
#include "Parsing.h"
#include "StateMachine.h"
#include <cassert>
#include <limits.h>

#define time_since_first_strum_ms \
  ((long long)(((long long)millis()) - time_of_first_strum_ms))

uint8_t noteFile[MAX_NOTE_FILE_SIZE];

Adafruit_NeoPixel pixels(NUMPIXELS, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

NOTE_FILE_METADATA_t metadata;

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

void readPinsAndUpdateFSM() {
  bool file_transmission = digitalRead(fileTransmissionInterruptPin);
  bool strum = false;
  bool done = false;
  bool pause = digitalRead(pauseInterruptPin);
  bool restart = digitalRead(restartInterruptPin);

  Serial.print("file_transmission: ");
  Serial.println(file_transmission);

  Serial.print("strum: ");
  Serial.println(strum);

  Serial.print("done: ");
  Serial.println(done);

  Serial.print("pause: ");
  Serial.println(pause);

  Serial.print("restart: ");
  Serial.println(restart);

  fsm.update(file_transmission, strum, done, pause, restart);
}

void handleFileInterrupt() {
  Serial.println("handleFileInterrupt");
  printState();
  readPinsAndUpdateFSM();
}

void handlePauseInterrupt() {
  Serial.println("handlePauseInterrupt");
  printState();
  readPinsAndUpdateFSM();
}

void handleRestartInterrupt() {
  Serial.println("handleRestartInterrupt");
  printState();
  readPinsAndUpdateFSM();
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

  metadata.mode = TRAINING;  // default
  metadata.tempo_BPM = 60;   // default

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
  delay(500);

  buzzer =
    start_Buzzer(1000, 50,
                 10);  // Default buzzer setup to make Arduino compiler happy
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
unsigned long buzzerONtime_ms = 100;   // [ms]
unsigned long buzzerOFFtime_ms = 500;  // [ms]
unsigned long nextPrintTime_ms = 0;    // [ms]
void loop() {
  while (fsm.getState() == WAIT_TO_START) {
    if (nextPrintTime_ms < millis()) {
      Serial.println("Waiting to start");
      nextPrintTime_ms += 500;
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
        assert(0 <= bytePosition);
        assert(bytePosition < MAX_NOTE_FILE_SIZE);
        noteFile[bytePosition] = incomingByte;
        bytePosition++;
      }
      if (nextPrintTime_ms < millis()) {
        Serial.println("q");
        nextPrintTime_ms += 500;
      }
    }
  }

  while (fsm.getState() == PARSING_SONG) {
    uint32_t file_length = parse_uint32_t(&noteFile[0]);
    Serial.println(file_length);
    Serial.println(bytePosition);
    assert(file_length == bytePosition);

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
    metadata = parseNoteFile(noteFile);
    printNotes();
    fsm.setState(USER_EXPERIENCE);
  }

  unsigned long long nextFretboardSampleTime_ms = 0;       // [ms]
  unsigned long long time_last_LED_was_turned_off_ms = 0;  // [ms]

  bool first_time_in_user_experience = true;
  long long time_entered_user_experience_ms = 0;  // [ms]

  bool first_strum = true;
  long long time_of_first_strum_ms = LONG_MAX;  // [ms]

  long long earliest_time_to_play_note_ms = 0;  // [us]
  long long latest_time_to_play_note_ms = 0;    // [ms]

  long long current_note_starttime_ms = 0;  // [ms]
  long long next_note_starttime_ms = 0;     // [ms]
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
      nextFretboardSampleTime_ms = millis();
      first_time_in_user_experience = false;
      time_last_LED_was_turned_off_ms = 0;
      time_entered_user_experience_ms = millis();
    }

    if (NOTE_IDX < NUM_NOTES_FOUND) {
      if (metadata.mode == TRAINING) {
        NOTE_IDX = max(0, NOTE_IDX);  // don't let note_idx be negative
        NOTE_t expected_note = notes[NOTE_IDX];
        uint8_t LED_idx = get_LEDidx_from_note(expected_note);

        bool LED_already_ON = pixels.getPixelColor(LED_idx);
        bool move_onto_next_note =
          (time_last_LED_was_turned_off_ms + LED_OFF_TIME_ms < millis());
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
            time_last_LED_was_turned_off_ms = millis();
            NOTE_IDX++;
            HWSerial_write_int(1);
          } else {
            HWSerial_write_int(0);
          }
          Serial.println("STRUMMED");
        }
      } else {                                                           // PERFORMANCE MODE - TODO aggregate stats
        if (latest_time_to_play_note_ms <= time_since_first_strum_ms) {  // time to move onto next note
          if (0 <= NOTE_IDX and not attempted_to_strum_note) {
            Serial.print("------ LATE. Did not attempt strumming note ");
            Serial.print(NOTE_IDX);
            Serial.println(" LATE ------");

            long long delta_ms = (signed long)time_since_first_strum_ms - (signed long)current_note_starttime_ms;
            int feedback = delta_ms & ~0x1;  // lsb is 0 to indicate incorrect
            HWSerial_write_int(feedback);
          }

          attempted_to_strum_note = false;
          NOTE_IDX++;
          earliest_time_to_play_note_ms = latest_time_to_play_note_ms;

          current_note_starttime_ms = notes[NOTE_IDX].startTime;
          next_note_starttime_ms = notes[NOTE_IDX + 1].startTime;

          Serial.println(next_note_starttime_ms - current_note_starttime_ms);
          if (NOTE_IDX < NUM_NOTES_FOUND) {
            latest_time_to_play_note_ms = 0.25 * current_note_starttime_ms + 0.75 * next_note_starttime_ms;
          }
        }

        if (not first_strum) {
          int LED_idx;

          run_Buzzer(buzzer, time_since_first_strum_ms);

          if (current_note_starttime_ms <= time_since_first_strum_ms) {  // late portion of current note
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
            if (0 <= time_since_first_strum_ms and time_since_first_strum_ms <= current_note_starttime_ms - LED_OFF_TIME_ms) {  // before LED OFF margin
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
            time_of_first_strum_ms = millis();
            first_strum = false;

            uint32_t tempo_us_per_beat = 60.0e6 / (double)metadata.tempo_BPM;
            buzzer = start_Buzzer(
              tempo_us_per_beat, BUZZER_VOLUME,
              BUZZER_ON_TIME_ms);  // UPDATE LINE WITH ACTUAL TIME BETWEEN NOTES
                                   // AND WITH ON TIME/VOLUME
          }
          long long delta_ms = (signed long)time_since_first_strum_ms - (signed long)current_note_starttime_ms;

          bool played_note_correctly = false;

          NOTE_t note_played = { .fret_idx = notePressed.fret_idx,
                                 .string = stringStrummed,
                                 .startTime = time_since_first_strum_ms };
          NOTE_t expected_note = notes[NOTE_IDX];
          if (not attempted_to_strum_note) {  // first strum
            Serial.print("Delta: ");
            Serial.print(delta_ms / 1.0e3, 3);
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
