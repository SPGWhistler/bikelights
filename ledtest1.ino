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
  { LOW, LOW, 0, 8 } //?
};
int leftBlinkerButtonLed = A2;
int rightBlinkerButtonLed = A5;
int headLightButtonLed = A1;
int patternToggleButtonLed = A3;
int unused1ButtonLed = A0;
int unused2ButtonLed = A4;

#define mainLedPin 9
#define leftFrontLedPin 10
#define rightFrontLedPin 11
struct stripProps {
  CRGB *strip;
  unsigned int startIndex;
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
unsigned int gfDelay = 50;
unsigned long tlPreviousMillis = 0;
unsigned int tlDelay = 50;
unsigned long slPreviousMillis = 0;
unsigned int slDelay = 50;
unsigned long testPreviousMillis = 0;
unsigned int testDelay = 1000;
unsigned int testStrip = 0;

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
  FastLED.setMaxPowerInMilliWatts(500);
  allOff();
  FastLED.show();
  delay(3000);
}

void doTest() {
  if (currentMillis - testPreviousMillis >= testDelay) {
    off(strips[0]);
    off(strips[1]);
    off(strips[2]);
    off(strips[3]);
    off(strips[4]);
    off(strips[5]);
    off(strips[6]);
    solidColor(strips[testStrip], CRGB::Blue);
    testStrip++;
    if (testStrip > 6) {
      testStrip = 0;
    }
    testPreviousMillis = currentMillis;
  }
}




void loop() {
  currentMillis = millis();
  readButtons();
  doAnimationLoop();
  doHorn();
}
