// Code for the LMG1025 Flasher Shield (V2) for the SparkFun RedBoard Qwiic (5V) DEV-15123

// The flash pulse width is set by a Maxim DS1023-25 Programmable Timing Element (Delay Line)

// The LED current is controlled by setting A0-A3 high/low.
// High will short the corresponding resistor to increase the LED current.

// The MAX-M8Q is connected to D2/D3 for Software Serial.
// The timepulse pin sets the flash rate.

// Serial data is 9600 Baud
// The serial data format is:
//
// ASCII <                : 3C Hex
// Pulse Delay P7 (MSB)   : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P6         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P5         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P4         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P3         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P2         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P1         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P0 (LSB)   : ASCII 0 or 1 (30 or 31 Hex)
// LED R5                 : ASCII 0 or 1 (30 or 31 Hex) (0 = R is in circuit; 1 = R is shorted) (R5 is >> R2)
// LED R4                 : ASCII 0 or 1 (30 or 31 Hex) (0 = R is in circuit; 1 = R is shorted)
// LED R3                 : ASCII 0 or 1 (30 or 31 Hex) (0 = R is in circuit; 1 = R is shorted)
// LED R2                 : ASCII 0 or 1 (30 or 31 Hex) (0 = R is in circuit; 1 = R is shorted) (R2 is << R5)
// ASCII >                : 3E Hex
// LF                     : 0A Hex
//
// 15 bytes total
//
// - or -
//
// ASCII <                : 3C Hex
// Pulse Delay P7 (MSB)   : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P6         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P5         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P4         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P3         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P2         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P1         : ASCII 0 or 1 (30 or 31 Hex)
// Pulse Delay P0 (LSB)   : ASCII 0 or 1 (30 or 31 Hex)
// ASCII >                : 3E Hex
// LF                     : 0A Hex
//
// 11 bytes total
//
//
// The code can be configured to automatically loop through a pre-defined sequence of LED currents while keeping
// the pulse width constant.

//#define useSequence // Uncomment this line to enable a sequence of LED currents
// Then uncomment _one_ sequence definition:
// All 16 states from minimum brightness (B0000) to maximum brightness (B1111)
const byte seqLength = 16; const byte sequence[16] = {B0000, B0001, B0010, B0011, B0100, B0101, B0110, B0111, B1000, B1001, B1010, B1011, B1100, B1101, B1110, B1111};
// 5 states: shorting out none or one of the four resistors in turn ('dim')
//const byte seqLength = 5; const byte sequence[5] = {B0000, B0001, B0010, B0100, B1000};
// 5 states: shorting out all or three of the four resistors in turn ('bright')
//const byte seqLength = 5; const byte sequence[5] = {B1111, B1110, B1101, B1011, B0111};
// 5 states: maximum to minimum resistance in four ~equal steps (assumes R1-R5 are: 10/18/22/27/33) (Rs are: 110,89,62.9,39.9,13.8 assuming 1R for the photorelays)
//const byte seqLength = 5; const byte sequence[5] = {B0000, B0010, B0110, B1011, B1111};
// 4 states: shorting out none or R2/R3 in turn (the four dimmest combinations)
//const byte seqLength = 4; const byte sequence[4] = {B0000, B0001, B0010, B0011};
// 4 states: shorting out R4/R5 and R2/R3 in turn (the four brightest combinations)
//const byte seqLength = 4; const byte sequence[4] = {B1111, B1110, B1101, B1100};
// 2 states: shorting out none or all of the resistors in turn (dimmest and brightest)
//const byte seqLength = 2; const byte sequence[4] = {B0000, B1111};

static byte seqStep = 0; // Used to store the sequence step (0 : seqLength-1)

#define BUFFER_LENGTH 15 // Serial buffer length
#define SHORT_MESSAGE 4 // A short message is this much sorter than BUFFER_LENGTH
static unsigned char RX_BUFFER[BUFFER_LENGTH]; // Serial buffer

#define TRIG_BUFFER_LENGTH 4
#define HALF_TRIG_BUFFER_LENGTH 2
byte TRIG_BUFFER[TRIG_BUFFER_LENGTH]; // Buffer to store the state of TRIG_IN for edge detection

// Maxim DS1023-25 Pin Definitions:
#define DELAY0 5    // P0 (LSB) : D5      : Delay Bit 0
#define DELAY1 6    // P1       : D6      : Delay Bit 1
#define DELAY2 7    // P2       : D7      : Delay Bit 2
#define DELAY3 8    // P3       : D8      : Delay Bit 3
#define DELAY4 9    // P4       : D9      : Delay Bit 4
#define DELAY5 12   // P5       : D12     : Delay Bit 5
#define DELAY6 11   // P6       : D11     : Delay Bit 6
#define DELAY7 10   // P7 (MSB) : D10     : Delay Bit 7
#define TRIG_EN 19  // 74AHCT1G125 Enable (Active Low) : A5/D19
#define DELAY_LE 4  // LE       : D4      : Normally high; low latches the delay
#define TRIG_IN 18  // A4/D18   : Trigger signal from the MAX-M8Q (used to control the flash rate and current sequence)
//#define TRIG_OUT 13 // D13 : Used to confirm that TRIG_IN is being received correctly (comment out if not required)

