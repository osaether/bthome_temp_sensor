BTHome Temperature Sensor
#########################

|license|

A BTHome v2 Bluetooth Low Energy temperature and humidity sensor for Nordic
nRF52840 and nRF54L15 development kits, built with the `nRF Connect SDK`_ and
`Zephyr`_. It supports TMP112, DS18B20 and BME680 sensors and is automatically
discoverable by `Home Assistant`_.

Overview
********

This Zephyr application broadcasts measurements as unencrypted `BTHome v2`_
service data. BTHome is an energy-efficient Bluetooth Low Energy (BLE)
advertising format supported by home automation platforms such as Home
Assistant. No BLE pairing or connection is required.

Supported hardware
==================

========================  =============  ========================
Sensor                    Interface      Measurements
========================  =============  ========================
Texas Instruments TMP112  I2C            Temperature (default)
Maxim DS18B20             1-Wire         Temperature
Bosch BME680              I2C            Temperature and humidity
========================  =============  ========================

All three sensor configurations support the Nordic nRF52840 DK and nRF54L15
DK.

Key features:

* Advertises measurements using the BTHome v2 format
* Works with Home Assistant's automatic BTHome discovery
* Low power consumption suitable for battery-powered applications
* Selects the attached sensor through Zephyr Kconfig

Quick Start
***********

After installing the nRF Connect SDK, clone this repository into the SDK
workspace and build the default TMP112 configuration:

.. code-block:: console

   west build -b nrf52840dk/nrf52840
   west flash

Choose another supported sensor by adding one of the following CMake options
to the build command:

* ``-- -DCONFIG_SENSOR_DS18B20=y``
* ``-- -DCONFIG_SENSOR_BME680=y``

See `Building`_ for every board and sensor combination. Review
`Sensor Connections`_ before powering the hardware.

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

* `nRF Connect SDK`_ v2.4.0 or later
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
======================

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
==============================

* Ensure BTHome integration is installed
* Check Bluetooth adapter range and interference
* Restart Home Assistant if discovery is delayed
* Check HA logs for BTHome-related errors

Build Errors
============

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
   ├── CONTRIBUTING.md          # Contribution guidelines
   ├── Kconfig                  # Configuration options
   ├── LICENSE                  # Apache 2.0 license (default)
   ├── LICENSES/
   │   └── LicenseRef-Nordic-5-Clause.txt  # Nordic board overlay license
   ├── prj.conf                 # Project configuration
   └── README.rst              # This file

Contributing
************

Bug reports, documentation improvements and support for additional Zephyr
boards or sensor drivers are welcome. See `CONTRIBUTING.md`_ for the expected
workflow and validation checklist.

License
*******

Except where otherwise noted, this project is licensed under the
`Apache License 2.0`_. The files in ``boards/`` are licensed under the
`Nordic Semiconductor 5-Clause License`_, as indicated by their SPDX license
identifiers.

.. _nRF Connect SDK: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/index.html
.. _Zephyr: https://www.zephyrproject.org/
.. _Home Assistant: https://www.home-assistant.io/integrations/bthome/
.. _BTHome v2: https://bthome.io/format/
.. _CONTRIBUTING.md: CONTRIBUTING.md
.. _Apache License 2.0: LICENSE
.. _Nordic Semiconductor 5-Clause License: LICENSES/LicenseRef-Nordic-5-Clause.txt
.. |license| image:: https://img.shields.io/badge/license-mixed-blue.svg
   :target: #license
   :alt: Mixed licenses

For more information about BTHome format, visit https://bthome.io
