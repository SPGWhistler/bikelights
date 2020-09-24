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
  doTailLightsAnimation();
  if (updated) {
    FastLED.show();
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