// LED Resistor Control: HIGH = R is in circuit; LOW = R is shorted
#define LED_R5  17 // LED R5   : A3/D17
#define LED_R4  16 // LED R4   : A2/D16
#define LED_R3  15 // LED R3   : A1/D15
#define LED_R2  14 // LED R2   : A0/D14

// Software Serial for the MAX-M8Q GNSS
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX, TX
//#define echoGNSS // Uncomment this line to echo the GNSS data to Serial
#define endSerial // Uncomment this to disable the software serial after the GNSS has been configured (overrides echoGNSS)

// Update an output pin
void update_pin(int pin, unsigned char val) {
  if (pin >= 0) {
    bool state = val & 0x01;
    digitalWrite(pin, state);
  }
}

// Update an output pin inverted
void update_pin_i(int pin, unsigned char val) {
  if (pin >= 0) {
    bool state = val & 0x01;
    digitalWrite(pin, !state);
  }
}

// Set UART1 to UBX only (disable the NMEA noise)
static const uint8_t setUART1[] = {
  0xb5, 0x62, 0x06, 0x00, 0x14, 0x00,
  0x01, 0x00, 0x00, 0x00, 0xC0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00,
  0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
  };
static const int len_setUART1 = 26;

// Configure Time Pulse - set freqPeriod and freqPeriodLock to 60Hz (0000003C)
static const uint8_t setTimePulse_60Hz[] = {
  0xb5, 0x62, 0x06, 0x31, 0x20, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00
  };
// Configure Time Pulse - set freqPeriod and freqPeriodLock to 500Hz (000001F4)
static const uint8_t setTimePulse_500Hz[] = {
  0xb5, 0x62, 0x06, 0x31, 0x20, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0xF4, 0x01, 0x00, 0x00, 0xF4, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00
  };
// Configure Time Pulse - set freqPeriod and freqPeriodLock to 1000Hz (000003E8)
static const uint8_t setTimePulse_1kHz[] = {
  0xb5, 0x62, 0x06, 0x31, 0x20, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00
  };
// Configure Time Pulse - set freqPeriod and freqPeriodLock to 10000Hz (00002710)
static const uint8_t setTimePulse_10kHz[] = {
  0xb5, 0x62, 0x06, 0x31, 0x20, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00
  };
// Configure Time Pulse - set freqPeriod and freqPeriodLock to 100000Hz (000186A0)
static const uint8_t setTimePulse_100kHz[] = {
  0xb5, 0x62, 0x06, 0x31, 0x20, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0xA0, 0x86, 0x01, 0x00, 0xA0, 0x86, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00
  };
// Configure Time Pulse - set freqPeriod and freqPeriodLock to 1000000Hz (000F4240)
static const uint8_t setTimePulse_1MHz[] = {
  0xb5, 0x62, 0x06, 0x31, 0x20, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x42, 0x0F, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00
  };
static const int len_setTimePulse = 38;

