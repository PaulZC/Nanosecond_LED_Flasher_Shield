# Nanosecond LED Flasher Shield: Hardware Overview
 
A nanosecond LED flasher which provides digital control of both pulse width and LED current. Designed to aid
photomultiplier detector characterisation.

![Top](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/img/Top.JPG)

![Bottom](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/img/Bottom.JPG)

![Dimensions](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/img/Dimensions.JPG)

The full schematic is available [here](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/Hardware/Schematic.pdf)

The LED is driven by the new [LMG1025 from Texas Instruments](http://www.ti.com/product/LMG1025-Q1).
Designed for automotive LIDAR applications, this gate driver has typical rise and fall times of 650ps
and 850ps and can produce 5V pulse widths as short as 1.25ns.

![DS1023](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/img/DS1023.JPG)

The functional block diagram for the LMG1025 is:

![LMG1025](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/img/LMG1025.JPG)

The LMG1025 is designed to drive a GaN FET to switch a LIDAR source, but with pull-up and pull-down
current limits of 7A and 5A it can of course drive a standard or high-brightness LED directly.

The truth table for the LMG1025 is:

![Truth Table](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/img/Truth.JPG)

It is important to note that OUTH is pulled up to 5V only when IN+ is high and IN- is low. In each of the other
three possible states, OUTL is pulled low. We can take advantage of this by connecting OUTH to the current-limiting
resistor and OUTL directly to the LED anode. This allows the LED to be turned off very quickly.

The Maxim Integrated DS1023S-25+ is a programmable timing element with two outputs: OUT and REF. The delay between IN and
OUT is digitally-programmable from 0ns to ~64ns in 256 0.25ns increments. There is also an additional inherrent "step zero"
delay between IN and OUT caused by the propagation delay through the chip. The REF output is delayed by a small fixed delay
of approximately 1.5ns plus the same "step zero" delay. By driving the LMG1025 IN+ and IN- from REF and OUT respectively,
the LED pulse can be adjusted from essentially zero pulse width (+/-0.125ns) to a maximum of 62.5ns (64ns - 1.5ns).
The LED turns on when REF goes high, and off again when OUT goes high.

Five resistors, connected in series, are connected between the LMG1025 OUTH pin and the LED anode. By shorting out four
of these resistors in 16 permutations, the LED current can be adjusted between minimum and maximum values dictated by
the selection of resistor values. The resistors are shorted out by TLP3475 photorelays which provide fast response times
and an on-resistance of approximately 1ohm.

For example: with resistances of 10, 18, 22, 27 and 33 ohms, with the 10 ohm resistor always in circuit, it is possible to achieve
an even spread of resistance values from 13.8 ohms to 110 ohms.

![Arduino](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/img/Arduino.JPG)

The ATmega328 Arduino: controls the LED pulse width, by setting the eight parallel lines which set the DS1023 delay;
the pulse brightness, by selecting which of the four photorelays are enabled; and the pulse frequency, by setting the
frequency of the MAX-M8Q GNSS module TIMEPULSE pin. Pulse frequencies of 0.25Hz to 10MHz are possible.

The Arduino pin allocation is:
- D0: reserved for Serial Rx
- D1: reserved for Serial Tx
- D2: software serial Rx - receives GNSS data from the MAX-M8Q
- D3: software serial Tx - transmits configuration messages to the MAX-M8Q
- D4: DS1023 Latch Enable
- D5: DS1023 delay bit 0
- D6: DS1023 delay bit 1
- D7: DS1023 delay bit 2
- D8: DS1023 delay bit 3
- D9: DS1023 delay bit 4
- D10: DS1023 delay bit 7
- D11: DS1023 delay bit 6
- D12: DS1023 delay bit 5
- D13: reserved for LED_BUILTIN (but can also be used to indicate the reception of the GNSS time pulses)
- A0/D14: the photorelay for R2
- A1/D15: the photorelay for R3
- A2/D16: the photorelay for R4
- A3/D17: the photorelay for R5
- A4/D18: the MAX-M8Q time pulse (to allow the LED current to be changed between LED pulses)
- A5/D19: enables / disables LED pulses via the 74HCT1G125 buffer

![GNSS](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/img/GNSS.JPG)

The time pulse signal produced by the MAX-M8Q GNSS receiver is used to trigger the DS1023. The frequency of the time pulse
signal can be defined by the Arduino code via software serial (D2 and D3). The LED flashes on the rising edge of the time pulse.
The time pulse signal is connected to the Arduino pin A4/D18 so that the LED current can be adjusted between pulses on the
falling edge of the signal. It is possible to change the current for adjacent LED pulses from one of the sixteen permutaions
to any other. The Arduino code can produce repetitive pulse patterns using any of the sixteen states.

![Level_Shift](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/blob/master/img/Level_Shift.JPG)

The MAX-M8Q is a 3.3V device. A 74HCT1G125 buffer is used to convert the 3.3V time pulses to 5V for the DS1023. The enable pin
of the buffer is connected to Arduino pin A5/D19 such that pulses can be skipped or disabled as required.





