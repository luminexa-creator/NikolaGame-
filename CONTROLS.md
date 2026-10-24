NIKOLAGAME Console — Game User Guide

Welcome to NIKOLAGAME Console, a portable retro gaming project by LUMINEXA, built using the ESP32 microcontroller and an OLED display.

This guide explains how to navigate the console, select games, and use the controls.

1. Getting Started

1. Connect the ESP32 to a suitable USB power source.
2. Wait for the firmware to start.
3. The OLED display should show the game interface if the correct firmware is installed.
4. Use the joystick and buttons to navigate the menu and play games.

Note: The exact menu and controls depend on the firmware version installed on the ESP32.

2. Controls

Control| Function
Joystick UP| Move up or navigate upward
Joystick DOWN| Move down or navigate downward
Joystick LEFT| Move left or navigate backward
Joystick RIGHT| Move right or navigate forward
Joystick Button (SW)| Select or confirm
Button A| Primary action
Button B| Secondary action
Button START| Start or pause a game
Button BACK| Return to the previous menu
Buzzer| Provides sound effects when supported

The final button mapping may differ depending on the installed firmware.

3. Selecting a Game

1. Turn on the console.
2. Navigate through the game menu using the joystick.
3. Highlight the game you want to play.
4. Press the joystick button or the configured SELECT button.
5. Follow the instructions displayed on the OLED screen.

4. Playing a Game

- Use the joystick to control movement.
- Press the action button when required.
- Use START or the designated pause button if the game supports pausing.
- Follow any score, timer, or status information shown on the screen.

5. Returning to the Main Menu

If the game supports exiting, press the configured BACK button or follow the on-screen instructions.

Some games may require a reset if the current firmware does not implement a return-to-menu feature.

6. Troubleshooting

The screen is blank

- Check the OLED wiring.
- Confirm that SDA is connected to GPIO 21 and SCL to GPIO 22, if using the recommended wiring.
- Check the OLED I²C address, commonly "0x3C" or "0x3D".
- Confirm that the firmware has been uploaded successfully.

The joystick does not respond

- Check the VRx, VRy, and SW connections.
- Confirm that the firmware uses the correct GPIO assignments.
- Test the joystick using the dedicated hardware test sketch.

The buttons do not work

- Check the button wiring and GPIO assignments.
- For buttons configured with "INPUT_PULLUP", connect each button between its GPIO pin and GND.
- Verify that the firmware reads the correct button pins.

There is no sound

- Check the buzzer connections and firmware settings.
- Confirm whether the buzzer requires a driver circuit.

7. Safety

- Use a suitable USB power source while testing.
- Verify the power requirements before connecting a battery.
- Avoid short circuits and incorrect voltage connections.
- Disconnect power before changing wiring.

8. Project Status

The user guide describes the intended controls. Actual functionality depends on the firmware currently installed.

Not every planned game or feature is necessarily available in the current build.

---

NIKOLAGAME Console
Made with ❤️ in Tanzania 🇹🇿
By LUMINEXA
