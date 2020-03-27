# Nanosecond LED Flasher Shield
 
A nanosecond LED flasher which provides digital control of both pulse width and LED current. Designed to aid
photomultiplier detector characterisation.
 
![Flasher](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/img/Flasher.JPG)

The LED is driven by the new [LMG1025 from Texas Instruments](http://www.ti.com/product/LMG1025-Q1).
Designed for automotive LIDAR applications, this gate driver has typical rise and fall times of 650ps
and 850ps and can produce 5V pulse widths as short as 1.25ns.

The pulse width is controlled by a [Maxim Integrated DS1023S-25+](https://www.maximintegrated.com/en/products/analog/clock-generation-distribution/DS1023.html)
programmable timing element which can produce digitally-adjustable pulse widths of 0 to ~64ns in 0.25ns increments.

The LED current is set by five surface mount resistors connected in series, four of which can be shorted
out by [Toshiba TLP3475 photorelays](https://toshiba.semicon-storage.com/ap-en/semiconductor/product/optoelectronics/photorelay-mosfet-output/detail.TLP3475.html)
allowing the LED current to be adjusted digitally in 16 increments.

The shield also uses a [u-blox MAX-M8Q GNSS module](https://www.u-blox.com/en/product/max-m8-series) as a
signal generator to dictate the flash rate. The flash rate can be set to any frequency between 0.25Hz
and 10MHz.

The shield is designed to be mounted on a [5V ATmega328 Arduino Uno or similar](https://www.sparkfun.com/products/13975).
(Please note that the shield requires 5V I/O signals for correct operation and will not work if mounted
on a 3.3V Arduino Zero or similar.)

## Example Performance

In the image below:
- the yellow trace shows the anode voltage of a Bivar UV3TZ-400-15 UV LED being driven by the shield at maximum brightness (equivalent series resistance approx. 14 ohms) (1V per division)
- the green trace shows the corresponding fast output of a SensL J-series silicon photomultiplier (50mV per division)
- the timebase is 2 nsec per division

![max_brightness](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/img/max_brightness.png)

The overlapping yellow traces show the LED pulse width being increased in 0.25 nsec increments. The corresponding green traces show how the peak output of
the SiPM increases in line with the LED pulse width and the number of photons emitted during each pulse.

## Repository Contents

- **/Documentation** - Documentation for the hardware and software
- **/Hardware** - BOM, schematic and Eagle PCB design files
- **/Software** - Arduino code
- **LICENSE.md** contains the licence information

## Documentation

- [Hardware overview](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/Documentation/Hardware/README.md): an overview of the hardware
- [Software user guide](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/Documentation/Software/README.md): a user guide for the Arduino code

Enjoy!

**_Paul_**


