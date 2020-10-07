void doGroundEffectAnimation() {
  if (currentMillis - gfPreviousMillis >= gfDelay) {
    if (strips[0].writable && patternOn == true) {
      //juggle(strips[0], 6, 10, 150);
      theaterChaseRainbow(strips[0]);
    }
    if (strips[0].writable && patternOn == false) {
      solidColor(strips[0], CRGB::Black);
    }
    gfPreviousMillis = currentMillis;
  }
}

void doTailLightsAnimation() {
  if (currentMillis - tlPreviousMillis >= tlDelay) {
    if (strips[2].writable && strips[3].writable) {
      //We can do animation...
      fadeOffBy(strips[2], 70);
      int pos = beatsin16( 30, strips[2].offset, (strips[2].offset + strips[2].count) - 1 );
      strips[2].strip[pos] += CRGB::Red;
      fadeOffBy(strips[3], 70);
      pos = beatsin16( 30, strips[3].offset, (strips[3].offset + strips[3].count) - 1 );
      strips[3].strip[pos] += CRGB::Red;
    } else {
      //If we cant do animation, black out any strips that are still writable.
      if (strips[2].writable) {
        solidColor(strips[2], CRGB::Black);
      }
      if (strips[3].writable) {
        solidColor(strips[3], CRGB::Black);
      }
    }
    tlPreviousMillis = currentMillis;
  }
}

void doSideLightsAnimation() {
  if (currentMillis - slPreviousMillis >= slDelay) {
    if (strips[1].writable && strips[4].writable && patternOn == true) {
      //We can do animation...
      juggle(strips[1], 4, 30, 150);
      juggle(strips[4], 4, 30, 150);
    } else {
      //If we cant do animation, black out any strips that are still writable.
      if (strips[1].writable) {
        solidColor(strips[1], CRGB::Black);
      }
      if (strips[4].writable) {
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
