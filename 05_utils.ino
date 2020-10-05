void fadeOffBy(stripProps &stripProps, int by) {
  for (int i = stripProps.offset; i < stripProps.offset + stripProps.count; i++) {
    stripProps.strip[i].fadeToBlackBy(by);
  }
}

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
