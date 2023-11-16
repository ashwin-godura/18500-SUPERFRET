#include "Notes.h"
#include "Arduino.h"
#include "Constants.h"
#include <Adafruit_NeoPixel.h>
#include <cstdint>

uint8_t get_LEDidx_from_note(NOTE_t note) {
  // TODO index into a look-up table
  return (4 * note.fret_idx + convert_STRING_to_string_idx(note.string)) %
         NUMPIXELS;
}

uint8_t NoteArray[NUM_FRETS][4] = {
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

STRING convert_string_idx_to_STRING(uint8_t string_idx) {
  if (string_idx == 0) {
    return E;
  } else if (string_idx == 1) {
    return A;
  } else if (string_idx == 2) {
    return D;
  } else if (string_idx == 3) {
    return G;
  }
}

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

uint32_t convert_Note_To_COLOR(NOTE_t note) {
  static Adafruit_NeoPixel pixels;
  if (note.string == E) {
    return pixels.Color(255, 0, 0); // RED
  } else if (note.string == A) {
    return pixels.Color(0, 0, 255); // BLUE
  } else if (note.string == D) {
    return pixels.Color(0, 255, 0); // GREEN
  } else if (note.string == G) {
    return pixels.Color(255, 100, 0); // YELLOW
  } else
    return pixels.Color(0, 0, 0);
}
