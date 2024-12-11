BTHome temperature sensor
#########################

This application implements a temperature sensor using the `Texas Instruments TMP112 <https://www.ti.com/product/TMP112>`_ or
the `Maxim DS18B20 <https://www.analog.com/en/products/ds18b20.html>`_ temperature sensors.
It advertises in the `BTHome <https://bthome.io>`_ format.

The sensor is configured to advertise every 1 seconds, and to send a notification with the current temperature.
The default sensor is the TMP112, but it can be changed to the DS18B20 by selecting the `CONFIG_SENSOR_DS18B20` Kconfig option.

Requirements
************

* A board with one of these sensors built in to its devicetree, or a devicetree overlay with such a node added.
  An overlay file for the nRF52840DK is provided in the boards folder.
  For information on how to connect and use these sensors see the TMP112 and DS18B20 samples in
  the `Zephyr Project <https://github.com/zephyrproject-rtos/zephyr>`_
* A BTHome compatible listener, for example `Home Assistant <https://www.home-assistant.io/>`_ with the BTHome integration running.


Building
********

You need the `nRF Connect SDK <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/index.html>`_ to build this sample.

Clone this repo under the samples folder in the nRF Connect SDK.
To build for the nRF52840 DK and the DS18B20 sensor run this west build command:

.. code-block:: console
  
   west build -b nrf52840dk/nrf52840 -- -DCONFIG_SENSOR_DS18B20=y

and to build for the nRF52840 DK and the TMP112 sensor run this west build command:

.. code-block:: console
  
   west build -b nrf52840dk/nrf52840 -- -DCONFIG_SENSOR_TMP112=y