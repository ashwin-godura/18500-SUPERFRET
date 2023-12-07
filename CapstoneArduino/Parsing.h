
#include "Arduino.h"
#include "Constants.h"
#include "Notes.h"
#include <cassert>
#include <cstdint>

extern NOTE_t notes[MAX_NOTES];

extern int32_t NUM_NOTES_FOUND;

enum USER_MODE { TRAINING, PERFORMANCE };

typedef struct {
  uint32_t tempo_BPM;
  uint32_t metronome_volume;
  USER_MODE mode;
} NOTE_FILE_METADATA_t;

NOTE_FILE_METADATA_t parseNoteFile(uint8_t *noteFile);

uint32_t parse_uint32_t(uint8_t *buff);
void printNote(const NOTE_t &note);
void printNotes();
