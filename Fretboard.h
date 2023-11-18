#ifndef _CAPSTONE_FRETBOARD_
#define _CAPSTONE_FRETBOARD_

#include "Notes.h"
#include "StateMachine.h"
#include <cstdint>

void clearShiftRegister();
void loadShiftRegister();

extern NOTE_t notePlayed;
void sampleFrets();

extern StateMachine fsm;
extern bool strum;
extern STRING stringStrummed;
void samplePick();

#endif  // _CAPSTONE_FRETBOARD_
