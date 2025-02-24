#include <light_CD74HC4067.h>

CD74HC4067 mux(2, 3, 4, 5);

// Map pin id's to position around coral
char channelToLetter[] = {'H', 'F', 'G', 'E', 'D', 'C', 'I', 'J', 'A', 'B', 'K', 'L'};

const int signal_pin = A0;                    // Pin Connected to Sig pin of CD74HC4067
const int pressed_deadband = 870;             // Min analogue reading indicating button is pressed
const unsigned long pressed_cooldown = 300;   // Cooldown in milliseconds
const unsigned long debounce = 20;

char april_tag_position = ' ';  // Variable to store the letter of the pressed button

// Arrays to store button numbers for different types of buttons
const int toggleButtons[] = {0, 1, 2, 3, 4, 5};       // Buttons that stay in when pressed
const int momentaryButtons[] = {8, 9, 10, 11, 6, 7};  // Buttons that do not stay in when pressed

class Button {
 public:
  unsigned long last_pressed_time;
  bool state;
  bool is_toggle;
  bool last_value;

  Button() {
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

// Create an array of Button objects for each coral position
Button coral_buttons[12];

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
}

void loop() {
  // Loop through channels [0, 11]
  for (byte i = 0; i < 12; i++) {
    mux.channel(i);
    int val = analogRead(signal_pin);
    unsigned long current_time = millis();

    if (coral_buttons[i].shouldUpdatePose(val, current_time)) {
      april_tag_position = channelToLetter[i];  // Update the AprilTagPosition with the corresponding letter
      Serial.println("April tag position: " + String(april_tag_position));
    }
  }
}
