#include <light_CD74HC4067.h>
#include <Joystick.h>

CD74HC4067 mux(2, 3, 4, 5);
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD, 12, 0, false, false, false, false, false, false, false, false, false, false);

const int signal_pin = A0;
const int pressed_deadband = 870;            // Threshold for button press
const unsigned long pressed_cooldown = 300;  // Cooldown in milliseconds
const unsigned long debounce = 20;           // Debounce in milliseconds

// Map button presses to standard joystick buttons (1-12)
const int buttonMapping[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

// Categorizing buttons
const int toggleButtons[] = { 0, 1, 2, 3, 4, 5 };       // Toggle buttons
const int momentaryButtons[] = { 6, 7, 8, 9, 10, 11 };  // Momentary buttons

// Button class definition
class Button {
public:
  unsigned long last_pressed_time;
  bool state;
  bool is_toggle;
  bool last_value;

  Button() {
    last_pressed_time = 0;
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
    } else {  // Update state for momentary button
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
Button buttons[12];

void setup() {
  Serial.begin(9600);
  pinMode(signal_pin, INPUT);
  Joystick.begin();

  // loop that determines whether or not a button is a toggle button or momentary button
  for (byte i = 0; i < 12; i++) {
    for (int j = 0; j < sizeof(toggleButtons) / sizeof(toggleButtons[0]); j++) {
      if (toggleButtons[j] == i) {
        buttons[i].is_toggle = true;
        break;
      }
    }
  }
}
void loop() {
  unsigned long current_time = millis();
  for (byte i = 0; i < 12; i++) {
    mux.channel(i);
    int val = analogRead(signal_pin);
    if (buttons[i].shouldUpdatePose(val, current_time)) {
      bool button_state = val > pressed_deadband; 
      Joystick.setButton(buttonMapping[i], button_state);
    }
  }
}
