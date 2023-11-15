#include "Notes.h"
#include "Constants.h"
#include <Adafruit_NeoPixel.h>
#include <cstdint>

uint8_t get_LEDidx_from_note(uint8_t note) {
  // TODO index into a look-up table
  return note % NUMPIXELS;
}

uint8_t NoteArray[15][4] = {
    //       E            A             D           G
    0x43, 0x42, 0x41, 0x40, // Fret 0 (open string)
    0x47, 0x46, 0x45, 0x44, // Fret 1
    0x4B, 0x4A, 0x49, 0x48, // Fret 2
    0x00, 0x3E, 0x3D, 0x3C, // Fret 3
    0x00, 0x00, 0x00, 0x05, // Fret 4
    0x00, 0x00, 0x00, 0x00, // Fret 5
    0x00, 0x00, 0x00, 0x00, // Fret 6
    0x00, 0x00, 0x00, 0x00, // Fret 7
    0x00, 0x00, 0x00, 0x00, // Fret 8
    0x00, 0x00, 0x00, 0x00, // Fret 9
    0x00, 0x00, 0x00, 0x00, // Fret 10
    0x00, 0x00, 0x00, 0x00, // Fret 11
    0x00, 0x00, 0x00, 0x00, // Fret 12
    0x00, 0x00, 0x00, 0x00, // Fret 13
    0x00, 0x00, 0x00, 0x00  // Fret 14
};

uint8_t convert_STRING_to_string_idx(STRING string) {
  if (string == E) {
    return 0;
  } else if (string == A) {
    return 1;
  } else if (string == D) {
    return 2;
  } else if (string == G) {
    return 3;
  } else
    return 0;
}

void convertNoteToFretCoordinates(uint8_t note, STRING &string, uint8_t &fret) {
}

uint8_t convertFretCoordinatesToNote(STRING string, uint8_t fret) {
  if (string == None) {
    return 0x00;
  }
  uint8_t string_idx = convert_STRING_to_string_idx(string);
  return NoteArray[fret][string_idx];
}

uint32_t convertFretCoordinatesToCOLOR(STRING string, uint8_t fret) {
  if (string == E) {
    return pixels.Color(0, 255, 0);
  } else if (string == A) {
    return pixels.Color(0, 255, 0);
  } else if (string == D) {
    return pixels.Color(0, 255, 0);
  } else if (string == G) {
    return pixels.Color(0, 255, 0);
  } else
    return pixels.Color(0, 255, 0);
}