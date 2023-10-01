#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#include "MIDI.h"

// extern Musical_note_TO_LED_idx[128][2];

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 6  // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 32  // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 50  // Time (in milliseconds) to pause between pixels

void setup() {
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  Serial.begin(9600);
  analogReadResolution(8);

  parseMIDI();



  Serial.print("ticks/beat: ");
  Serial.println(TICKS_PER_QUARTER_NOTE);

  Serial.print("uS/beat: ");
  Serial.println(MICROSECONDS_PER_BEAT);

  Serial.print("NUM_NOTES_FOUND: ");
  Serial.println(NUM_NOTES_FOUND);

  for (int i = 0; i < NUM_NOTES_FOUND; i++) {
    Serial.print("Note");
    Serial.print(i);
    Serial.print(": [");

    Serial.print("Duration: ");
    Serial.print(notes[i].duration);

    if (notes[i].ON) {
      Serial.print("\t ON ");
    } else {
      Serial.print("\t OFF ");
    }

    Serial.print(" \t Note: ");
    Serial.print(notes[i].note);

    Serial.println("]");
  }

  extern NOTE_t notes[MAX_NUM_NOTES];  // TODO hold more notes?
}

void loop() {
  pixels.clear();  // Set all pixel colors to 'off'

  for (int i = 0; i < NUMPIXELS; i++) {  // For each pixel...
    pixels.setPixelColor(i - 3, pixels.Color(0, 0, 0));
    pixels.setPixelColor(i - 2, pixels.Color(5, 0, 0));
    pixels.setPixelColor(i - 1, pixels.Color(0, 5, 0));
    pixels.setPixelColor(i, pixels.Color(0, 0, 5));

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(DELAYVAL);  // Pause before next pass through loop
  }
}
