# XENSIV PAS CO2 Library (STM32 HAL)

This library provides a driver for the Infineon XENSIV PAS CO2 sensor.

## Folder Structure

- `Inc/pasco2.h`: Public API and types
- `Inc/xensiv_pasco2_regs.h`: Register map definitions
- `Src/pasco2.c`: I2C driver implementation


## STM32CubeIDE Setup

1. Open your STM32CubeIDE project.
2. Inside the project, create a folder named `UserDefLib`.
3. Under `UserDefLib`, create two subfolders: `Inc` and `Src`.
4. Copy `pasco2.h` and `xensiv_pasco2_regs.h` into `UserDefLib/Inc`.
5. Copy `pasco2.c` into `UserDefLib/Src`.
6. Right-click the project and select `Properties`.
7. In `C/C++ General`, go to `Paths and Symbols`.
   - Under the `Includes` tab, add the path to `UserDefLib/Inc`.
   - Under the `Source Location` tab, add the path to `UserDefLib/Src`.
8. Click `Apply` and then `OK`.
9. Include `pasco2.h` in your application source file (for example `main.c`) to use the PASCO2 driver.
## Notes

- Sensor I2C address is `0x28` (7-bit), defined as `XENSIV_PASCO2_DEV_ADDR`.
- Measurement period for continuous mode is expected in seconds.
- Valid measurement rate range in this driver is defined by:
  - `XENSIV_PASCO2_MEAS_RATE_MIN` = 5
  - `XENSIV_PASCO2_MEAS_RATE_MAX` = 4095

## Examples

I will add in future

## License

Register definitions in `xensiv_pasco2_regs.h` include Infineon copyright and Apache-2.0 SPDX header.
This was developed as part of learning. 
