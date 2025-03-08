#include <light_CD74HC4067.h>
#include <Joystick.h>

const int ANALOG_PRESSED_DEADBAND = 900;
const unsigned long INSTANTANEOUS_BUTTON_POSTPRESS_COOLDOWN = 200;

enum ActuatorCategory {
  CORAL_POSE,
  YELLOW_HOLD,
  WHITE_ELEVATOR_LEVEL,
  RED_TOGGLE,
  SWITCH
};

enum ActuatorType {
  TOGGLE_BUTTON,         // A button that toggles between states when pressed; considered "on" for one loop iteration
  INSTANTANEOUS_BUTTON,  // A button that returns to "off" state immediately after release; requires a cooldown period to prevent repeated triggers
  HOLD_BUTTON,           // A button that should be considered "on" for multiple loop iterations as long as it reading is high
  SWITCH_ACTUATOR         // A switch with distinct "on" and "off" positions; remains "on" as long as the reading is high
};

enum ActuatorLocation {
    MUX_A,
    MUX_B,
    DIG
};

class Actuator {
  public:
    Actuator(ActuatorCategory category, ActuatorType type, ActuatorLocation location, uint8_t pin, uint8_t joystickId)
      : category(category), type(type), location(location), pin(pin), joystickId(joystickId), lastPressedTime(0), lastState(false), valueToReport(false) {}

    ActuatorLocation getLocation() const {
      return location;
    }

    ActuatorCategory getCategory() const {
      return category; 
    }

    uint8_t getPin() const {
      return pin;
    }

    uint8_t getJoystickId() const {
      return joystickId;
    }

    bool getReportedValue() {
      return valueToReport;
    }

    void update(int value, unsigned long currentTime) {
      bool currentState;

      valueToReport = false;
      
      // Standardize any value to true/false boolean
      switch(location) {
        case MUX_A:
        case MUX_B:
          currentState = value > ANALOG_PRESSED_DEADBAND;
          break;
        case DIG:
          currentState = (value == HIGH);
          break;
      }
  
      // Interpret the value based on actuator type
      switch(type) {
        case TOGGLE_BUTTON: {
          if (currentState != lastState && ((currentTime - lastPressedTime) > INSTANTANEOUS_BUTTON_POSTPRESS_COOLDOWN || lastPressedTime == 0)) {
            lastPressedTime = currentTime;
            valueToReport = !valueToReport;
          }
          break;
        }
        case INSTANTANEOUS_BUTTON: {
          if (currentState && !lastState && ((currentTime - lastPressedTime) > INSTANTANEOUS_BUTTON_POSTPRESS_COOLDOWN || lastPressedTime == 0)) {
            lastPressedTime = currentTime;
            valueToReport = true;
          }
          break;
        }
        case HOLD_BUTTON:
        case SWITCH_ACTUATOR: {
          if (currentState) {
            lastPressedTime = currentTime;
            valueToReport = true;
          }
          break;
        }
      }

      lastState = currentState;
    }

  private:
    const ActuatorCategory category;
    const ActuatorType type;
    const ActuatorLocation location;
    const uint8_t pin;
    const uint8_t joystickId;

    unsigned long lastPressedTime;
    bool lastState;
    bool valueToReport;
};

class Multiplexer {
  public:
    Multiplexer(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, uint8_t sigPin)
      : multiplexer(s0, s1, s2, s3), signalPin(sigPin) {}

    void setChannel(uint8_t channel) {
      multiplexer.channel(channel);
    }

    int readAnalog() {
      return analogRead(signalPin);
    }

    int readAnalog(uint8_t channel) {
      setChannel(channel);
      return analogRead(signalPin);
    }

    uint8_t getSignalPin() {
      return signalPin;
    }

  private:
    CD74HC4067 multiplexer;
    uint8_t signalPin;
};

Multiplexer muxA(10, 16, 14, 15, A0);
Multiplexer muxB(9, 8, 7, 6, A3);

Joystick_ joystick;

