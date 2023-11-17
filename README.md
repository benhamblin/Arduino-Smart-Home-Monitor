# Smart Home Monitor - Embedded Systems Programming

## Project Overview
This project involves programming an Arduino to function as a dummy smart home monitor. The system interacts with custom messages over the Serial Monitor and displays the status of various smart devices on an LCD screen. Users can navigate the device information using Arduino's buttons.

## Features
- **Custom Message Handling**: Receives and processes custom text strings over Serial Interface.
- **Device Management**: Manages smart devices like Speakers, Sockets, Lights, Thermostats, and Cameras.
- **LCD Display**: Presents device information, including ID, location, type, state, and power percentage.
- **User Interaction**: Utilizes Arduino buttons for navigating and interacting with the displayed information.
- **State Indicators**: Changes backlight color based on the device state (yellow for OFF, green for ON).

## Technical Specifications
- **Language**: Embedded C/C++ for Arduino.
- **Hardware**: Arduino Uno with LCD shield.
- **Serial Communication**: Custom protocol over Serial Monitor for device interaction and monitoring.

## Installation and Usage
1. Connect the Arduino Uno to your computer.
2. Upload the provided `.ino` file to the Arduino using the Arduino IDE.
3. Interact with the system via the Serial Monitor or by using the buttons on the Arduino LCD shield.

## Protocol Details
- **Synchronization Phase**: Involves character exchange between Arduino and the host for setup.
- **Main Phase**: Handles various operations like adding new devices, changing states, adjusting power output, and removing devices.
- **Error Handling**: Reports protocol deviations and invalid messages for debugging.

## Finite State Machine (FSM)
- The system operates based on a well-defined FSM.
- Documentation includes a detailed FSM diagram with states and transitions.

## Extensions (Optional)
- **UDCHARS**: Custom character definitions for LCD display.
- **FREERAM**: Displaying free SRAM on the Arduino.
- **HCI**: Enhanced user interface for filtering device states.
- **EEPROM**: Persistent storage of device data.
- **SCROLL**: Scrolling long text on the LCD.

## Development and Testing
- Utilize the Arduino IDE for development.
- Test functionalities using the provided Python script or directly via the Serial Monitor.

## Documentation and Submission
- Include a detailed report in PDF format, outlining FSM, implementation details, and reflection on the code.
- Submit the `.ino` source file and the report as specified in the coursework guidelines.

## Contributing
This is an individual coursework project. Collaboration or code sharing is strictly prohibited and subject to academic integrity policies.

## License
This project is intended for educational purposes within the context of the Embedded Systems Programming course. Redistribution or use outside this context requires appropriate permissions.
