🔌 NIKOLAGAME Console — Wiring Guide

This document explains how to connect the hardware components used in the NIKOLAGAME Console prototype.

Controller: ESP32 DevKit V1
Display: SSD1306 OLED, 128 × 64, I²C
Input: Analog joystick and four push buttons
Audio: Passive buzzer

«Important: Disconnect USB and all other power sources before changing wiring. The pin assignments below must match the firmware.»

---

1. OLED Display — SSD1306 I²C

Connect the OLED display to the ESP32 as follows:

OLED Pin| ESP32 Pin
VCC| 3V3*
GND| GND
SDA| GPIO 21
SCL| GPIO 22

*Use 3.3 V only if supported by your particular OLED module.

Notes:

- The common I²C address is "0x3C".
- Some OLED modules use "0x3D".
- If the display remains blank, check the wiring, I²C address, and library configuration.

---

2. Analog Joystick

Connect the joystick module to the ESP32:

Joystick Pin| ESP32 Pin
VCC| 3V3
GND| GND
VRx| GPIO 34
VRy| GPIO 35
SW| GPIO 25

How it works:

- "VRx" reads horizontal movement.
- "VRy" reads vertical movement.
- "SW" detects when the joystick is pressed.

GPIO 34 and GPIO 35 are input-only pins, which makes them suitable for analog readings. They do not have internal pull-up resistors.

---

3. Navigation Buttons

Each push button connects between its assigned GPIO and GND.

Button| ESP32 GPIO| Firmware Configuration
Left| GPIO 26| "INPUT_PULLUP"
Right| GPIO 27| "INPUT_PULLUP"
Select| GPIO 14| "INPUT_PULLUP"
Back| GPIO 13| "INPUT_PULLUP"

Button Wiring

For each button:

- Connect one terminal to the assigned GPIO.
- Connect the other terminal to GND.
- Configure the pin as "INPUT_PULLUP" in the firmware.

With this configuration:

- Not pressed: "HIGH"
- Pressed: "LOW"

An external pull-up resistor is normally unnecessary for these buttons when the ESP32's internal pull-up is enabled.

---

4. Passive Buzzer

Buzzer Connection| ESP32
Signal / positive terminal, where applicable| GPIO 4 through a suitable drive circuit if needed
GND / negative terminal, where applicable| GND

The exact connections depend on the buzzer module.

Safety notes:

- Confirm whether the buzzer is passive or active.
- Check its rated voltage and current.
- Do not exceed the ESP32 GPIO current capability.
- Use a transistor driver if the buzzer requires more current than the GPIO can safely provide.

---

5. Complete GPIO Reference

Component| ESP32 GPIO
OLED SDA| 21
OLED SCL| 22
Joystick VRx| 34
Joystick VRy| 35
Joystick SW| 25
Left Button| 26
Right Button| 27
Select Button| 14
Back Button| 13
Buzzer Signal| 4

All components must share a common GND when connected to the same power system.

---

6. Recommended Connection Order

1. Connect the ESP32 to USB without peripherals and confirm it powers on.
2. Disconnect USB before adding the OLED.
3. Connect the OLED and run "tests/test_oled.ino".
4. Add the joystick and run "tests/test_joystick.ino".
5. Add the buttons and run "tests/test_buttons.ino".
6. Add the buzzer and run "tests/test_buzzer.ino".
7. Test the combined hardware using the main firmware.

Test one component at a time to make faults easier to identify.

---

7. Battery Power

Use USB power during initial development.

Do not connect a lithium-ion battery, charging module, or boost converter until the complete power circuit has been verified. Refer to ""POWER.md"" (POWER.md) for power-design and battery-safety guidance.

---

8. Troubleshooting

Problem| Possible Checks
OLED is blank| VCC, GND, SDA, SCL, I²C address
Joystick readings do not change| VRx, VRy, power and GND
Buttons appear permanently pressed| GPIO assignment, GND and "INPUT_PULLUP"
Buzzer is silent| Buzzer type, signal pin and drive circuit
ESP32 resets unexpectedly| USB supply, short circuits and loose connections

---

Project: NIKOLAGAME Console
Platform: ESP32 DevKit V1
Maintained by: LUMINEXA

Made with ❤️ in Tanzania 🇹🇿
