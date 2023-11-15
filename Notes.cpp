#include "Constants.h"
#include <stdint.h>

uint8_t get_LEDidx_from_note(uint8_t note) {
  // TODO index into a look-up table
  return note % NUMPIXELS;
}

uint8_t convertFretCoordinatesToNote(bool notePlayedOn_E_string,
                                     bool notePlayedOn_A_string,
                                     bool notePlayedOn_D_string,
                                     bool notePlayedOn_G_string, uint8_t fret) {
  if (notePlayedOn_E_string) {
    // Serial.print("Detected press on E string, fret ");
    // Serial.println(fret);
    if (fret == 0) {
      return 0x43; // verified
    } else if (fret == 1) {
      return 0x47;
    } else if (fret == 2) {
      return 0x4B;
    } else if (fret == 3) {
      return 0x00;
    } else if (fret == 4) {
      return 0x00;
    }
  } else if (notePlayedOn_A_string) {
    // Serial.print("Detected press on A string, fret ");
    // Serial.println(fret);
    if (fret == 0) {
      return 0x42;
    } else if (fret == 1) {
      return 0x46;
    } else if (fret == 2) {
      return 0x4A;
    } else if (fret == 3) {
      return 0x3E;
    } else if (fret == 4) {
      return 0x00;
    }
  } else if (notePlayedOn_D_string) {
    // Serial.print("Detected press on D string, fret ");
    // Serial.println(fret);
    if (fret == 0) {
      return 0x41; // verified
    } else if (fret == 1) {
      return 0x45;
    } else if (fret == 2) {
      return 0x49;
    } else if (fret == 3) {
      return 0x3D;
    } else if (fret == 4) {
      return 0x00;
    }
  } else if (notePlayedOn_G_string) {
    // Serial.print("Detected press on G string, fret ");
    // Serial.println(fret);
    if (fret == 0) {
      return 0x40;
    } else if (fret == 1) {
      return 0x44;
    } else if (fret == 2) {
      return 0x48;
    } else if (fret == 3) {
      return 0x3C; // verified
    } else if (fret == 4) {
      return 0x00;
    }
  } else {
    return 0x0;
  }
}