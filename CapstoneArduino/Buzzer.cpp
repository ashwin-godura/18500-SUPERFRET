#include "Buzzer.h"
#include "Arduino.h"
#include "Constants.h"
#include "PINS.h"

BUZZER_t start_Buzzer(uint32_t buzzer_delay, uint8_t buzzer_volume, uint32_t on_time) {
  BUZZER_t buzzer;
  buzzer.buzzer_delay = buzzer_delay;
  buzzer.buzzer_volume = buzzer_volume;
  buzzer.buzzer_pin = BUZZER_PIN;
  buzzer.on_time = on_time;
  return buzzer;
}

void run_Buzzer(BUZZER_t buzzer, uint32_t time_since_start){
  uint32_t time_since_last_beat = (time_since_start - BUZZER_TIME_OFFSET) % buzzer.buzzer_delay;
  if (time_since_last_beat < buzzer.on_time) {
    analogWrite(buzzer.buzzer_pin, buzzer.buzzer_volume);
  } else {
    analogWrite(buzzer.buzzer_pin, 0);
  }
}

void turn_off_Buzzer(BUZZER_t buzzer) {
  analogWrite(buzzer.buzzer_pin, 0);
}
