#include "Buzzer.h"
#include "Arduino.h"
#include "Constants.h"
#include "PINS.h"

BUZZER_t start_Buzzer(uint32_t buzzer_delay, uint8_t buzzer_volume,
                      uint32_t on_time_ms) {
  BUZZER_t buzzer;
  buzzer.buzzer_delay_ms = buzzer_delay;
  buzzer.buzzer_volume = buzzer_volume;
  buzzer.buzzer_pin = BUZZER_PIN;
  buzzer.on_time_ms = on_time_ms;
  return buzzer;
}

void run_Buzzer(BUZZER_t buzzer, uint32_t time_since_start_ms) {
  uint32_t time_since_last_beat_ms =
    (time_since_start_ms - BUZZER_TIME_OFFSET_ms) % buzzer.buzzer_delay_ms;
  if (time_since_last_beat_ms < buzzer.on_time_ms) {
    analogWrite(buzzer.buzzer_pin, buzzer.buzzer_volume);
  } else {
    analogWrite(buzzer.buzzer_pin, 0);
  }
}

void turn_off_Buzzer(BUZZER_t buzzer) {
  analogWrite(buzzer.buzzer_pin, 0);
}

uint32_t map_metronome_volume(uint32_t metronome_volume) {
  if (metronome_volume == 0) {
    return 0; //Ensures the volume is 0 when the user doesn't want volume
  } else {
    return map(metronome_volume, 1, 10, 50, 255); //60 is around the minimum audible, 255 is max
  }
}
