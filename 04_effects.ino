
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
*/
byte hues[8];
unsigned int beatsStep = 0;
void juggle(stripProps &stripProps, unsigned int count, unsigned int bpm, unsigned int fade) {
  fadeOffBy(stripProps, fade);
  for( int i = 0; i < count; i++) {
    if (beatsStep == 0) {
      hues[i] = random8();
    }
    stripProps.strip[beatsin88( (i+bpm)*256, stripProps.offset, (stripProps.offset + stripProps.count) - 1 )] |= CHSV(hues[i], 255, 255);
  }
  beatsStep++;
  if (beatsStep > 100) {
    beatsStep = 0;
  }
}

int tcrJ = 0;
int tcrQ = 0;
void theaterChaseRainbow(stripProps &stripProps) {
  byte *c;
  solidColor(stripProps, CRGB::Black);
  for (int i = stripProps.offset; i < stripProps.count; i=i+3) {
    c = Wheel( (i+tcrJ) % 255);
    if (i + tcrQ < stripProps.count) {
      stripProps.strip[i + tcrQ] = CRGB(*c, *(c+1), *(c+2));
    }
  }
  tcrJ++;
  tcrQ++;
  if (tcrJ > 255) {
    tcrJ = 0;
  }
  if (tcrQ > 2) {
    tcrQ = 0;
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}
