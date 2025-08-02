BTHome Temperature Sensor
#########################

Overview
********

This application implements a Bluetooth Low Energy (BLE) temperature sensor using the `BTHome <https://bthome.io>`_ format for data advertisement. BTHome is an energy-efficient format for transmitting sensor data over Bluetooth LE, compatible with popular home automation platforms like `Home Assistant <https://www.home-assistant.io/>`_.

The application supports three different temperature sensors:

* **Texas Instruments TMP112** - I2C digital temperature sensor (default)
* **Maxim DS18B20** - 1-Wire digital temperature sensor
* **Bosch BME680** - I2C environmental sensor (temperature + humidity)

Key features:

* Advertises temperature data every ~1 second using BTHome format
* Low power consumption suitable for battery-powered applications
* Supports both nRF52840 DK and nRF54L15 DK development boards
* Configurable sensor selection through Kconfig

Prerequisites
*************

Hardware Requirements
=====================

* Nordic nRF52840 DK or nRF54L15 DK development board
* One of the supported temperature sensors:
  
  * TMP112 (I2C address 0x48)
  * DS18B20 (1-Wire)
  * BME680 (I2C address 0x76)

* Breadboard and jumper wires for sensor connections
* BTHome compatible listener (e.g., Home Assistant with BTHome integration)

Software Requirements
=====================

* `nRF Connect SDK <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/index.html>`_ v2.4.0 or later
* West build tool
* nRF Connect for Desktop (for board configuration and flashing)

Sensor Connections
******************

TMP112 (I2C)
============

Connect to nRF52840 DK:

* VCC → 3.3V
* GND → GND  
* SDA → P0.26 (Arduino A4)
* SCL → P0.27 (Arduino A5)

DS18B20 (1-Wire)
================

Connect to nRF52840 DK:

* VCC → 3.3V
* GND → GND
* DATA → P1.02 (Arduino D1) with 4.7kΩ pull-up resistor to 3.3V

BME680 (I2C)
============

Connect to nRF52840 DK:

* VCC → 3.3V
* GND → GND
* SDA → P0.26 (Arduino A4)
* SCL → P0.27 (Arduino A5)

Building
********

Clone this repository under the samples folder in your nRF Connect SDK installation.

For nRF52840 DK
===============

TMP112 sensor (default):

.. code-block:: console

   west build -b nrf52840dk/nrf52840 -- -DCONFIG_SENSOR_TMP112=y

DS18B20 sensor:

.. code-block:: console

   west build -b nrf52840dk/nrf52840 -- -DCONFIG_SENSOR_DS18B20=y

BME680 sensor (temperature + humidity):

.. code-block:: console

   west build -b nrf52840dk/nrf52840 -- -DCONFIG_SENSOR_BME680=y

For nRF54L15 DK
===============

TMP112 sensor (default):

.. code-block:: console

   west build -b nrf54l15dk/nrf54l15/cpuapp -- -DCONFIG_SENSOR_TMP112=y

DS18B20 sensor:

.. code-block:: console

   west build -b nrf54l15dk/nrf54l15/cpuapp -- -DCONFIG_SENSOR_DS18B20=y

BME680 sensor (temperature + humidity):

.. code-block:: console

   west build -b nrf54l15dk/nrf54l15/cpuapp -- -DCONFIG_SENSOR_BME680=y

Flashing
********

After building, flash the application using west:

.. code-block:: console

   west flash

Or use nRF Connect Programmer from nRF Connect for Desktop.

Special Note for nRF54L15 DK
============================

Before flashing firmware to the nRF54L15 DK:

1. Use the `Board Configurator <https://docs.nordicsemi.com/bundle/nrf-connect-board-configurator/page/index.html>`_
2. Disable VCOM0 
3. Set VDD to 3.3V
4. Apply the configuration before flashing

Usage
*****

After flashing and powering on the device:

1. The device will start advertising as "BTHome sensor"
2. Temperature data is broadcast every ~1 second using BTHome format
3. For BME680, both temperature and humidity data are advertised

Console Output
==============

You can monitor the device operation via RTT or UART:

.. code-block:: console

   Starting BTHome temp sensor
   Bluetooth initialized
   Sensor initialized
   Temperature: 23.5°C
   [BME680 only] Humidity: 45.2%

Viewing Sensor Data
*******************

Home Assistant
==============

1. Ensure the BTHome integration is installed and configured
2. The sensor should be auto-discovered as "BTHome sensor"
3. Temperature (and humidity for BME680) will appear as sensor entities
4. Data updates approximately every second

Other BTHome Listeners
======================

Any application supporting BTHome format can receive the sensor data. The device advertises:

* Service UUID: 0xFCD2 (BTHome)
* Data format: BTHome v2 (unencrypted)
* Temperature: 0.01°C resolution
* Humidity: 0.01% resolution (BME680 only)

Troubleshooting
***************

Device Not Advertising
=======================

* Check that the sensor is properly connected and powered
* Verify the correct sensor configuration is selected
* Monitor console output for initialization errors
* Ensure the device is not already connected to another BLE central

Sensor Reading Issues
=====================

* **TMP112/BME680**: Check I2C connections and pull-up resistors
* **DS18B20**: Verify 1-Wire connection and 4.7kΩ pull-up resistor
* Check sensor power supply (3.3V)
* Verify sensor address matches overlay configuration

Home Assistant Not Discovering
===============================

* Ensure BTHome integration is installed
* Check Bluetooth adapter range and interference
* Restart Home Assistant if discovery is delayed
* Check HA logs for BTHome-related errors

Build Errors
=============

* Ensure nRF Connect SDK is properly installed
* Check that you're in the correct directory
* Verify the target board is supported
* Clean build directory: ``west build -t clean``

Project Structure
*****************

.. code-block:: text

   ├── boards/                    # Board-specific overlays
   │   ├── nrf52840dk_nrf52840.overlay
   │   └── nrf54l15dk_nrf54l15_cpuapp.overlay
   ├── include/
   │   └── sensor.h              # Sensor interface header
   ├── src/
   │   ├── main.c                # Main application and BLE advertising
   │   └── sensor.c              # Sensor initialization and reading
   ├── CMakeLists.txt           # Build configuration
   ├── Kconfig                  # Configuration options
   ├── prj.conf                 # Project configuration
   └── README.rst              # This file


For more information about BTHome format, visit https://bthome.io
