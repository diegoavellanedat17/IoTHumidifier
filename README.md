# Humidifier Circuit Hacking and MQTT Control Project

<!-- ![Project Image](project_image.jpg) <!-- Replace with an image relevant to your project -->

Welcome to the Humidifier Circuit Hacking and MQTT Control Project! This project focuses on modifying the circuit of a humidifier to enable control using MQTT messages and a 107 kHz frequency signal. This README provides an overview of the project, its goals, and how to set it up.

## Table of Contents

- [Introduction](#introduction)
- [Project Overview](#project-overview)
- [Setup](#setup)
  - [Circuit Hacking](#circuit-hacking)
  - [MQTT Configuration](#mqtt-configuration)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This project aims to showcase how a humidifier's circuit can be hacked and controlled remotely using MQTT communication. By interfacing with the circuit's control signals and leveraging a 107 kHz frequency, we can send on/off commands via MQTT messages. This provides a flexible way to manage the humidifier's operation from a remote device or application.

## Project Overview

The project consists of two main components: circuit hacking and MQTT communication.

### Circuit Hacking

We delve into the circuitry of the humidifier to identify the control signals responsible for turning it on and off. By understanding the electronics, we can interface with these signals and send custom commands to control the humidifier's operation. This project assumes a basic understanding of electronics and soldering.

### MQTT Configuration

MQTT (Message Queuing Telemetry Transport) is a lightweight messaging protocol often used in IoT applications. We set up an MQTT broker or cloud service and configure our devices to communicate with it. By subscribing to relevant MQTT topics, we can send on/off commands to the hacked humidifier circuit.

## Setup

Before you begin, ensure you have the following components ready:

- Humidifier with accessible control circuitry
- Microcontroller (Arduino, ESP8266, etc.)
- MQTT Broker or Cloud Service account

### Circuit Hacking

1. **Identify Control Signals:** Open up the humidifier and locate the control signals responsible for turning it on and off. These are often connected to buttons or relays.

2. **Interfacing:** Connect the control signals to the microcontroller's pins. Be sure to use appropriate voltage level shifters if necessary to match the microcontroller's logic levels.

3. **Frequency Generation:** If your humidifier operates at 107 kHz, configure your microcontroller to generate the appropriate frequency signal for control.

### MQTT Configuration

1. **MQTT Broker Setup:** If you don't already have an MQTT broker or cloud service, set one up. Popular choices include Mosquitto, HiveMQ, and cloud-based services like AWS IoT or Adafruit IO.

2. **Device Configuration:** Configure your microcontroller to connect to the MQTT broker using the appropriate library. Subscribe to a topic where you'll send commands for controlling the humidifier.

## Usage

1. Power on your hacked humidifier and ensure the microcontroller is connected to the MQTT broker.

2. Use an MQTT client (web-based, mobile app, etc.) to send MQTT messages to the topic you subscribed to in the microcontroller.

3. Craft MQTT messages to send on/off commands to control the humidifier remotely.

## Contributing

Contributions to this project are welcome! If you find any issues or improvements, feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).
