#ifndef _CAPSTONE_NOTES_
#define _CAPSTONE_NOTES_

#include <stdint.h>

uint8_t get_LEDidx_from_note(uint8_t note);

uint8_t convertFretCoordinatesToNote(bool notePlayedOn_E_string,
                                     bool notePlayedOn_A_string,
                                     bool notePlayedOn_D_string,
                                     bool notePlayedOn_G_string, uint8_t fret);
#endif // _CAPSTONE_NOTES_
