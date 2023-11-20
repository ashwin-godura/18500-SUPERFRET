
#include "Arduino.h"
#include "Constants.h"
#include "Notes.h"
#include <cassert>
#include <cstdint>

extern NOTE_t notes[MAX_NOTES];

extern uint32_t NUM_NOTES_FOUND;

void parseNoteFile(uint8_t *noteFile);

void printNote(const NOTE_t &note);
void printNotes();