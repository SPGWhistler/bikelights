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
            //nextPattern();
            patternOn = true;
            break;
          case 5: //headlight
            headLightOn = true;
            digitalWrite(headLightButtonLed, HIGH);
            break;
          case 6: //?
            brakesOn = true;
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
            patternOn = false;
            break;
          case 5: //headlight
            headLightOn = false;
            digitalWrite(headLightButtonLed, LOW);
            break;
          case 6: //?
            brakesOn = false;
            break;
        }
      } //else button was in same state for multiple reads, ignore
      buttons[i].previousMillis = currentMillis;
      buttons[i].previousState = buttons[i].currentState;
    }
  }
}
