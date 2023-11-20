#ifndef _CAPSTONE_NOTES_
#define _CAPSTONE_NOTES_

#include <stdint.h>

enum STRING { E, A, D, G, None };

struct NOTE_t {
  uint8_t fret_idx;
  STRING string;
  uint32_t startTime; // [ms]

  bool valid() { return this->string != None; }
  bool operator==(NOTE_t const &rhs) {
    return this->fret_idx == rhs.fret_idx and this->string == rhs.string;
  }
};

STRING convert_string_idx_to_STRING(uint8_t string_idx);
uint8_t get_LEDidx_from_note(NOTE_t note);
uint8_t convert_STRING_to_string_idx(STRING string);
uint32_t convert_Note_To_COLOR(NOTE_t note);
#endif // _CAPSTONE_NOTES_
