# Nanosecond LED Flasher Shield: Software Overview
 
The Arduino code for the nanosecond flasher is available [here](https://github.com/PaulZC/Nanosecond_LED_Flasher_Shield/tree/master/Software/LMG1025_Flasher).

Please ensure you select _Arduino Uno_ as the Board.

The code has been deliberately kept very simple. It begins by setting the MAX-M8Q time pulse pin to the desired frequency.
You will find definitions in the code for frequencies of 60Hz, 500Hz, 1kHz, 10kHz, 100kHz and 1MHz.

Also defined in the code are sequences of LED pulse brightnesses (currents) that can be used if desired. By default,
the brightness sequence is disabled. You can enable one by uncommenting _#define useSequence_ plus one of the
definitions for _sequence_.

The LED pulse width and current can be configured by sending a serial string to the Arduino, via the Serial Monitor.
The Serial baud rate should be set to 9600. Set the line ending to LF. The serial data format is:
- ASCII <                : 3C Hex
- Pulse Delay P7 (MSB)   : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P6         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P5         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P4         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P3         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P2         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P1         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P0 (LSB)   : ASCII 0 or 1 (30 or 31 Hex)
- LED R5                 : ASCII 0 or 1 (30 or 31 Hex) (0 = R is in circuit; 1 = R is shorted) (R5 is >> R2)
- LED R4                 : ASCII 0 or 1 (30 or 31 Hex) (0 = R is in circuit; 1 = R is shorted)
- LED R3                 : ASCII 0 or 1 (30 or 31 Hex) (0 = R is in circuit; 1 = R is shorted)
- LED R2                 : ASCII 0 or 1 (30 or 31 Hex) (0 = R is in circuit; 1 = R is shorted) (R2 is << R5)
- ASCII >                : 3E Hex
- LF                     : 0A Hex

A shorter message can be sent if you only want to change the pulse width:
- ASCII <                : 3C Hex
- Pulse Delay P7 (MSB)   : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P6         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P5         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P4         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P3         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P2         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P1         : ASCII 0 or 1 (30 or 31 Hex)
- Pulse Delay P0 (LSB)   : ASCII 0 or 1 (30 or 31 Hex)
- ASCII >                : 3E Hex
- LF                     : 0A Hex

The code will: parse the incoming serial data, check teh format is valid, set the DS1023 pulse width,
and the state of the four photorelays.