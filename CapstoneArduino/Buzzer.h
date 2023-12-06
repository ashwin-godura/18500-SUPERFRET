#ifndef _CAPSTONE_BUZZER_
#define _CAPSTONE_BUZZRR_

#include <cstdint>

struct BUZZER_t {
  uint8_t buzzer_pin;
  uint32_t buzzer_delay_ms; // [ms] beep period

  uint32_t on_time_ms; // [ms] beep ON-time
  uint8_t buzzer_volume;
};

BUZZER_t start_Buzzer(uint32_t buzzer_delay_ms, uint8_t buzzer_volume,
                      uint32_t on_time_ms);
void run_Buzzer(BUZZER_t buzzer, uint32_t currentTime_ms);
void turn_off_Buzzer(BUZZER_t buzzer);
#endif // _CAPSTONE_BUZZER_
