

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#include "MIDI.h"
#include "StateMachine.h"

// extern Musical_note_TO_LED_idx[128][2];

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 6  // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 16  // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 50  // Time (in milliseconds) to pause between pixels

// set this to the hardware serial port you wish to use
#define HWSERIAL Serial2

double uS_per_tick;

StateMachine fsm;

void setup() {
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  Serial.begin(9600);
  analogReadResolution(8);

  assert(fsm.getState() == WAIT_TO_START);
}

void loop() {

  fsm.update(true, false, false, false, false);  // TODO TEMPORARY; REMOVE ME

  // //getting the song from the Pi over Serial
  // HWSERIAL.begin(9600);
  // if (HWSERIAL.available() > 0) {
  //   incomingByte = HWSERIAL.read();
  //   Serial.print("UART received: ");
  //   Serial.println((char)incomingByte);
  // }


  assert(fsm.getState() == RECEIVING_SONG);

  fsm.update(true, false, false, false, false);  // TODO TEMPORARY; REMOVE ME

  assert(fsm.getState() == RECEIVING_SONG);



  // parsing the MIDI file
  parseMIDI();
  uS_per_tick = (double)MICROSECONDS_PER_BEAT / (double)TICKS_PER_QUARTER_NOTE;
  Serial.print("uS_per_tick: ");
  Serial.println(uS_per_tick, 10);
  printMIDI();







  pixels.clear();  // Set all pixel colors to 'off'
  pixels.show();   // Send the updated pixel colors to the hardware.
  delay(5000);

  for (int i = 0; i < NUM_NOTES_FOUND; i++) {
    auto us_duration = uS_per_tick * notes[i].duration;
    delayMicroseconds(us_duration);

    auto LED_idx = notes[i].note % NUMPIXELS;
    if (notes[i].ON) {
      Serial.print("Waiting ");
      Serial.print(us_duration);
      Serial.print(" uS before turning LED ");
      Serial.print(LED_idx);
      Serial.print(" ON (note ");
      Serial.print(notes[i].note);
      Serial.println(")");

      pixels.setPixelColor(notes[i].note % NUMPIXELS, pixels.Color(0, 255, 0));
    } else {
      Serial.print("Waiting ");
      Serial.print(us_duration);
      Serial.print(" uS before turning LED ");
      Serial.print(LED_idx);
      Serial.print(" OFF (note ");
      Serial.print(notes[i].note);
      Serial.println(")");

      pixels.setPixelColor(notes[i].note % NUMPIXELS, pixels.Color(0, 0, 0));
    }
    pixels.show();  // Send the updated pixel colors to the hardware.
  }
}
