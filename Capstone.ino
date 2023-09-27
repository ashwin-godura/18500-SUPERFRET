#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif


// uint8_t MIDI[422] = { 00, C0, 00, 00, 90, 3C, 32, 81, 40, 80, 3C, 00, 81, 40, 90, 3C, 32, 81, 40, 80, 3C, 00, 81, 40, 90, 43, 3, 81, 40, 80, 43, 00, 81, 40, 90, 43, 32, 81, 40, 80, 43, 00, 81, 40, 90, 45, 32, 81, 40, 80, 45, 00, 81, 40, 90, 45, 32, 81, 40, 80, 45, 00, 81, 40, 90, 43, 32, 83, 00, 80, 43, 00, 83, 00, 90, 41, 32, 81, 40, 80, 41, 00, 81, 40, 90, 41, 32, 81, 40, 80, 41, 00, 81, 40, 90, 40, 32, 81, 40, 80, 40, 00, 81, 40, 90, 40, 32, 81, 40, 80, 40, 00, 81, 40, 90, 3E, 32, 81, 40, 80, 3E, 00, 81, 40, 90, 3E, 32, 81, 40, 80, 3E, 00, 81, 40, 90, 3C, 32, 83, 00, 80, 3C, 00, 83, 00, 90, 43, 32, 81, 40, 80, 43, 00, 81, 40, 90, 43, 32, 81, 40, 80, 43, 00, 81, 40, 90, 41, 32, 81, 40, 80, 41, 00, 81, 40, 90, 41, 32, 81, 40, 80, 41, 00, 81, 40, 90, 40, 32, 81, 40, 80, 40, 00, 81, 40, 90, 40, 32, 81, 40, 80, 40, 00, 81, 40, 90, 3E, 32, 83, 00, 80, 3E, 00, 83, 00, 90, 43, 32, 81, 40, 80, 43, 00, 81, 40, 90, 43, 32, 81, 40, 80, 43, 00, 81, 40, 90, 41, 32, 81, 40, 80, 41, 00, 81, 40, 90, 41, 32, 81, 40, 80, 41, 00, 81, 40, 90, 40, 32, 81, 40, 80, 40, 00, 81, 40, 90, 40, 32, 81, 40, 80, 40, 00, 81, 40, 90, 3E, 32, 83, 00, 80, 3E, 00, 83, 00, 90, 3C, 32, 81, 40, 80, 3C, 00, 81, 40, 90, 3C, 32, 81, 40, 80, 3C, 00, 81, 40, 90, 43, 32, 81, 40, 80, 43, 00, 81, 40, 90, 43, 32, 81, 40, 80, 43, 00, 81, 40, 90, 45, 32, 81, 40, 80, 45, 00, 81, 40, 90, 45, 32, 81, 40, 80, 45, 00, 81, 40, 90, 43, 32, 83, 00, 80, 43, 00, 83, 00, 90, 41, 32, 81, 40, 80, 41, 00, 81, 40, 90, 41, 32, 81, 40, 80, 41, 00, 81, 40, 90, 40, 32, 81, 40, 80, 40, 00, 81, 40, 90, 40, 32, 81, 40, 80, 40, 00, 81, 40, 90, 3E, 32, 81, 40, 80, 3E, 00, 81, 40, 90, 3E, 32, 81, 40, 80, 3E, 00, 81, 40, 90, 3C, 32, 83, 00, 80, 3C, 00, 00 };


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
}

long num_reads = 1'000'0;
void loop() {

  auto start = micros();
  for(int i=0; i<num_reads; i++){
    analogRead(5);
  }
  auto end = micros();
  Serial.println(end-start);

  // pixels.clear();  // Set all pixel colors to 'off'

  // for (int i = 0; i < NUMPIXELS; i++) {  // For each pixel...
  //   pixels.setPixelColor(i - 3, pixels.Color(0, 0, 0));
  //   pixels.setPixelColor(i - 2, pixels.Color(5, 0, 0));
  //   pixels.setPixelColor(i - 1, pixels.Color(0, 5, 0));
  //   pixels.setPixelColor(i, pixels.Color(0, 0, 5));

  //   pixels.show();  // Send the updated pixel colors to the hardware.

  //   delay(DELAYVAL);  // Pause before next pass through loop
  // }
}
