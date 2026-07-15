# Arduino LoRa Fox Hunt

## Project Overview

The Arduino LoRa Fox Hunt project is an educational Radio Direction Finding (RDF) system designed to introduce students to embedded systems, RF communications, and practical troubleshooting.

The project consists of two handheld Arduino-based devices:

- A battery-powered LoRa beacon ("Fox")
- A handheld direction-finding receiver

Students assemble, configure, operate, and troubleshoot the equipment while learning the fundamentals of RF systems.

The long-term goal is to develop this project into a complete Practical Exercise (PE) suitable for use in RF Fundamentals and related technical training courses.

---

## Project Objectives

- Learn Arduino programming
- Learn embedded hardware integration
- Understand SPI communications
- Configure and operate LoRa radios
- Introduce RSSI concepts
- Demonstrate basic Radio Direction Finding
- Develop troubleshooting methodology
- Practice technical documentation

---

## System Architecture

The project consists of two independent devices.

### Beacon

Purpose:

Periodically transmits a configurable beacon identifier using LoRa.

Hardware

- Arduino Nano Every
- SX1262 LoRa Module
- OLED Display
- Pushbutton
- Status LED
- 18650 Battery
- SMA Antenna

Features

- Configurable beacon ID
- Adjustable transmit interval
- OLED status display
- Battery indication
- Low-power operation

---

### Direction Finding Receiver

Purpose:

Receives LoRa packets and displays received signal strength to assist in locating the beacon.

Hardware

- Arduino Nano Every
- SX1262 LoRa Module
- OLED Display
- Three Pushbuttons
- Passive Buzzer
- 18650 Battery
- SMA Antenna

Features

- RSSI display
- Packet decoding
- Audible signal indication
- Menu navigation
- Battery monitoring

---

## Repository Structure

```
arduino-lora-fox-hunt/

├── beacon/
├── receiver/
├── docs/
├── images/
└── README.md
```

---

## Current Status

Current Development

- Repository created
- Beacon prototype operational
- Beacon modifications and improvements
- Receiver under development

Planned Features

- Adjustable LoRa parameters
- Multiple beacon profiles
- RSSI averaging
- Audible proximity indication
- Battery monitoring
- Enclosure design
- Complete wiring documentation
- Student assembly guide

---

## Future Development

Future work will focus on expanding the project into a complete instructional platform including:

- Instructor documentation
- Student guide
- Wiring schematics
- Troubleshooting exercises
- Practical Exercise (PE)
- Bill of Materials
- 3D printable enclosure
- Future GPS integration

---

## License

Released under the MIT License.
