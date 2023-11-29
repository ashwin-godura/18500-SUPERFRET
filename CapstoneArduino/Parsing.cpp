
#include "Parsing.h"
#include "Arduino.h"
#include "Constants.h"
#include <cassert>
#include <cstdint>

NOTE_t notes[MAX_NOTES];
int32_t NUM_NOTES_FOUND = 0;

uint32_t parse_uint32_t(uint8_t *buff) {
  return (((uint32_t)buff[0]) << 24) | (((uint32_t)buff[1]) << 16) | (((uint32_t)buff[2]) << 8) | ((uint32_t)buff[3]);
}

void parseNoteFile(uint8_t *noteFile) {
  NUM_NOTES_FOUND = 0;
  uint32_t file_length = parse_uint32_t(&noteFile[0]);
  for (int i = 4; i < file_length; i++) {
    notes[NUM_NOTES_FOUND].startTime = parse_uint32_t(&noteFile[i]);
    i += 4;
    notes[NUM_NOTES_FOUND].fret_idx = (noteFile[i] & 0x0F) % NUM_FRETS;

    // subtract 1 because data received is one-based index
    uint8_t string_idx = ((noteFile[i] & 0xF0) >> 4) - 1;
    notes[NUM_NOTES_FOUND].string = convert_string_idx_to_STRING(string_idx);
    printNote(notes[NUM_NOTES_FOUND]);

    Serial.println();
    assert((0 <= string_idx and string_idx < 4) or string_idx == 0xE);  // string_idx == 0xE for dummy note
    NUM_NOTES_FOUND++;
  }
  NUM_NOTES_FOUND--;  // ignore last dummy note
  Serial.println(notes[0].startTime);
  assert(notes[0].startTime == 0.0);
}

void printNote(const NOTE_t &note) {
  Serial.print("Start time: ");
  Serial.print(note.startTime / 1.0e3, 3);
  Serial.print(" \t Fret: ");
  Serial.print(note.fret_idx);
  if (note.string == E) {
    Serial.print(" \t String: E");
  } else if (note.string == A) {
    Serial.print(" \t String: A");
  } else if (note.string == D) {
    Serial.print(" \t String: D");
  } else if (note.string == G) {
    Serial.print(" \t String: G");
  } else {
    Serial.print(" \t String: None");
  }
}

void printNotes() {
  Serial.print("NUM_NOTES_FOUND: ");
  Serial.println(NUM_NOTES_FOUND);

  for (int i = 0; i < NUM_NOTES_FOUND; i++) {
    Serial.print("Note");
    Serial.print(i);
    Serial.print(": [");
    printNote(notes[i]);
    Serial.println("]");
  }
}