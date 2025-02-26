#include <light_CD74HC4067.h>
#include <Joystick.h>

CD74HC4067 mux(2, 3, 4, 5);
CD74HC4067 mux2(15, 14, 16, 10);

// Map pin id's to position around coral
int channelToButton[] = {7, 5, 6, 4, 3, 2, 8, 9, 0, 1, 10, 11, 1, 0, 5, 4, 2, 3};
// int yellowChannelToButton = {1, 0, 5, 4, 2, 3};

const int signal_pin = A0;                    // Pin Connected to Sig pin of CD74HC4067
const int pressed_deadband = 870;             // Min analogue reading indicating button is pressed
const unsigned long pressed_cooldown = 300;   // Cooldown in milliseconds
const unsigned long debounce = 20;

byte april_tag_position = -1;  // Variable to store the letter of the pressed button

// Arrays to store button numbers for different types of buttons
const int toggleButtons[] = {0, 1, 2, 3, 4, 5};       // Buttons that stay in when pressed
const int momentaryButtons[] = {8, 9, 10, 11, 6, 7};  // Buttons that do not stay in when pressed

class CoralButton {
 public:
  unsigned long last_pressed_time;
  bool state;
  bool is_toggle;
  bool last_value;

  CoralButton() {
    last_pressed_time = -1;
    state = false;
    is_toggle = false;
    last_value = false;
  }

  // Returns true if pose should be updated
  bool shouldUpdatePose(int value, unsigned long current_time) {
    bool curr_pressed = value > pressed_deadband;
    if (is_toggle) {  // Update state for toggle buttons
      if (curr_pressed != last_value) {
        last_value = curr_pressed;
        return true;
      }
      return false;
    } else {  // Update state for momentary buttons
      if (curr_pressed && !last_value && (current_time - last_pressed_time > pressed_cooldown)) {
        last_pressed_time = current_time;  // Record time when pressed
        last_value = curr_pressed;
        return true;
      }
      last_value = curr_pressed;
      return false;
    }
  }
};

// Elevator class to handle elevator levels
class Elevator {
 public:
  int levels[4];
  
  Elevator(int pin1, int pin2, int pin3, int pin4) {
    levels[0] = pin1;
    levels[1] = pin2;
    levels[2] = pin3;
    levels[3] = pin4;
  }
  
  void checkLevels() {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(levels[i]) == HIGH) {
        Serial.println("Elevator level: " + String(i));
        delay(debounce);  // debounce delay
        resetLevels();
        break;
      }
    }
  }

 private:
  void resetLevels() {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(levels[i]) == HIGH) {
        // Wait for button to be released
        while (digitalRead(levels[i]) == HIGH) {
          delay(10);
        }
      }
    }
  }
};

// Create an array of Button objects for each coral position
CoralButton coral_buttons[12];

// Create an elevator object with buttons on pins 6, 7, 8, 9
Elevator elevator(6, 7, 8, 9);

// Create joystick
Joystick_ joystick;

void setup() {
  Serial.begin(9600);
  pinMode(signal_pin, INPUT);  // Set as input for reading through signal pin

  // Record whether each button is a toggle or momentary button
  for (byte i = 0; i < 12; i++) {
    for (int j = 0; j < sizeof(toggleButtons) / sizeof(toggleButtons[0]); j++) {
      if (toggleButtons[j] == i) {
        coral_buttons[i].is_toggle = true;
        break;
      }
    }
  }
  
  // Set elevator level pins as input
  for (int i = 6; i < 10; i++) {
    pinMode(i, INPUT);
  }
  
  // Initialize Joystick Library
	joystick.begin();
}

void loop() {
  // Loop through all coral buttons mux[0, 11]
  for (byte i=0; i < 12; i++) {
    mux.channel(i);
    int val = analogRead(signal_pin);
    unsigned long current_time = millis();

    if (coral_buttons[i].shouldUpdatePose(val, current_time)) {
      april_tag_position = channelToButton[i];  // Update the AprilTagPosition with the corresponding button number
      Serial.println(april_tag_position);
    }
  }
  
  // Set joystick values for coral position
  for(byte but = 0; but < 12; but++) {
    joystick.setButton(0, LOW);
  }
  if(april_tag_position != -1) {
    joystick.setButton(april_tag_position, HIGH);
  }

  // for(byte i=12; i<14; i++) {
  //   Serial.print(String(val) + "---");
  // }

  // The six yellow buttons
  // for(byte i=0; i<7; i++) {
  //   mux2.channel(i);
  //   int val = analogRead(A0);
  // }

  // // The six swithces
  // for(byte i=7; i<13; i++) {
  //   mux2.channel(i);
  //   int val = analogRead(A1);
  //   Serial.print(String(val) + "---");
  // }


  // for (int i = 6; i < 10; i++) {
  //   if (digitalRead(i) == LOW) {
  //     Serial.println("Elevator level: " + String(i));
  //   }
  // }

  //Serial.println(digitalRead(6));
  
  // Reset
  april_tag_position = -1;
}
