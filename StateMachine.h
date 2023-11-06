enum STATE {
  WAIT_TO_START,
  RECEIVING_SONG,
  WAIT_FOR_STRUM,
  USER_EXPERIENCE,
  PAUSED
};

class StateMachine {

public:
  StateMachine();

  void update(bool file_transmission, bool strum, bool done, bool pause,
              bool restart);

  STATE getState() {
    return state_;
  }
  STATE getPrevState() {
    return prevState_;
  }
  void setState(STATE newState) {
    state_ = newState;
  }

private:
  STATE prevState_, state_;
};