
//https://www.pjrc.com/teensy/td_joystick.html

////////////////////////////////////////////////////
#define DEBUG 1
#define BUTTON_COUNT 23

typedef struct {
  int pin;
  int buttonID;
  char state;
  char oldState;
} Button;

//{teensy Pin Number, joystick number, pin state}
Button buttons[BUTTON_COUNT] = {
  { 0, 1, 0 },  //{teensy Pin Number, joystick number, pin state}
  { 1, 2, 0 },
  { 2, 3, 0 },
  { 3, 4, 0 },
  { 4, 5, 0 },
  { 5, 6, 0 },
  { 6, 7, 0 },
  { 7, 8, 0 },
  { 8, 9, 0 },
  { 9, 10, 0 },
  { 10, 11, 0 },
  { 11, 12, 0 },
  { 12, 13, 0 },
  { 13, 14, 0 },
  { 14, 15, 0 },
  { 15, 16, 0 },
  { 16, 17, 0 },
  { 17, 18, 0 },
  { 18, 19, 0 },
  { 19, 20, 0 },
  { 20, 21, 0 },
  { 21, 22, 0 },
  { 22, 23, 0 },
  { 23, 24, 0 },
};

void InitButtons(void) {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }

  delay(10);

  for (int i = 0; i < BUTTON_COUNT; i++) {
    if (digitalRead(buttons[i].pin)) {  //active low
      buttons[i].oldState = 0;
    } else {
      buttons[i].oldState = 1;
    }
  }
}

void UpdateButtons(void) {
  for (int i = 0; i < BUTTON_COUNT; i++) {
    if (digitalRead(buttons[i].pin)) {  //active low
      buttons[i].state = 0;
    } else {
      buttons[i].state = 1;
    }

#if (DEBUG)
    if (buttons[i].oldState != buttons[i].state) {
      Serial.print("Button ");
      Serial.print(buttons[i].buttonID);

      if (buttons[i].state)
        Serial.println(" Released");
      else
        Serial.println(" Pressed");
    }
#endif

    Joystick.button(buttons[i].buttonID, buttons[i].state);
    buttons[i].oldState = buttons[i].state;
  }
}
////////////////////////////////////////////////////

typedef struct {
  int clkPin;
  int dirPin;
  int counts;
  char oldState;
} Encoder;

//{clk pin number, dir pin number, encoder count, old clk state}
Encoder encoders[] = {
  { 24, 25, 0, 0 },  //z axis
  { 26, 27, 0, 0 },  //left slider
  { 28, 29, 0, 0 },  //right slider
};

void InitEncoders(void) {
  for (int i = 0; i < 3; i++) {
    pinMode(encoders[i].dirPin, INPUT);
    pinMode(encoders[i].clkPin, INPUT);
  }

  delay(10);

  for (int i = 0; i < 3; i++) {
    encoders[i].oldState = digitalRead(encoders[i].clkPin);
  }
}

void UpdateEncoders(void) {
  char tempState = 0;

  for (int i = 0; i < 3; i++) {
    tempState = digitalRead(encoders[i].clkPin);

    if (tempState != encoders[i].oldState) {
      if (digitalRead(encoders[i].dirPin)) {
        encoders[i].counts++;
      } else {
        encoders[i].counts--;
      }

      if (encoders[i].counts > 1023)
        encoders[i].counts = 0;

      if (encoders[i].counts < 0)
        encoders[i].counts = 1023;

#if (DEBUG)
      Serial.print("Encoder ");
      Serial.print(i);
      Serial.print(" Changed, count: ");
      Serial.println(encoders[i].counts);
#endif
    }
  }

  Joystick.Zrotate(encoders[0].counts);  //[0,1023] 512 = center
  Joystick.sliderLeft(encoders[1].counts);
  Joystick.sliderRight(encoders[2].counts);
}

////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  Joystick.useManualSend(true);

  InitButtons();
  InitEncoders();

  Serial.println("Hey gamer, we're ready to game");
}

void loop() {
  UpdateButtons();
  UpdateEncoders();

  Joystick.send_now();

  delay(1000 / 200);  //200Hz update
}
