#ifndef _CAPSTONE_NOTES_
#define _CAPSTONE_NOTES_

#include <Adafruit_NeoPixel.h>
#include <stdint.h>

enum STRING { E, A, D, G, None };

uint8_t get_LEDidx_from_note(uint8_t note);

uint8_t convert_STRING_to_string_idx(STRING string);
uint8_t convertFretCoordinatesToNote(STRING string, uint8_t fret);
void convertNoteToFretCoordinates(uint8_t note, STRING &string, uint8_t &fret);

extern Adafruit_NeoPixel pixels;
uint32_t convertFretCoordinatesToCOLOR(STRING string, uint8_t fret);

#endif // _CAPSTONE_NOTES_
