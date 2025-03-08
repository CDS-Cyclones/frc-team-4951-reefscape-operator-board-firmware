# Team 4951 Operator Board (Custom Controller)

This custom controller features 24 buttons and 6 switches powered by a Micro Pro, utilizing two CD74HC4067 multiplexers. Here's a breakdown:
- **12 Buttons**: Represent the shape of the coral on the field, with each button corresponding to a spike where a coral can be placed.
- **4 Buttons on the Right**: Represent the four levels of scoring (L1-L4).
- **Remaining Buttons**: Functionalities will be disclosed once finalized.

---

## Layout and Button IDs
![This image shows layout along with each actuator's ID.](https://i.ibb.co/PztC0RtX/operatorboardlayout.png)  
*Note: In WPILIB HID, button indexes are offset by one. For example, index 0 becomes 1, index 2 becomes 3, and so on.*

---

## Photos
*This section will feature a photo of the final controller, to be added later.*  

---

## Libraries Used
- [light_CD74HC4067](https://github.com/SunitRaut/Lightweight-CD74HC4067-Arduino) (version 1.0.1)
- [Joystick](https://github.com/MHeironimus/ArduinoJoystickLibrary) (version 2.1.1)
