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
          case 0: //brakes (not connected)
            brakesOn = true;
            break;
          case 1: //left blinker (button 1, pin 6, A2)
            leftBlinkerOn = true;
            break;
          case 2: //right blinker (button 6, pin 3, A5)
            rightBlinkerOn = true;
            break;
          case 3: //ground effects (button 5, pin 4, A4)
            patternOn = true;
            digitalWrite(patternToggleButtonLed, HIGH);
            break;
          case 4: //horn (button 4, pin 5, A3)
            //nextPattern();
            hornOn = true;
            break;
          case 5: //headlight (button 2, pin 7, A1)
            headLightOn = true;
            digitalWrite(headLightButtonLed, HIGH);
            break;
          case 6: //? (button 3, pin 8, A0)
            brakesOn = true; //temporary
            digitalWrite(brakesToggleButtonLed, HIGH);
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
          case 3: //ground effects
            patternOn = false;
            digitalWrite(patternToggleButtonLed, LOW);
            break;
          case 4: //horn
            hornOn = false;
            break;
          case 5: //headlight
            headLightOn = false;
            digitalWrite(headLightButtonLed, LOW);
            break;
          case 6: //?
            brakesOn = false;
            digitalWrite(brakesToggleButtonLed, LOW);
            break;
        }
      } //else button was in same state for multiple reads, ignore
      buttons[i].previousMillis = currentMillis;
      buttons[i].previousState = buttons[i].currentState;
    }
  }
}