Actuator actuators[] = {
  Actuator(CORAL_POSE, TOGGLE_BUTTON, MUX_A, 0, 0),
  Actuator(CORAL_POSE, TOGGLE_BUTTON, MUX_A, 1, 2),
  Actuator(CORAL_POSE, TOGGLE_BUTTON, MUX_A, 2, 1),
  Actuator(CORAL_POSE, TOGGLE_BUTTON, MUX_A, 3, 3),
  Actuator(CORAL_POSE, TOGGLE_BUTTON, MUX_A, 4, 4),
  Actuator(CORAL_POSE, TOGGLE_BUTTON, MUX_A, 5, 5),
  Actuator(CORAL_POSE, INSTANTANEOUS_BUTTON, MUX_A, 6, 11),
  Actuator(CORAL_POSE, INSTANTANEOUS_BUTTON, MUX_A, 7, 10),
  Actuator(CORAL_POSE, INSTANTANEOUS_BUTTON, MUX_A, 8, 7),
  Actuator(CORAL_POSE, INSTANTANEOUS_BUTTON, MUX_A, 9, 6),
  Actuator(CORAL_POSE, INSTANTANEOUS_BUTTON, MUX_A, 10, 9),
  Actuator(CORAL_POSE, INSTANTANEOUS_BUTTON, MUX_A, 11, 8),
  Actuator(WHITE_ELEVATOR_LEVEL, INSTANTANEOUS_BUTTON, MUX_A, 12, 19),
  Actuator(WHITE_ELEVATOR_LEVEL, INSTANTANEOUS_BUTTON, MUX_A, 13, 20),
  Actuator(WHITE_ELEVATOR_LEVEL, INSTANTANEOUS_BUTTON, MUX_A, 14, 18),
  Actuator(WHITE_ELEVATOR_LEVEL, INSTANTANEOUS_BUTTON, MUX_A, 15, 21),
  Actuator(YELLOW_HOLD, HOLD_BUTTON, MUX_B, 0, 13),
  Actuator(YELLOW_HOLD, HOLD_BUTTON, MUX_B, 1, 12),
  Actuator(YELLOW_HOLD, HOLD_BUTTON, MUX_B, 2, 16),
  Actuator(YELLOW_HOLD, HOLD_BUTTON, MUX_B, 3, 17),
  Actuator(YELLOW_HOLD, HOLD_BUTTON, MUX_B, 4, 15),
  Actuator(YELLOW_HOLD, HOLD_BUTTON, MUX_B, 5, 14),
  Actuator(SWITCH, SWITCH_ACTUATOR, MUX_B, 7, 22),
  Actuator(SWITCH, SWITCH_ACTUATOR, MUX_B, 8, 25),
  Actuator(SWITCH, SWITCH_ACTUATOR, MUX_B, 9, 23),
  Actuator(SWITCH, SWITCH_ACTUATOR, MUX_B, 10, 26),
  Actuator(SWITCH, SWITCH_ACTUATOR, MUX_B, 11, 27),
  Actuator(SWITCH, SWITCH_ACTUATOR, MUX_B, 12, 24),
  Actuator(RED_TOGGLE, HOLD_BUTTON, MUX_B, 14, 28),
  Actuator(RED_TOGGLE, HOLD_BUTTON, MUX_B, 15, 29),
};

void setup() {
  Serial.begin(9600);

  // Set multiplexer signal pins as input
  pinMode(muxA.getSignalPin(), INPUT);
  pinMode(muxB.getSignalPin(), INPUT);

  for (int i = 0; i < sizeof(actuators) / sizeof(actuators[0]); ++i) {
    Actuator& actuator = actuators[i];  // Reference to the existing Actuator object

    // Actuators plugged directly into Arduino need their pin to be set to input
    if (actuator.getLocation() == DIG) {
      pinMode(actuator.getPin(), INPUT);
    }
  }

  joystick.begin();
}

void loop() {
  unsigned long currentTime = millis();  // Get the current time once to use in the loop

  for (int i = 0; i < sizeof(actuators) / sizeof(actuators[0]); ++i) {
    Actuator& actuator = actuators[i];  // Reference to the existing Actuator object

    // Some actuator categories should get "reset" on every iteration
    if (actuator.getCategory() == CORAL_POSE || actuator.getCategory() == WHITE_ELEVATOR_LEVEL) {
      joystick.setButton(actuator.getJoystickId(), LOW);
    }

    int value;
    switch (actuator.getLocation()) {
      case MUX_A:
        value = muxA.readAnalog(actuator.getPin());
        break;
      case MUX_B:
        value = muxB.readAnalog(actuator.getPin());
        break;
      case DIG:
        value = digitalRead(actuator.getPin());
        break;
    }
    actuator.update(value, currentTime);

    if (actuator.getReportedValue()) {
      joystick.setButton(actuator.getJoystickId(), HIGH);
    } else {
      joystick.setButton(actuator.getJoystickId(), LOW);
    }
  }

  delay(20);  // DriverStation needs time to register the press
}
