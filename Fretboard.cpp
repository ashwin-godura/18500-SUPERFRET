#include "Fretboard.h"
#include "Arduino.h"
#include "Constants.h"
#include "Notes.h"
#include "PINS.h"

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

NOTE_t notePlayed;
void sampleFrets() {
  clearShiftRegister();
  // Serial.println("Cleared");
  loadShiftRegister();
  // Serial.println("Loaded");
  STRING string = None;
  notePlayed.string = None;
  for (int fret = 0; fret < NUM_FRETS; fret++) {
    STRING string = None;
    if (digitalRead(E_stringPin)) {
      string = E;
    }
    if (digitalRead(A_stringPin)) {
      string = A;
    }
    if (digitalRead(D_stringPin)) {
      string = D;
    }
    if (digitalRead(G_stringPin)) {
      string = G;
    }
    if (notePlayed.string == None and string != None) {  // if already sensed a note, don't read again and
      // potentially overwrite it.
      notePlayed.fret_idx = fret;
      notePlayed.string = string;
      //   Serial.print(convert_STRING_to_string_idx(string));
      //   Serial.print("\t");
      //   Serial.print(fret);
      //   Serial.print("\t");
      //   Serial.println(notePlayed, HEX);
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