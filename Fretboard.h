#ifndef _CAPSTONE_FRETBOARD_
#define _CAPSTONE_FRETBOARD_

#include "StateMachine.h"
#include <cstdint>

void clearShiftRegister();
void loadShiftRegister();

extern uint8_t notePlayed;
void sampleFrets();

extern StateMachine fsm;
extern bool strum;
void samplePick();

#endif // _CAPSTONE_FRETBOARD_
