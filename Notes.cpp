#include "Notes.h"
#include "Arduino.h"
#include "Constants.h"
#include <Adafruit_NeoPixel.h>
#include <cassert>
#include <cstdint>

uint8_t get_LEDidx_from_note(NOTE_t note) {
  // TODO index into a look-up table
  uint8_t LED_idx = 4 * (NUM_FRETS - note.fret_idx - 1) +
                    convert_STRING_to_string_idx(note.string);
  assert(LED_idx < NUMPIXELS);
  return LED_idx;
}

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
