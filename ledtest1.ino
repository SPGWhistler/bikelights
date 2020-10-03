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
typedef void (*SimplePatternList[])(stripProps &strip);
SimplePatternList gPatterns = { off, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


void loop() {
  currentMillis = millis();
  readButtons();
  doAnimationLoop();
  doHorn();
}

void doHorn() {
  if (hornOn) {
    tone(hornPin, 1000);
  } else {
    noTone(hornPin);
  }
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
            break;
          case 1: //left blinker
            leftBlinkerOn = true;
            break;
          case 2: //right blinker
            rightBlinkerOn = true;
            break;
          case 3: //horn
            hornOn = true;
            break;
          case 4: //toggle pattern
            nextPattern();
            break;
          case 5: //headlight
            headLightOn = true;
            digitalWrite(headLightButtonLed, HIGH);
            break;
          case 6: //?
            break;
        }
      } else if (buttons[i].currentState == HIGH && buttons[i].previousState == LOW) {
        //Button is not pressed and was pressed before
        switch (i) {
          case 0: //brakes
            brakesOn = false;
            break;
          case 1: //left blinker
            leftBlinkerOn = false;
            digitalWrite(leftBlinkerButtonLed, LOW);
            break;
          case 2: //right blinker
            rightBlinkerOn = false;
            digitalWrite(rightBlinkerButtonLed, LOW);
            break;
          case 3: //horn
            hornOn = false;
            break;
          case 4: //toggle pattern
            break;
          case 5: //headlight
            headLightOn = false;
            digitalWrite(headLightButtonLed, LOW);
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
    //doBrakesAnimation();
    doBlinkerAnimation();
    hpPreviousMillis = currentMillis;
  }
  /*
  Then call animation functions.
  These must check each strip's locked state before writing to it.
  These must obey that lock.
  */
  //doGroundEffectAnimation();
  doTailLightsAnimation();
  //doSideLightsAnimation();
  if (updated) {
    FastLED.show();
  }
}

void doGroundEffectAnimation() {
  if (currentMillis - gfPreviousMillis >= gfDelay && strips[0].writable) {
    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber](strips[0]);
    if (gCurrentPatternNumber == 0) {
      digitalWrite(patternToggleButtonLed, LOW);
    } else {
      digitalWrite(patternToggleButtonLed, HIGH);
    }
    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    //EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
    gfPreviousMillis = currentMillis;
  }
}

void doTailLightsAnimation() {
  if (currentMillis - tlPreviousMillis >= tlDelay) {
    if (strips[2].writable && strips[3].writable) {
      //We can do animation...
      fadeToBlackBy( strips[2].strip, strips[2].count, 40);
      int pos = beatsin16( 13, strips[2].startIndex, strips[2].startIndex + strips[2].count - 1);
      strips[2].strip[pos] += CRGB::Red;
      fadeToBlackBy( strips[3].strip, strips[3].count, 40);
      int pos2 = beatsin16( 13, strips[3].startIndex, strips[3].startIndex + strips[3].count - 1 );
      strips[3].strip[pos2] += CRGB::Red;
    } else {
      //If we cant do animation, black out any strips that are still writable.
      if (strips[2].writable) {
        solidColor(strips[2], CRGB::Black);
      } else if (strips[3].writable) {
        solidColor(strips[3], CRGB::Black);
      }
    }
    tlPreviousMillis = currentMillis;
  }
}

void doSideLightsAnimation() {
  if (currentMillis - slPreviousMillis >= slDelay) {
    if (strips[1].writable && strips[4].writable) {
      //We can do animation...
      gPatterns[gCurrentPatternNumber](strips[1]);
      gPatterns[gCurrentPatternNumber](strips[4]);
    } else {
      //If we cant do animation, black out any strips that are still writable.
      if (strips[1].writable) {
        solidColor(strips[1], CRGB::Black);
      } else if (strips[4].writable) {
        solidColor(strips[4], CRGB::Black);
      }
    }
    slPreviousMillis = currentMillis;
  }
}

void doHeadLightAnimation() {
  if (headLightOn == true) {
    //Turn headlight on
    solidColor(strips[5], CRGB::White);
    solidColor(strips[6], CRGB::White);
    strips[5].writable = false;
    strips[6].writable = false;
  } else if (headLightOn == false && strips[5].writable == false) {
    //Turn headlight off (should only execute one time)
    solidColor(strips[5], CRGB::Black);
    solidColor(strips[6], CRGB::Black);
    strips[5].writable = true;
    strips[6].writable = true;
  }
}

//Set the back leds to all red
void doBrakesAnimation() {
  if (brakesOn == true) {
    //Turn brakes on
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
    //TODO This seems like a bug - if the blinker sets writable to false, we'd enter this condition...
    //Turn brakes off
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
        solidColor(strips[1], CRGB::Red);
        solidColor(strips[2], CRGB::Red);
        digitalWrite(leftBlinkerButtonLed, HIGH);
      } else {
        solidColor(strips[1], CRGB::Black);
        solidColor(strips[2], CRGB::Black);
        digitalWrite(leftBlinkerButtonLed, LOW);
      }
    } else if (leftBlinkerOn == false && strips[1].writable == false && brakesOn == false) {
      solidColor(strips[1], CRGB::Black);
      solidColor(strips[2], CRGB::Black);
      strips[1].writable = true;
      strips[2].writable = true;
      digitalWrite(leftBlinkerButtonLed, LOW);
    }
    if (rightBlinkerOn == true) {
      strips[3].writable = false;
      strips[4].writable = false;
      if (blink) {
        solidColor(strips[3], CRGB::Red);
        solidColor(strips[4], CRGB::Red);
        digitalWrite(rightBlinkerButtonLed, HIGH);
      } else {
        solidColor(strips[3], CRGB::Black);
        solidColor(strips[4], CRGB::Black);
        digitalWrite(rightBlinkerButtonLed, LOW);
      }
    } else if (rightBlinkerOn == false && strips[3].writable == false && brakesOn == false) {
      solidColor(strips[3], CRGB::Black);
      solidColor(strips[4], CRGB::Black);
      strips[3].writable = true;
      strips[4].writable = true;
      digitalWrite(rightBlinkerButtonLed, LOW);
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
  for (int i = stripProps.startIndex; i < stripProps.startIndex + stripProps.count - 1; i++) { //TODO add count direction
    stripProps.strip[i] = color;
  } 
}



#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void off(stripProps &strip)
{
  solidColor(strip, CRGB::Black);
}

void rainbow(stripProps &strip) 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( strip.strip, strip.count, gHue, 7);
}

void rainbowWithGlitter(stripProps &strip) 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow(strip);
  addGlitter(strip, 80);
}

void addGlitter( stripProps &strip, fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    strip.strip[ random16(strip.startIndex, strip.startIndex + strip.count - 1) ] += CRGB::White;
  }
}

void confetti(stripProps &strip) 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( strip.strip, strip.count, 10);
  int pos = random16(strip.startIndex, strip.startIndex + strip.count - 1);
  strip.strip[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon(stripProps &strip)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( strip.strip, strip.count, 20);
  int pos = beatsin16( 13, strip.startIndex, strip.startIndex + strip.count - 1 );
  strip.strip[pos] += CHSV( gHue, 255, 192);
}

void bpm(stripProps &strip)
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < strip.startIndex + strip.count - 1; i++) { //9948
    strip.strip[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle(stripProps &strip) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( strip.strip, strip.count, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    strip.strip[beatsin16( i+7, strip.startIndex, strip.startIndex + strip.count - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
