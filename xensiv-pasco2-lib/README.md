# XENSIV PAS CO2 Library (STM32 HAL)

This library provides a driver for the Infineon XENSIV PAS CO2 sensor.

## Folder Structure

- `Inc/pasco2.h`: Public API and types
- `Inc/xensiv_pasco2_regs.h`: Register map definitions
- `Src/pasco2.c`: I2C driver implementation

## Quick Start (I2C)

1. Add `Inc` to your compiler include paths.
2. Include `pasco2.h` in your application code.
3. Ensure your STM32 I2C peripheral is initialized before sensor init.
4. Initialize sensor context with `PASCO2_I2C_Init`.
5. Run `PASCO2_Sensor_init` once after power-up.
6. Start continuous mode using `PASCO2_Start_ContinousMode`.
7. Periodically call `PASCO2_ppm` or `PASCO2_ppm2` and read `dev.co2ppm`.

## Notes

- Sensor I2C address is `0x28` (7-bit), defined as `XENSIV_PASCO2_DEV_ADDR`.
- Measurement period for continuous mode is expected in seconds.
- Valid measurement rate range in this driver is defined by:
  - `XENSIV_PASCO2_MEAS_RATE_MIN` = 5
  - `XENSIV_PASCO2_MEAS_RATE_MAX` = 4095

## License

Register definitions in `xensiv_pasco2_regs.h` include Infineon copyright and Apache-2.0 SPDX header.
This was developed as part of learning. 
