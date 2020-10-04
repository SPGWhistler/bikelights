
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
/*
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
  fill_rainbow( strip.strip, strip.endIndex, gHue, 7);
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
    strip.strip[ random16(strip.endIndex) ] += CRGB::White;
  }
}

void confetti(stripProps &strip) 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( strip.strip, strip.endIndex, 10);
  int pos = random16(strip.endIndex);
  strip.strip[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon(stripProps &strip)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( strip.strip, strip.endIndex, 20);
  int pos = beatsin16( 13, 0, strip.endIndex - 1 );
  strip.strip[pos] += CHSV( gHue, 255, 192);
}

void bpm(stripProps &strip)
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < strip.endIndex; i++) { //9948
    strip.strip[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle(stripProps &strip) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( strip.strip, strip.endIndex, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    strip.strip[beatsin16( i+7, 0, strip.endIndex - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
*/
