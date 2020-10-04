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
  for (int i = stripProps.offset; i < stripProps.offset + stripProps.count; i++) { //TODO add count direction
    stripProps.strip[i] = color;
  } 
}
