#ifndef _CAPSTONE_BUZZER_
#define _CAPSTONE_BUZZRR_

#include <cstdint>

struct BUZZER_t {
  uint8_t buzzer_pin;
  uint32_t buzzer_delay;

  uint32_t on_time;
  uint8_t buzzer_volume;
};

BUZZER_t start_Buzzer(uint32_t buzzer_delay, uint8_t buzzer_volume, uint32_t on_time);
void run_Buzzer(BUZZER_t buzzer, uint32_t currentTime);
void turn_off_Buzzer(BUZZER_t buzzer);
#endif // _CAPSTONE_BUZZER_
