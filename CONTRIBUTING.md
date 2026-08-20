# Contributing

Contributions that improve hardware support, Zephyr compatibility,
documentation, or BTHome interoperability are welcome.

## Before opening a change

1. Create a focused branch from the latest `main` branch.
2. Keep source changes compatible with the project's supported nRF52840 DK and
   nRF54L15 DK targets where possible.
3. Use the existing Zephyr APIs and follow the style of the surrounding code.
4. Update `README.rst`, Kconfig, and board overlays when behavior, hardware
   support, or configuration changes.

## Building and testing

Build the configurations affected by your change. For example:

```console
west build -p always -b nrf52840dk/nrf52840 -- -DCONFIG_SENSOR_TMP112=y
west build -p always -b nrf52840dk/nrf52840 -- -DCONFIG_SENSOR_DS18B20=y
west build -p always -b nrf52840dk/nrf52840 -- -DCONFIG_SENSOR_BME680=y
```

For nRF54L15 DK testing, replace the board target with
`nrf54l15dk/nrf54l15/cpuapp`. Hardware-facing changes should be flashed and
checked with the relevant physical sensor when practical.

In a pull request, state:

- which board and sensor configurations were built;
- which configurations were tested on hardware;
- any configurations that could not be tested;
- whether the BTHome advertisement was verified with Home Assistant or another
  BTHome v2 listener.

## Reporting bugs

Please include the nRF Connect SDK version, board, sensor, build command,
relevant console output, and steps to reproduce the problem. For sensor-reading
issues, also describe the wiring and power supply.

## Proposing support for new hardware

Explain the use case and identify the corresponding Zephyr board or sensor
driver. Prefer additions that reuse upstream Zephyr drivers rather than adding
project-specific device drivers.
