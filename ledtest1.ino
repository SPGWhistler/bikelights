#include <FastLED.h>

struct buttonProps {
  boolean currentState;
  boolean previousState;
  unsigned long previousMillis;
  unsigned int pin;
};
buttonProps buttons[7] = {
  { LOW, LOW, 0, 3 }, //brakes (reed switch)
  { LOW, LOW, 0, 4 }, //leftBlinker (toggle)
  { LOW, LOW, 0, 5 }, //rightBlinker (toggle)
  { LOW, LOW, 0, 6 }, //horn (momentary)
  { LOW, LOW, 0, 7 }, //patternToggle (momentary)
  { LOW, LOW, 0, 8 }, //headlight (toggle)
  { LOW, LOW, 0, 9 } //?
};

#define mainLedPin 10
#define leftFrontLedPin 11
#define rightFrontLedPin 12
struct stripProps {
  CRGB *strip;
  unsigned int startIndex;
  unsigned int endIndex;
  int countDirection;
  boolean writable;
};
CRGB strip1[92];
CRGB strip2[15];
CRGB strip3[15];
stripProps strips[7] = {
  { strip1, 0, 31, 1, true }, //0 groundEffect
  { strip1, 32, 47, 1, true }, //1 leftSide
  { strip1, 48, 61, -1, true }, //2 leftTail
  { strip1, 62, 75, 1, true }, //3 rightTail
  { strip1, 76, 91, -1, true }, //4 rightSide
  { strip2, 0, 14, 1, true }, //5 leftFront
  { strip3, 0, 14, 1, true } //6 rightFront
};

boolean updated = false;
int debounceDelay = 50;
unsigned long currentMillis = 0;
boolean leftBlinkerOn = false;
boolean rightBlinkerOn = false;
boolean blink = false;
unsigned long blinkerPreviousMillis = 0;
unsigned int blinkerDelay = 250;
unsigned long hpPreviousMillis = 0;
unsigned int hpDelay = 50;
boolean brakesOn = false;
boolean headLightOn = false;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < (sizeof(buttons) / sizeof(buttons[0])); i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
  FastLED.addLeds<SK9822, mainLedPin, 11, BGR, DATA_RATE_MHZ(12)>(strip1, 92);
  /* TODO change when using on bike
  FastLED.addLeds<WS2812, mainLedPin, GRB>(strip1, 92);
  FastLED.addLeds<WS2812, leftFrontLedPin, GRB>(strip2, 15);
  FastLED.addLeds<WS2812, rightFrontLedPin, GRB>(strip3, 15);
  */
  FastLED.setMaxPowerInMilliWatts(500);
  allOff();
  FastLED.show();
  Serial.println("Ready.");
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


void loop() {
  currentMillis = millis();
  readButtons();
  doAnimationLoop();
}

void readButtons() {
  for (int i = 0; i < (sizeof(buttons) / sizeof(buttons[0])); i++) {
    if (currentMillis - buttons[i].previousMillis >= debounceDelay) {
      //Debounce time has passed, check state
      buttons[i].currentState = digitalRead(buttons[i].pin);
      if (buttons[i].currentState == LOW && buttons[i].previousState == HIGH) {
        //Button is pressed and was not pressed before
        switch (i) {
          case 0: //brakes
            brakesOn = true;
            Serial.println("turning brakes on...");
            break;
          case 1: //left blinker
            leftBlinkerOn = true;
            Serial.println("turning left blinker on...");
            break;
          case 2: //right blinker
            rightBlinkerOn = true;
            Serial.println("turning right blinker on...");
            break;
          case 3: //horn
            break;
          case 4: //toggle pattern
            break;
          case 5: //headlight
            headLightOn = true;
            Serial.println("turning headlight on...");
            break;
          case 6: //?
            break;
        }
      } else if (buttons[i].currentState == HIGH && buttons[i].previousState == LOW) {
        //Button is not pressed and was pressed before
        switch (i) {
          case 0: //brakes
            brakesOn = false;
            Serial.println("turning brakes off...");
            break;
          case 1: //left blinker
            leftBlinkerOn = false;
            Serial.println("turning left blinker off...");
            break;
          case 2: //right blinker
            rightBlinkerOn = false;
            Serial.println("turning right blinker off...");
            break;
          case 3: //horn
            break;
          case 4: //toggle pattern
            break;
          case 5: //headlight
            headLightOn = false;
            Serial.println("turning headlight off...");
            break;
          case 6: //?
            break;
        }
      } //else button was in same state for multiple reads, ignore
      buttons[i].previousMillis = currentMillis;
      buttons[i].previousState = buttons[i].currentState;
    }
  }
}

void doAnimationLoop() {
  updated = false;
  /*
  First, do high priority functions, in order of least to highest:
  Any strips written to by these functions should be locked.
  But, these functions must not obey that lock.
  */
  if (currentMillis - hpPreviousMillis >= hpDelay) {
    updated = true;
    doHeadLightAnimation();
    doBrakesAnimation();
    doBlinkerAnimation();
    hpPreviousMillis = currentMillis;
  }
  /*
  Then call animation functions.
  These must check each strip's locked state before writing to it.
  These must obey that lock.
  */
  doGroundEffectAnimation();
  doTailLightsAnimation();
  if (updated) {
    FastLED.show();
  }
}

