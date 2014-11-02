# Arduino Scales

More information available here.

## Theory

The idea was to modify existing bathroom scales to output its result to the internet. The scales themselves should still work as before. 

## PCB

The PCB uses an XRF radio, along with a MCP3421 18bit ADC, the Wheatstone bridge made up of 4 strain gauges can be measured accurately (~90g per step).

A single gate 74AHC inverter takes the LCD backlight signal and uses it to wake the Atmega328 chip from deep sleep.

## Parts list

- Various 0805 resistors and capacitors
- **SMD crystal** - 16 Mhz TXC 7B-16.000MAAJ-T - Farnell no: 1841977
- **ATMEGA328P-AU** - TQFP-32 package - Farnell no: 1715486
- **Microchip MCP3421A1T-E/CH** - Farnell no: 1605568
- **NXP 74AHC1G04GW/T1** - Single gate SOT-353 inverter - Farnell no: 1085239
- **TI TPS73133DBVTG4** - 0.15A 3.3v voltage regulator - Farnell no: 1207341