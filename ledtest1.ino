#include <FastLED.h>

struct buttonProps {
  boolean currentState;
  boolean previousState;
  unsigned long previousMillis;
  unsigned int pin;
};
buttonProps buttons[7] = {
  { LOW, LOW, 0, 2 }, //brakes (reed switch)
  { LOW, LOW, 0, 6 }, //leftBlinker (toggle)
  { LOW, LOW, 0, 3 }, //rightBlinker (toggle)
  { LOW, LOW, 0, 4 }, //horn (momentary)
  { LOW, LOW, 0, 5 }, //patternToggle (momentary)
  { LOW, LOW, 0, 7 }, //headlight (toggle)
  { LOW, LOW, 0, 8 } //? temporary brakes
};
int brakesToggleButtonLed = A0;
int headLightButtonLed = A1;
int leftBlinkerButtonLed = A2;
int hornToggleButtonLed = A3;
int patternToggleButtonLed = A4;
int rightBlinkerButtonLed = A5;

#define mainLedPin 9
#define leftFrontLedPin 10
#define rightFrontLedPin 11
struct stripProps {
  CRGB *strip;
  unsigned int offset;
  unsigned int count;
  int countDirection;
  boolean writable;
};
CRGB strip1[92];
CRGB strip2[15];
CRGB strip3[15];
stripProps strips[7] = {
  { strip1, 0, 32, 1, true }, //0 groundEffect
  { strip1, 32, 16, 1, true }, //1 leftSide
  { strip1, 48, 14, -1, true }, //2 leftTail
  { strip1, 62, 14, 1, true }, //3 rightTail
  { strip1, 76, 16, -1, true }, //4 rightSide
  { strip2, 0, 15, 1, true }, //5 leftFront
  { strip3, 0, 15, 1, true } //6 rightFront
};

#define hornPin 12

boolean hornOn = false;
boolean patternOn = false;
boolean updated = false;
int debounceDelay = 50;
unsigned long currentMillis = 0;
boolean leftBlinkerOn = false;
boolean rightBlinkerOn = false;
boolean blink = false;
unsigned long blinkerPreviousMillis = 0;
unsigned int blinkerDelay = 100;
unsigned long hpPreviousMillis = 0;
unsigned int hpDelay = 50;
boolean brakesOn = false;
boolean headLightOn = false;
unsigned long gfPreviousMillis = 0;
unsigned int gfDelay = 100;
unsigned long tlPreviousMillis = 0;
unsigned int tlDelay = 25;
unsigned long slPreviousMillis = 0;
unsigned int slDelay = 25;
unsigned long testPreviousMillis = 0;
unsigned int testDelay = 50;
unsigned int testStep = 0;

void setup() {
  for (int i = 0; i < (sizeof(buttons) / sizeof(buttons[0])); i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  //FastLED.addLeds<SK9822, mainLedPin, 11, BGR, DATA_RATE_MHZ(12)>(strip1, 92);
  FastLED.addLeds<WS2812, mainLedPin, GRB>(strip1, 92);
  FastLED.addLeds<WS2812, leftFrontLedPin, GRB>(strip2, 15);
  FastLED.addLeds<WS2812, rightFrontLedPin, GRB>(strip3, 15);
  //FastLED.setMaxPowerInMilliWatts(500);
  allOff();
  FastLED.show();
  delay(3000);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
/*
typedef void (*SimplePatternList[])(stripProps &strip);
SimplePatternList gPatterns = { off, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
*/


void loop() {
  currentMillis = millis();
  readButtons();
  doAnimationLoop();
  //doHorn();
  //doTest();
}