// Set Nav Mode to Portable
static const uint8_t setNavPortable[] = {
  0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 
  0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Set Nav Mode to Pedestrian
static const uint8_t setNavPedestrian[] = {
  0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 
  0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Set Nav Mode to Airborne <1G
static const uint8_t setNavAir[] = {
  0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 
  0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const int len_setNav = 42;

// Set GNSS Config to GPS + Galileo + GLONASS + SBAS (Causes the M8 to restart!)
static const uint8_t setGNSS[] = {
  0xb5, 0x62, 0x06, 0x3e, 0x3c, 0x00,
  0x00, 0x20, 0x20, 0x07,
  0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01,
  0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01,
  0x02, 0x04, 0x08, 0x00, 0x01, 0x00, 0x01, 0x01,
  0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01,
  0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x03,
  0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x05,
  0x06, 0x08, 0x0e, 0x00, 0x01, 0x00, 0x01, 0x01 };
static const int len_setGNSS = 66;

// Send message in u-blox UBX format
// Calculates and appends the two checksum bytes
// Doesn't add the 0xb5 and 0x62 sync chars (these need to be included at the start of the message)
void sendUBX(const uint8_t *message, const int len) {
  int csum1 = 0; // Checksum bytes
  int csum2 = 0;
  for (int i=0; i<len; i++) { // For each byte in the message
    mySerial.write(message[i]); // Write the byte
    if (i >= 2) { // Don't include the sync chars in the checksum
      csum1 = csum1 + message[i]; // Update the checksum bytes
      csum2 = csum2 + csum1;
    }
  }
  csum1 = csum1 & 0xff; // Limit checksums to 8-bits
  csum2 = csum2 & 0xff;
  mySerial.write((uint8_t)csum1); // Send the checksum bytes
  mySerial.write((uint8_t)csum2);
}

void setup() {

  pinMode(TRIG_EN, OUTPUT); digitalWrite(TRIG_EN, LOW); // Enable the MAX-M8Q trigger pulses via the 74AHCT1G125

  // Set delay latch to output; set high
  pinMode(DELAY_LE, OUTPUT); digitalWrite(DELAY_LE, HIGH);
  delay(10);

  // Set delay pins to outputs; set default delay of 255 * 0.25nsec = ~64nsec
  pinMode(DELAY0, OUTPUT); update_pin(DELAY0, 1);
  pinMode(DELAY1, OUTPUT); update_pin(DELAY1, 1);
  pinMode(DELAY2, OUTPUT); update_pin(DELAY2, 1);
  pinMode(DELAY3, OUTPUT); update_pin(DELAY3, 1);
  pinMode(DELAY4, OUTPUT); update_pin(DELAY4, 1);
  pinMode(DELAY5, OUTPUT); update_pin(DELAY5, 1);
  pinMode(DELAY6, OUTPUT); update_pin(DELAY6, 1);
  pinMode(DELAY7, OUTPUT); update_pin(DELAY7, 1);
  delay(10);

  // Set delay latch low
  digitalWrite(DELAY_LE, LOW);

  // Set LED resistor pins to outputs; leave resistors unshorted (minimum LED current)
  pinMode(LED_R5, OUTPUT); update_pin(LED_R5, 0);
  pinMode(LED_R4, OUTPUT); update_pin(LED_R4, 0);
  pinMode(LED_R3, OUTPUT); update_pin(LED_R3, 0);
  pinMode(LED_R2, OUTPUT); update_pin(LED_R2, 0);

  // Trigger input and output (if required)
  pinMode(TRIG_IN, INPUT);
#ifdef TRIG_OUT
  pinMode(TRIG_OUT, OUTPUT); update_pin(TRIG_OUT, 0);
#endif

  // Initialise the serial ports
  Serial.begin(9600); // Start the console
  mySerial.begin(9600); // Start the GNSS serial

  delay(1000); // Wait

  Serial.println(F("LMG1025 Flasher")); // Send welcome message

  // Configure MAX-M8Q GNSS
#ifndef echoGNSS
  sendUBX(setUART1, len_setUART1); // Set UART1 to UBX protocol only (disable the NMEA noise for software serial)
  delay(100);
#endif
  sendUBX(setNavAir, len_setNav); // Set Airborne <1G Navigation Mode
  delay(100);
  //sendUBX(setTimePulse_60Hz, len_setTimePulse); // Set Time Pulse configuration
  //sendUBX(setTimePulse_500Hz, len_setTimePulse); // Set Time Pulse configuration
  sendUBX(setTimePulse_1kHz, len_setTimePulse); // Set Time Pulse configuration
  //sendUBX(setTimePulse_10kHz, len_setTimePulse); // Set Time Pulse configuration
  //sendUBX(setTimePulse_100kHz, len_setTimePulse); // Set Time Pulse configuration
  //sendUBX(setTimePulse_1MHz, len_setTimePulse); // Set Time Pulse configuration
  delay(100);
  sendUBX(setGNSS, len_setGNSS); // Set GNSS - causes M8 to restart!
  delay(3100); // Wait for GNSS to restart

#ifdef endSerial
  mySerial.end(); // End the software serial (to improve TRIG_IN edge detection by disabling software serial)
#endif
}

void loop() {
  unsigned char c = 0;
  
#ifdef echoGNSS
#ifndef endSerial
  // Echo the GNSS serial data if required
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
#endif
#endif

  // Check for a new serial character from the console
  if (Serial.available()) {
    c = Serial.read();
  }

  // If we got a new character:
  if (c > 0) {
    for (int i = 1; i < BUFFER_LENGTH; i++) {
      RX_BUFFER[i-1] = RX_BUFFER[i]; // Shuffle the buffer data along by one
    }
    RX_BUFFER[BUFFER_LENGTH - 1] = c; // Store the new character at the end of the buffer
    // Check for correctly formatted message (long format)
    if ((RX_BUFFER[0] == 0x3c) and (RX_BUFFER[BUFFER_LENGTH - 2] == 0x3e) and (RX_BUFFER[BUFFER_LENGTH - 1] == 0x0a)) {
      // Message received so update pins
      update_pin(DELAY7, RX_BUFFER[1]);
      update_pin(DELAY6, RX_BUFFER[2]);
      update_pin(DELAY5, RX_BUFFER[3]);
      update_pin(DELAY4, RX_BUFFER[4]);
      update_pin(DELAY3, RX_BUFFER[5]);
      update_pin(DELAY2, RX_BUFFER[6]);
      update_pin(DELAY1, RX_BUFFER[7]);
      update_pin(DELAY0, RX_BUFFER[8]);
      // Latch new delay value
      delay(10);
      digitalWrite(DELAY_LE, HIGH);
      delay(10);
      digitalWrite(DELAY_LE, LOW);
      delay(10);
      // Update the LED current
      update_pin(LED_R5, RX_BUFFER[9]); // Assumes R5 >> R2
      update_pin(LED_R4, RX_BUFFER[10]);
      update_pin(LED_R3, RX_BUFFER[11]);
      update_pin(LED_R2, RX_BUFFER[12]);
#ifndef echoGNSS
      // Echo the settings
      for (int i = 0; i < BUFFER_LENGTH; i++) {
        Serial.write(RX_BUFFER[i]);
      }
      Serial.println(F("OK"));
#endif
    }
    // Check for correctly formatted message (short format)
    else if ((RX_BUFFER[SHORT_MESSAGE] == 0x3c) and (RX_BUFFER[BUFFER_LENGTH - 2] == 0x3e) and (RX_BUFFER[BUFFER_LENGTH - 1] == 0x0a)) {
      // Message received so update pins
      update_pin(DELAY7, RX_BUFFER[SHORT_MESSAGE+1]);
      update_pin(DELAY6, RX_BUFFER[SHORT_MESSAGE+2]);
      update_pin(DELAY5, RX_BUFFER[SHORT_MESSAGE+3]);
      update_pin(DELAY4, RX_BUFFER[SHORT_MESSAGE+4]);
      update_pin(DELAY3, RX_BUFFER[SHORT_MESSAGE+5]);
      update_pin(DELAY2, RX_BUFFER[SHORT_MESSAGE+6]);
      update_pin(DELAY1, RX_BUFFER[SHORT_MESSAGE+7]);
      update_pin(DELAY0, RX_BUFFER[SHORT_MESSAGE+8]);
      // Latch new delay value
      delay(10);
      digitalWrite(DELAY_LE, HIGH);
      delay(10);
      digitalWrite(DELAY_LE, LOW);
      delay(10);
#ifndef echoGNSS
      // Echo the settings
      for (int i = 4; i < BUFFER_LENGTH; i++) {
        Serial.write(RX_BUFFER[i]);
      }
      Serial.println(F("OK"));
#endif
    }
  }

#ifdef useSequence // Sequence the LED currents if required
  // Read and store TRIG_IN
  for (int i = 1; i < TRIG_BUFFER_LENGTH; i++) {
    TRIG_BUFFER[i-1] = TRIG_BUFFER[i]; // Shuffle the buffer data along by one
  }
  TRIG_BUFFER[TRIG_BUFFER_LENGTH - 1] = digitalRead(TRIG_IN); // Store the state of TRIG_IN

  // Check if we have seen a falling edge on TRIG_IN
  bool edgeSeen = true;
  for (int i = 0; i < HALF_TRIG_BUFFER_LENGTH; i++) {
    if (TRIG_BUFFER[i] == 0) edgeSeen = false; // Check the first half of the buffer is high
  }
  for (int i = HALF_TRIG_BUFFER_LENGTH; i < TRIG_BUFFER_LENGTH; i++) {
    if (TRIG_BUFFER[i] == 1) edgeSeen = false; // Check the second half of the buffer is low
  }

  // If we have seen a falling edge:
  if (edgeSeen == true)
  {
#ifdef TRIG_OUT // Invert TRIG_OUT if required to show edge was detected (TRIG_OUT will clock at half the frequency of TRIG_IN)
    digitalWrite(TRIG_OUT, !digitalRead(TRIG_OUT));
#endif
    if (seqLength > 0) // Redundant?
    {
      byte shortRs = sequence[seqStep]; // Read the next step from the sequence
      // Apply the sequence step: update the LED current
      update_pin(LED_R5, ((shortRs & B1000) >> 3)); // Assumes R5 >> R2
      update_pin(LED_R4, ((shortRs & B0100) >> 2));
      update_pin(LED_R3, ((shortRs & B0010) >> 1));
      update_pin(LED_R2, (shortRs & B0001));
      seqStep++; // Increment seqStep
      if (seqStep >= seqLength) seqStep = 0; // Reset seqStep if required
    }
  }
#endif
}