void doGroundEffectAnimation() {
  if (strips[0].writable) {
    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();
    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  }
}

void doTailLightsAnimation() {
  if (strips[2].writable == true) {
    //Example for only writing to a strip if its not locked
  }
}

//TODO Change back to 5 when on bike
void doHeadLightAnimation() {
  if (headLightOn == true) {
    Serial.println("headlight on");
    //Turn headlight on
    solidColor(strips[0], CRGB::White);
    solidColor(strips[6], CRGB::White);
    strips[0].writable = false;
    strips[6].writable = false;
  } else if (headLightOn == false && strips[0].writable == false) {
    Serial.println("headlight off");
    //Turn headlight off (should only execute one time)
    solidColor(strips[0], CRGB::Black);
    solidColor(strips[6], CRGB::Black);
    strips[0].writable = true;
    strips[6].writable = true;
  }
}

//Set the back leds to all red
void doBrakesAnimation() {
  if (brakesOn == true) {
    //Turn brakes on
    Serial.println("brakes on");
    if (leftBlinkerOn == false) {
      solidColor(strips[1], CRGB::Red);
      solidColor(strips[2], CRGB::Red);
      strips[1].writable = false;
      strips[2].writable = false;
    }
    if (rightBlinkerOn == false) {
      solidColor(strips[3], CRGB::Red);
      solidColor(strips[4], CRGB::Red);
      strips[3].writable = false;
      strips[4].writable = false;
    }
  } else if (brakesOn == false && strips[1].writable == false) {
    //Turn brakes off
    Serial.println("brakes off");
    if (leftBlinkerOn == false) {
      solidColor(strips[1], CRGB::Black);
      solidColor(strips[2], CRGB::Black);
      strips[1].writable = true;
      strips[2].writable = true;
    }
    if (rightBlinkerOn == false) {
      solidColor(strips[3], CRGB::Black);
      solidColor(strips[4], CRGB::Black);
      strips[3].writable = true;
      strips[4].writable = true;
    }
  }
}

void doBlinkerAnimation() {
  if (currentMillis - blinkerPreviousMillis >= blinkerDelay) {
    if (leftBlinkerOn == true) {
      strips[1].writable = false;
      strips[2].writable = false;
      if (blink) {
        Serial.println("left blinker on");
        solidColor(strips[1], CRGB::Red);
        solidColor(strips[2], CRGB::Red);
      } else {
        Serial.println("left blinker off");
        solidColor(strips[1], CRGB::Black);
        solidColor(strips[2], CRGB::Black);
      }
    } else if (leftBlinkerOn == false && strips[1].writable == false && brakesOn == false) {
      solidColor(strips[1], CRGB::Black);
      solidColor(strips[2], CRGB::Black);
      strips[1].writable = true;
      strips[2].writable = true;
    }
    if (rightBlinkerOn == true) {
      strips[3].writable = false;
      strips[4].writable = false;
      if (blink) {
        Serial.println("right blinker on");
        solidColor(strips[3], CRGB::Red);
        solidColor(strips[4], CRGB::Red);
      } else {
        Serial.println("right blinker off");
        solidColor(strips[3], CRGB::Black);
        solidColor(strips[4], CRGB::Black);
      }
    } else if (rightBlinkerOn == false && strips[3].writable == false && brakesOn == false) {
      solidColor(strips[3], CRGB::Black);
      solidColor(strips[4], CRGB::Black);
      strips[3].writable = true;
      strips[4].writable = true;
    }
    blink = !blink;
    blinkerPreviousMillis = currentMillis;
  }
}

/*
void fadeOff(int strip[]) {
  int colorArray = strip[0];
  int ledStart = strip[1];
  int ledStop = strip[2];
  for (int i = ledStart; i <= ledStop; i++) {
    colors[colorArray][i].fadeToBlackBy(10);
  }
  FastLED.show();
}

void fadeAllOff() {
  for (int i = 0; i < (sizeof(strips) / sizeof(strips[0])); i++) {
    int colorArray = strips[i][0];
    int ledStart = strips[i][1];
    int ledStop = strips[i][2];
    for (int j = ledStart; j <= ledStop; j++) {
      colors[colorArray][j].fadeToBlackBy(10);
    }
  }
  FastLED.show();
}
*/

//Set all leds to black
void allOff() {
  for (int i = 0; i < (sizeof(strips) / sizeof(strips[0])); i++) {
    solidColor(strips[i], CRGB::Black);
  }
}

//Given an led strip, set all of its pixels to the given color
void solidColor(stripProps &stripProps, CRGB color) {
  for (int i = stripProps.startIndex; i < stripProps.endIndex; i++) { //TODO add count direction
    stripProps.strip[i] = color;
  } 
}



#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( strips[0].strip, strips[0].endIndex, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    strips[0].strip[ random16(strips[0].endIndex) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( strips[0].strip, strips[0].endIndex, 10);
  int pos = random16(strips[0].endIndex);
  strips[0].strip[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( strips[0].strip, strips[0].endIndex, 20);
  int pos = beatsin16( 13, 0, strips[0].endIndex - 1 );
  strips[0].strip[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < strips[0].endIndex; i++) { //9948
    strips[0].strip[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( strips[0].strip, strips[0].endIndex, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    strips[0].strip[beatsin16( i+7, 0, strips[0].endIndex - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
