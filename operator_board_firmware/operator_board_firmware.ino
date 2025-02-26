#include <light_CD74HC4067.h>
#include <Joystick.h>

CD74HC4067 mux(2, 3, 4, 5);
//6,7,8,9
CD74HC4067 mux2(15,14,16,10);
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD, 12, 0, false, false, false, false, false, false, false, false, false, false);

const int signal_pin = A0;
const int signal_pin2 = A1;
const int pressed_deadband = 870;            // Threshold for button press
const unsigned long pressed_cooldown = 300;  // Cooldown in milliseconds
const unsigned long debounce = 20;           // Debounce in milliseconds

// Map button presses to standard joystick buttons (1-12)
const int buttonMapping[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ,12, 13, 14,15,16,17,18,19,20,21,22,23,24,25,26,27};

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

class OtherStuff{
  public:
    unsigned long last_pressed_time;
    bool state;
    OtherStuff(){
      last_pressed_time = 0;
      state = false;
          }
      void UpdateState(int value, unsigned long current_time){
          if ((current_time - last_pressed_time) < pressed_deadband pressed_deadband && value == state){
            return 0;
          }
          bool curr_pressed = value > pressed_deadband;
          if (!state && curr_pressed){
            last_pressed_time = current_time;
          }
          state = curr_pressed;

      }
      void UpdateStateForDigitalButtonsCzTheirSpecialAndGoFrom1To0InsteadOf0To1(int value, unsigned long current_time){
        if ((current_time - last_pressed_time) < pressed_deadband && value == state){
            return 0;
          }
          bool curr_pressed = value;
          if (state && !curr_pressed){
            last_pressed_time = current_time;
          }
          state = curr_pressed;
      }
      unsigned long getLastPressed(){
        return last_pressed_time;
      }
    
};
// Create an array of Button objects for each coral position
Button buttons[12];
OtherStuff otherbuttons[6];
OtherStuff otherswitches[6];
OtherStuff otherotherbuttons[4];
unsigned long lastDetected;
void setup() {
  Serial.begin(9600);
  pinMode(signal_pin, INPUT);
  pinMode(6,INPUT);
  pinMode(7,INPUT);
  pinMode(8,INPUT);
  pinMode(9,INPUT);
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
  for (byte i = 0; i < 6; i++){
    mux2.channel(i);
    int val = analogRead(signal_pin2);
   
    otherbuttons[i].UpdateState(val,current_time);
    Joystick.setButton(buttonMapping[12+i],otherbuttons[i].state);
  }
  for (byte i = 7; i < 13; i++){
    mux2.channel(i);
    int val = analogRead(signal_pin2);
    otherswitches[i].UpdateState(val,current_time);
    Joystick.setButton(buttonMapping[18+i],otherbuttons[i].state);
   

    
  }
  for(byte i = 6; i < 10; i++){
    int val = digitalRead(i);
    otherotherbuttons[i].UpdateStateForDigitalButtonsCzTheirSpecialAndGoFrom1To0InsteadOf0To1(val,current_time);
    Joystick.setButton(buttonMapping[23+i],otherbuttons[i].state);
  }

}
