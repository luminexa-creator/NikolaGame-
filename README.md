🎮 NIKOLAGAME Console

A Portable Retro Gaming Console Powered by ESP32

NIKOLAGAME Console is an open-source retro gaming console project built around the ESP32 DevKit V1, an SSD1306 OLED display, and a joystick-based control system. The goal is to bring classic arcade-style gaming to a compact, affordable, and maker-friendly device.

<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif" alt="ESP32">
  <img src="https://img.shields.io/badge/Display-OLED-6A5ACD?style=for-the-badge" alt="OLED">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-orange?style=for-the-badge&logo=cplusplus" alt="C++">
  <img src="https://img.shields.io/badge/Status-Development-yellow?style=for-the-badge" alt="Development status">
</p><p align="center">
  <b>Designed by LUMINEXA</b><br>
  Made with ❤️ in Tanzania 🇹🇿
</p>---

📑 Table of Contents

- "Overview" (#-overview)
- "Features" (#-features)
- "Games" (#-games)
- "Hardware Requirements" (#-hardware-requirements)
- "Pin Configuration" (#-pin-configuration)
- "Software Requirements" (#-software-requirements)
- "Installation" (#-installation)
- "Controls" (#-controls)
- "Project Structure" (#-project-structure)
- "Testing and Verification" (#-testing-and-verification)
- "Power and Battery Safety" (#-power-and-battery-safety)
- "Troubleshooting" (#-troubleshooting)
- "Roadmap" (#-roadmap)
- "Contributing" (#-contributing)
- "License" (#-license)

---

🚀 Overview

NIKOLAGAME Console is designed for electronics enthusiasts, students, and developers interested in embedded systems, Arduino programming, and retro gaming.

The project combines an ESP32 microcontroller with a small OLED display, a joystick, navigation buttons, and sound output to create a handheld gaming experience.

Project Goals

- Build a compact retro gaming console.
- Learn embedded C++ programming.
- Explore OLED graphics and input handling.
- Implement multiple arcade and puzzle games.
- Experiment with persistent high scores and sound effects.
- Develop an affordable electronics project that can be assembled and improved by the maker community.

✨ Features

The planned feature set includes:

- 🖥️ SSD1306 OLED graphics over I²C.
- 🕹️ Analog joystick navigation.
- 🎮 Dedicated buttons for game controls.
- 🔊 Buzzer sound effects.
- 🏆 High-score storage using ESP32 Preferences/NVS.
- ⚙️ Menu navigation and game state management.
- 🔇 Optional sound control.
- 🔋 Optional rechargeable battery power.
- 🧩 Modular code and separate hardware documentation.

Note: Features should be considered implemented only after the corresponding firmware and hardware have been tested.

🕹️ Games

The planned game collection includes ten classic-style games.

#| Game| Category
1| Pong| Arcade
2| Snake| Arcade
3| Breakout| Arcade
4| Space Invaders| Arcade
5| Flappy Bird| Arcade
6| Race Car| Arcade
7| Whack-A-Mole| Reaction
8| Simon Says| Memory
9| Tetris| Puzzle
10| Lights Out| Puzzle

Game availability and completion depend on the current firmware version.

🔧 Hardware Requirements

Core Components

Component| Purpose
ESP32 DevKit V1| Main microcontroller
SSD1306 OLED, 128 × 64| Graphics and game display
Analog joystick module| Direction and movement input
Push buttons| Menu navigation and actions
Passive buzzer| Sound effects
Breadboard and jumper wires| Prototyping

Optional Power Components

- Protected 18650 lithium-ion cell.
- Compatible lithium-ion charging and protection circuitry.
- Regulated power converter, if required by the selected power design.
- Power switch and suitable fuse.

Start by powering and testing the console through USB. Add battery power only after verifying the complete power circuit.

🔌 Pin Configuration

The following pin assignments are the proposed default configuration.

OLED Display — SSD1306 I²C

OLED Pin| ESP32 Pin
VCC| 3V3
GND| GND
SDA| GPIO 21
SCL| GPIO 22

The typical I²C address is "0x3C"; some displays use "0x3D".

Joystick

Joystick Pin| ESP32 Pin
VCC| 3V3
GND| GND
VRx| GPIO 34
VRy| GPIO 35
SW| GPIO 25

GPIO 34 and GPIO 35 are input-only pins, making them suitable for analog joystick readings.

Buttons and Buzzer

Component| ESP32 Pin
Left Button| GPIO 26
Right Button| GPIO 27
Select Button| GPIO 14
Back Button| GPIO 13
Passive Buzzer| GPIO 4

For buttons configured with "INPUT_PULLUP", connect each button between its assigned GPIO and GND. Confirm that the firmware uses the same pin assignments.

For the buzzer, verify its type and current requirements before connecting it. Use a suitable driver circuit if required.

💻 Software Requirements

Install the following tools:

- "Arduino IDE" (https://www.arduino.cc/en/software)
- "ESP32 Arduino Core" (https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)
- An SSD1306-compatible display library, such as "Adafruit SSD1306" (https://github.com/adafruit/Adafruit_SSD1306)
- "Adafruit GFX Library" (https://github.com/adafruit/Adafruit-GFX-Library)

The exact libraries required depend on the firmware source.

⚡ Installation

1. Clone the Repository

git clone https://github.com/luminexa-creator/nikolagame-console.git

2. Open the Firmware

Open the main firmware file in Arduino IDE:

firmware/nikolagame_console.ino

3. Configure the Board

In Arduino IDE:

1. Install the ESP32 board package.
2. Select the appropriate ESP32 Dev Module board.
3. Select the correct serial port.
4. Install any missing libraries.

4. Connect the Hardware

Follow the pin configuration above and check every connection before applying power.

5. Compile and Upload

Compile the firmware, upload it to the ESP32, and open Serial Monitor if the firmware provides diagnostic messages.

Important: The repository must contain the actual firmware and required files before these steps can be completed successfully.

🎮 Controls

The intended default controls are:

Input| Intended Action
Joystick| Move or navigate
Left Button| Previous option or left action
Right Button| Next option or right action
Select Button| Select or confirm
Back Button| Return or exit
Long Press| Additional menu action, if implemented

Exact controls may differ between games and firmware versions. See "docs/CONTROLS.md" for the confirmed control mapping.

📁 Project Structure

nikolagame-console/
├── README.md
├── LICENSE
├── CHANGELOG.md
├── .gitignore
├── firmware/
│   └── nikolagame_console.ino
├── hardware/
│   ├── WIRING.md
│   ├── BOM.md
│   └── POWER.md
├── docs/
│   ├── CONTROLS.md
│   ├── GAMES.md
│   ├── TROUBLESHOOTING.md
│   └── TESTING.md
└── tests/
    ├── test_oled.ino
    ├── test_buzzer.ino
    ├── test_joystick.ino
    ├── test_buttons.ino
    └── test_power.ino

This is the intended repository structure; create each file before linking to it from GitHub.

🧪 Testing and Verification

Before calling the console ready for release, verify:

- [ ] The firmware compiles without errors.
- [ ] The OLED initializes and displays graphics correctly.
- [ ] The joystick reads both axes correctly.
- [ ] Every button responds as expected.
- [ ] The buzzer produces the intended sounds.
- [ ] Each included game can start, run, and exit.
- [ ] High scores persist after restarting, if implemented.
- [ ] The power circuit operates within the components' voltage and current limits.
- [ ] Extended gameplay does not cause unexpected resets or overheating.

See "docs/TESTING.md" for detailed test procedures.

🔋 Power and Battery Safety

For initial development, use a suitable USB power source.

If you add a lithium-ion battery:8

- Use a charger compatible with the exact cell chemistry and configuration.
- Confirm that the charging circuit includes the necessary protection.
- Check the polarity and voltage of every module.
- Measure the output of any boost converter before connecting the ESP32.
- Do not assume every TP4056 module has the same protection features.
- Do not charge a battery unattended or use damaged cells.
- Design for simultaneous charging and operation only if the circuit explicitly supports it.

Do not connect a battery directly to an ESP32 power pin without verifying the permitted voltage and the complete power design.

🛠️ Troubleshooting

Problem| Things to Check
OLED stays blank| Power, GND, SDA/SCL, I²C address and display library
ESP32 does not upload| USB data cable, board selection, serial port and boot mode
Joystick moves incorrectly| Axis wiring, analog readings and calibration
Buttons do not respond| GPIO mapping, GND connection and pull-up configuration
No buzzer sound| Buzzer type, polarity where applicable, pin assignment and drive circuit
ESP32 restarts unexpectedly| USB supply, wiring, current demand and short circuits

For additional help, open an issue in the repository with the error message, hardware setup, and steps needed to reproduce the problem.

🗺️ Roadmap

- [ ] Complete and verify the main firmware.
- [ ] Test the OLED, joystick, buttons, and buzzer independently.
- [ ] Implement and verify the game collection.
- [ ] Confirm high-score storage.
- [ ] Publish wiring diagrams and a complete bill of materials.
- [ ] Validate the optional battery power system.
- [ ] Publish a tested release with build instructions and photos.

🤝 Contributing

Contributions, bug reports, suggestions, and improvements are welcome.

1. Fork the repository.
2. Create a feature branch.
3. Make your changes.
4. Test your changes on the intended hardware.
5. Submit a pull request describing what changed and how it was tested.

For bugs or feature requests, use "GitHub Issues" (https://github.com/luminexa-creator/nikolagame-console/issues).

📄 License

A license has not yet been specified. Add a "LICENSE" file before publishing the project with explicit permissions for reuse, modification, and distribution.

---

<p align="center">
  <b>NIKOLAGAME Console</b><br>
  An embedded gaming project by LUMINEXA<br><br>
  <b>Made with ❤️ in Tanzania 🇹🇿</b>
</p>
