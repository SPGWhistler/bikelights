
void doTest() {
  if (currentMillis - testPreviousMillis >= testDelay) {
    allOff();
    if (brakesOn) {
      solidColor(strips[1], CRGB::Red);
      solidColor(strips[2], CRGB::Red);
      solidColor(strips[3], CRGB::Red);
      solidColor(strips[4], CRGB::Red);
    }
    if (leftBlinkerOn) {
      solidColor(strips[1], CRGB::Orange);
      solidColor(strips[2], CRGB::Orange);
    }
    if (rightBlinkerOn) {
      solidColor(strips[3], CRGB::Orange);
      solidColor(strips[4], CRGB::Orange);
    }
    if (headLightOn) {
      solidColor(strips[5], CRGB::White);
      solidColor(strips[6], CRGB::White);
    }
    if (patternOn) {
      solidColor(strips[0], CRGB::Green);
    }
    FastLED.show();
    testPreviousMillis = currentMillis;
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
  doGroundEffectAnimation();
  doTailLightsAnimation();
  doSideLightsAnimation();
  if (updated) {
    FastLED.show();
  }
}
