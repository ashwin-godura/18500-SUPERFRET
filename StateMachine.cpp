#include <cassert>
#include "StateMachine.h"

StateMachine::StateMachine() {
  prevState_ = WAIT_TO_START;
  state_ = WAIT_TO_START;
}

void StateMachine::update(bool file_transmission, bool strum, bool done, bool pause, bool restart) {
  STATE nextState = state_;
  switch (state_) {
    case WAIT_TO_START:
      {
        if (file_transmission) {
          nextState = RECEIVING_SONG;
        }
        break;
      }
    case RECEIVING_SONG:
      {
        if (not file_transmission) {
          nextState = WAIT_FOR_STRUM;
        }
        break;
      }
    case WAIT_FOR_STRUM:
      {
        if (restart) {
          nextState = WAIT_TO_START;
        } else if (strum) {
          nextState = USER_EXPERIENCE;
        }
        break;
      }
    case USER_EXPERIENCE:
      {
        if (restart) {
          nextState = WAIT_TO_START;
        } else if (done) {
          nextState = WAIT_TO_START;
        } else {
          if (pause) {
            nextState = PAUSED;
          }
        }
        break;
      }
    case PAUSED:
      {
        if (restart) {
          nextState = WAIT_TO_START;
        } else {
          if (strum) {
            nextState = USER_EXPERIENCE;
          }
        }
        break;
      }
  }
  prevState_ = state_;
  state_ = nextState;
}
