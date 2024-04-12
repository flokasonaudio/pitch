/*******************************************************************************

  PitchBox98/13 User Interface 20

  PitchUI20 Arduino Sketch on Arduino Nano module with ATmega328P MCU

  Remote I2C Display, Configuration and Control for PitchBox
  emulating multiple slave devices using TWI masking register.
  Remote control and configuration by serial MIDI-interface.
  Debug control by serial monitor.

  created   : version 0.10  27.12.2019  by Jvo Studer
  released  : version 0.90
  modified  : version 0.20  24. 1.2020  by Jvo Studer

JSL Engineering, Brunnmattstrasse 32, CH-4500 Solothurn, vinyljvo@gmail.com
(c) 2020  JSL Engineering, all rights reserved

*******************************************************************************/

// TWI-I2C Library for PitchBox Interface
#include <Wire.h>
// SoftWire Library for Adafruit 7-Seg Display
#include <AsyncDelay.h>
#include <SoftWire.h>

// PitchBox98 TWI/I2C Slave Addresses
#define ADDR_DISP 0x38  // SAA1064 4-digit 7-seg display
#define ADDR_CNFG 0x21  // PCF8574 configuration port
#define ADDR_CTRL 0x24  // PCF8574 remote control port
#define ADDR_AUX  0x26  // PCF8574 auxillary IO-port
// Adafruit 4-Digit 7-Segment Display Slave
#define AF7S_ADDR 0x70  // 7-Segment display address
#define AF7S_SYSOSC   0x21  // System setup oscillator ON
#define AF7S_DISPSET  0x81  // Display ON, blink off
#define AF7S_DIMMSET  0xE0  // Display brightness 0-15

 // Default Pitch-Config: ALGO_FAST, ECO_ON, WH2_ON, WHM_1, TM_2
 #define CFG_DFLT  B10001001

// Pitch Display Control: Digits on <C2:C1>, Dynamic mode <C0>
#define SAA1064_LO B00010111 //low     3mA I_seg
#define SAA1064_OP B00100111 //normal  6mA I_seg
#define SAA1064_HI B00110111 //hilight 9mA I_seg
// 7-Segment Display Brightness
#define AF7S_DIM_LO  0  // low (idle) brightness
#define AF7S_DIM_OP  7  // normal (operation) brighthness
#define AF7S_DIM_HI 14  // highlight brightness

// Remote control source
#define SERIAL_CTRL 1
#define MAN_CTRL    2
// Control constants
#define EMPTY 0


// SAA1064 Display Instruction Byte
#define CONTROL 0x00
#define DIGIT1  0x01
#define DIGIT2  0x02
#define DIGIT3  0x03
#define DIGIT4  0x04

// Pitch 7-segment LED display definitions
#define S7_dp 7  //          digit decimal point
#define S7_a  6  //    aa
#define S7_b  5  //   f  b
#define S7_c  4  //   f  b
#define S7_d  3  //    gg
#define S7_e  2  //   e  c
#define S7_f  1  //   e  c
#define S7_g  0  //    dd  dp

// 7-segment LED character definitions
#define D7_0 B01111110    //  0x7E 0
#define D7_1 B00110000    //  0x30 1
#define D7_2 B01101101    //  0x6D 2
#define D7_3 B01111001    //  0x79 3
#define D7_4 B00110011    //  0x33 4
#define D7_5 B01011011    //  0x5B 5
#define D7_6 B01011111    //  0x5F 6
#define D7_7 B01110000    //  0x70 7
#define D7_8 B01111111    //  0x7F 8
#define D7_9 B01111011    //  0x7B 9

#define D7_A B01110111    //  A
#define D7_b B00011111    //  b
#define D7_C B01001110    //  C
#define D7_d B00111101    //  d
#define D7_E B01001111    //  E
#define D7_F B01000111    //  F
#define D7_G B01011110    //  G
#define D7_H B00110111    //  H
#define D7_I B00000110    //  I
#define D7_J B00111100    //  J
#define D7_K B01010111    //  K
#define D7_L B00001110    //  L
#define D7_M B01110110    //  M
#define D7_n B00010101    //  n
#define D7_o B00011101    //  o
#define D7_P B01100111    //  P
#define D7_q B01110011    //  q
#define D7_r B00000101    //  r
#define D7_S B01011011    //  S (5)
#define D7_t B00001111    //  t
#define D7_U B00111110    //  U
#define D7_v B00011100    //  v (u)
#define D7_W B00111110    //  W (U)
#define D7_X B00110111    //  X (H)
#define D7_y B00111011    //  y
#define D7_Z B01101101    //  Z (2)

#define D7_c B00001101    //  c
#define D7_h B00010111    //  h
#define D7_i B00000100    //  i
#define D7_O D7_0         //  O (0)
#define D7_T B01000110    //  T
#define D7_u D7_v         //  u (v)

#define D7_  B00000000    //    space (empty)
#define D7_excl B10110000 //  ! exclamation mark
#define D7_quot B00100010 //  " quotation mark (seconds symbol)
#define D7_apo B00000010  //  ' apostroph (minute symbol)
#define D7_neg B00000001  //  - minus
#define D7_dot B10000000  //  . point
#define D7_equ B00001001  //  = equal
#define D7_ques B11100101 //  ? question mark
#define D7_at   B01101111 //  @ at
#define D7__ B00001000    //  _ underline
#define D7_ovr B01000000  //  overline
#define D7_r1 B01001000   //  revs_1
#define D7_r2 B00100100   //  revs_2
#define D7_r3 B00010010   //  revs_3

const byte SRfontArray[] PROGMEM = {
D7_0, D7_1, D7_2, D7_3, D7_4, D7_5, D7_6, D7_7, D7_8, D7_9,
D7_A, D7_b, D7_C, D7_d, D7_E, D7_F, D7_G, D7_H, D7_I, D7_J, D7_K, D7_L, D7_M,
D7_n, D7_o, D7_P, D7_q, D7_r, D7_S, D7_t, D7_U, D7_v, D7_W, D7_X, D7_y, D7_Z,
D7_c, D7_h, D7_i, D7_O, D7_T, D7_u,
D7_, D7_excl, D7_quot, D7_apo, D7_neg, D7_dot, D7_equ, D7_ques, D7_at,
D7__, D7_ovr, D7_r1, D7_r2, D7_r3
};
const char SRfontCHAR[] PROGMEM = {
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'A',  'b',  'C',  'd',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',
'n',  'o',  'P',  'q',  'r',  'S',  't',  'U',  'v',  'W',  'X',  'y',  'Z',
'c',  'h',  'i',  'O',  'T',  'u',
' ',  '!', '"',  "'",  '-',  '.',  '=',  '?',  '@',
'_',  '1',  '2',  '3'
};

// Configuration I2C address 0x21:33DEC:'!'
// 7 ShiftKEY  / 6A DIA / 6B ALGO / 5A ECHO / 5B ECO
// 4A SFTYGRV / 4B WH2ND / 3.2 MINWIDTH / 1.0 MARKTIME
// User configuration IO-port switches, keys & selectors
/*
Ix_2ND equ 7 ; // 2nd keys (shift) function B10000000
Ix_DIAM equ 6 ; // Diameter Selector 12",10",7" B01000000
Ix_ECHO equ 5 ; // Suppress Echo (Increased Land) B00100000
Ix_SGR equ 4 ; // Safety Groove (on/off) B00010000
IxALG equ 6 ; // algorithm (fast/relaxed) B11000000
IxECO equ 5 ; // economy (normal/off) B10100000
IxWH2 equ 4 ; // 2nd increased lead-in & -out depth (on/off) B10010000
IsWM1 equ 3 ; // minimum width (40um,50um,60um,80um) B00001000
IsWM0 equ 2 ; // (see config file for values) B00000100
IsTM1 equ 1 ; // mark time (0.6s,0.8s,1.0s,1.3s) B00000010
IsTM0 equ 0 ; // (see config file for values) B00000001
*/

// Remote Control I2C Address 0x24:'$'
#define CTRL_IDLE   B11111111
#define CTRL_STOP   B01111111
#define CTRL_START  B10111111
#define CTRL_MARK   B11011111
#define CTRL_FAST   B11101111
#define CTRL_ECHO   B11111101
#define CTRL_MODE   B11111110   // Remain Time | LPmm/LPI | Average Pitch
#define LED_STOP    B11110111
#define LED_REC     B11111011

// Pins
#define KEY_PIN 2     // Digital IN manual key (debug)
SoftWire swI2C(6, 7); // Digital IO Pin D6 & D7

// Global Memory
byte  pitchConfig = CFG_DFLT;
byte  digitData[4] = { D7_P, D7_I, D7_2, D7_0 }; // Display ID "PI20"
bool  updateDisplay = false;
uint8_t af7sErrors = 0;
uint8_t af7sBright = 15; // full brightness
char  remoteCommand;
bool  keyState;
uint8_t idleCount = 0;

// Setup -----------------------------------------------------------------------

void setup()
{
  // Standard Serial interface via cu.wchusbserial USB to RS-232 chip
  Serial.begin(115200);
  // Pitch I2C/TWI interface address & event handler setup
  byte SlaveAddress = (ADDR_CTRL | ADDR_CNFG | ADDR_DISP);
//  byte SlaveAddress = (ADDR_CTRL | ADDR_CNFG);
//  byte SlaveAddress = ADDR_CTRL;
  byte SlaveMask = (ADDR_CTRL ^ ADDR_CNFG) | (ADDR_CNFG ^ ADDR_DISP);
//  byte SlaveMask = (ADDR_CTRL ^ ADDR_CNFG);
//  byte SlaveMask = 0;
  Wire.begin(SlaveAddress);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  // set TWI address mask register for multiple slave devices
  TWAMR = SlaveMask << 1;

  Serial.println(F("\nPitchBox User Interface PitchUI20"));

  // Adafruit Display SoftWire interface setup
  swI2C.begin();
  swI2C.setTimeout_ms(5);
  swI2C.setClock(50000);
  af7sErrors = init7SegDisplay(AF7S_ADDR, af7sBright);
  // Display ID
  af7sErrors += write7SegDisplay(AF7S_ADDR, digitData);
  if (af7sErrors) {
    Serial.print(F("\nAdafruit Display Errors: "));
    Serial.println(af7sErrors);
  }

  // Inputs
  pinMode(KEY_PIN, INPUT_PULLUP);
  // Onboard LED
  pinMode(LED_BUILTIN, OUTPUT);
  
  // init vars
  keyState = HIGH;
  remoteCommand = 0;

  Serial.print(F("TWI-I2C Address: 0x")); Serial.print(TWAR >> 1, HEX);
  Serial.print(F(" ")); Serial.println(TWAR, BIN);

  Serial.print(F("TWI-I2C Mask:    0x")); Serial.print(TWAMR, HEX);
  Serial.print(F(" ")); Serial.println(TWAMR >> 1, BIN);

  Serial.print(F("Remote Control: s START, p STOP, m MARK, f FAST, "));
  Serial.println(F("l ECHO, d Display MODE"));
  Serial.print(F("Configuration: A/a ALGO F|S, E/e ECO 1|0, N/n W2ND 1|0; "));
  Serial.println(F("D DIA, k ECHO, g SFG"));

} // end setup

// Main loop -------------------------------------------------------------------

void loop()
{
    if (updateDisplay) {
      af7sErrors = write7SegDisplay(AF7S_ADDR, digitData);
      if (! af7sErrors) updateDisplay = false;
    }

    // check input key
    if (digitalRead(KEY_PIN) == LOW) {
      digitalWrite(LED_BUILTIN, HIGH); // Light LED to indicate active command
      sendInterfaceCommand(MAN_CTRL);
    }
    // check serial data
    else if (Serial.available() > 0) {
      digitalWrite(LED_BUILTIN, HIGH); // Light LED to indicate active command
      sendInterfaceCommand(SERIAL_CTRL);
    }
    else if (remoteCommand == EMPTY) {
      digitalWrite(LED_BUILTIN, LOW); // LED off if no more interface activity
    }
    
} // end main loop

// -----------------------------------------------------------------------------

void sendInterfaceCommand(byte controlSource)
{
  if (remoteCommand == EMPTY) {
    if (controlSource == MAN_CTRL) {
      remoteCommand = 'k';
    }
    else if (controlSource == SERIAL_CTRL) {
      remoteCommand = Serial.read();
    }
    else {
      Serial.println(F("Unknown command source"));
    }
  }
} // end sendInterfaceCommand


// handle RXD Event: incoming I2C slave data +++++++++++++++++++++++++++++++++++
// SAA1064 7-Segment Display Data

void receiveEvent(int numBytes)
{
    byte instructionByte;
    byte controlByte = 0;

  // S SAA1064-Adress + Write | InstructionByte A | ControlByte A |
  // Digit1 A ... Digit4 A P

  // Instruction Byte 00000.SC.SB.SA
  //             000     0 Control Register
  //             001     1 Digit1 
  //             010     2 Digit2
  //             011     3 Digit3
  //             100     4 Digit4

  // Control Byte X.C6:C0
  // C0 0/1   Mode Static/Dynamic
  // C1 0/1   Digit 1+3 OFF/ON
  // C2 0/1   Digit 2+4 OFF/ON
  // C3 0/1   All Segment Test off/ON
  // C4 0/1   Adds 3mA I_seg
  // C5 0/1   Adds 6mA I_seg
  // C6 0/1   Adds 12mA I_seg
  // Example: x0010111 display DYN-Digit1+3 ON-Digit2+4 ON-3mA
  
    Serial.print(F("\nDisplay > bytes:"));
    Serial.print(numBytes);
    if (numBytes) {
        instructionByte = Wire.read();     // grab instruction byte
//        Serial.print("Instruction: 0x"); // 0 control, 1 digit1,.., 4 digit4
//        Serial.print(instructionByte, HEX);

        switch(instructionByte)
        {
        case CONTROL: // ALL DIGIT
            controlByte = Wire.read(); // grab control byte
//            Serial.print(F(" Control: b"));
//            Serial.print(controlByte, BIN);
            digitData[0]=Wire.read(); // grab digit1..digit4
            digitData[1]=Wire.read();
            digitData[2]=Wire.read();
            digitData[3]=Wire.read();
            updateDisplay = true;
            break;
        case DIGIT1:   // DIGIT1
             Serial.println(F(", Digit1"));
             digitData[0]=Wire.read(); // grab digit 1
             break;
        case DIGIT2:  // DIGIT2
             Serial.println(F(" Digit2"));
             digitData[1]=Wire.read(); // grab digit 2
             break;
        case DIGIT3: // DIGIT3
             Serial.println(F(" Digit3"));
             digitData[2]=Wire.read(); // grab digit 3
             break;
        case DIGIT4:// DIGIT4
             Serial.println(F(", Digit4"));
             digitData[3]=Wire.read(); // grab digit 4
             break;
        default : // error ???
             Serial.print(F(" error Instruction 0x"));
             Serial.println(instructionByte, HEX);
        } // end switch(instructionByte)
        
        switch(controlByte)
        {
        case SAA1064_LO:
             Serial.print(", dark 3mA, ");
             break;
        case SAA1064_OP:
             Serial.print(", normal 6mA, ");
             break;
        case SAA1064_HI:
             Serial.print(", hilight 9mA, ");
             break;
        default:
             Serial.print(", unknown Control: B");
             Serial.println(controlByte, BIN);
             break;
        } // end switch(controlByte)
        
        Serial.print(F("Digits: " ));        
        Serial.print(digitData[0], BIN);
        Serial.print(F(" "));
        Serial.print(digitData[1], BIN);
        Serial.print(F(" "));
        Serial.print(digitData[2], BIN);
        Serial.print(F(" "));
        Serial.println(digitData[3],BIN);
        idleCount = 0;
        
        //Serial.println('SRfontCHAR[digitData[0]]');
        
    } // end if (numBytes)
    
} // end receiveEvent

// handle TXD Event: outgoing I2C slave data +++++++++++++++++++++++++++++++++++
// PCF8574 IO-ports Config & Control & Aux [& Display]

void requestEvent(void)
{
    // Config IO-port 0x21
    
    //Wire.write(B01111111);  // KEY SHIFT
    //Wire.write(B00111111);  // DIA SELECT 12"|10"|7"|78R
    //Wire.write(B01011111);  // ECHO - not working with PB13_3.2.0 
    //Wire.write(B01101111);  // SFTYGRV 1|0
    //Wire.write(B10111111);  // SW_ALGO F|S
    //Wire.write(B11011111);  // SW_ECO  1|0
    //Wire.write(B11101111);  // SW_DEPTH2 LeadIN LeadOUT 1|0
    //Wire.write(B11110111);  // MW1 Min Width
    //Wire.write(B11111011);  // MW0
    //Wire.write(B11111101);  // MT1 Mark Time
    //Wire.write(B11111110);  // MT0

    // get received slave address - shift TWDR right to align 7-bit I2C address
    byte slaveAddr = TWDR >> 1;

    // check if new command was transmitted
    bool cmdTxD = true;

    switch(slaveAddr)
    {
      case ADDR_CTRL:
          switch(remoteCommand) {
              case 'k':
                Wire.write (CTRL_ECHO);
                Serial.println(F("\nControl < ECHO key"));
                break;
              case 's':
                Wire.write(CTRL_START);
                Serial.println(F("\nControl < START"));
                break;
              case 'p':
                Wire.write(CTRL_STOP);
                Serial.println(F("\nControl < STOP"));
                break;
              case 'm':
                Wire.write(CTRL_MARK);
                Serial.println(F("\nControl < MARK"));
                break;
              case 'f':
                Wire.write(CTRL_FAST);
                Serial.println(F("\nControl < FAST"));
                break;
              case 'l':
                Wire.write(CTRL_ECHO);
                Serial.println(F("\nControl < ECHO"));
                break;
              case 'd':
                Wire.write(CTRL_MODE);
                Serial.println(F("\nControl < Display MODE"));
                break;
              default:
                Wire.write(CTRL_IDLE);
                cmdTxD = false;
                Serial.print(F(" R"));
                idleCount++;
                break;
          }
          break;
          
      case ADDR_CNFG:
          switch(remoteCommand) {
              case 'D':
                Wire.write(B00110000 + (pitchConfig & 0x0F));
                Serial.println(F("\nConfig < Diameter Selection"));
                break;
              case 'k':
                Wire.write(B01010000 + (pitchConfig & 0x0F));
                Serial.println(F("\nConfig < ECHO Key"));
                break;
              case 'g':
                Wire.write(B01100000 + (pitchConfig & 0x0F));
                Serial.println(F("\nConfig < Safety Groove"));
                break;
              case 'A':
                bitClear(pitchConfig, 6);
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Algorithm Fast"));
                break;
              case 'a':
                bitSet(pitchConfig, 6);
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Algorithm Slow"));
                break;
              case 'E':
                bitClear(pitchConfig, 5);
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Economy ON"));
                break;
              case 'e':
                bitSet(pitchConfig, 5);
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Economy off"));
                break;
              case 'N':
                bitClear(pitchConfig, 4);
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < 2nd Depth ON"));
                break;
              case 'n':
                bitSet(pitchConfig, 4);
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < 2nd Depth off"));
                break;
              case '1':
                pitchConfig = (pitchConfig & 0xFC) + 3;
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Mark Time #1 (0.6s)"));
                break;
              case '2':
                pitchConfig = (pitchConfig & 0xFC) + 2;
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Mark Time #2 (0.8s)"));
                break;
              case '3':
                pitchConfig = (pitchConfig & 0xFC) + 1;
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Mark Time #3 (1.0s)"));
                break;
              case '4':
                pitchConfig = (pitchConfig & 0xFC);
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Mark Time #4 (1.3s)"));
                break;
              case '5':
                pitchConfig = (pitchConfig & 0xF3) + (3 << 2);
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Min Width #1 (40um)"));
                break;
              case '6':
                pitchConfig = (pitchConfig & 0xF3) + (2 << 2);
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Min Width #2 (50um)"));
                break;
              case '7':
                pitchConfig = (pitchConfig & 0xF3) + (1 << 2);
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Min Width #3 (60um)"));
                break;
              case '8':
                pitchConfig = (pitchConfig & 0xF3);
                Wire.write(pitchConfig);
                Serial.println(F("\nConfig < Min Width #4 (80um/off)"));
                break;
              default:
                Wire.write(pitchConfig);
                cmdTxD = false;
                Serial.print(F(" C"));
                idleCount++;
                break;
          }
          break;
          
      default:
          cmdTxD = false;
          Serial.println(F("\nUnknown slave address < no answer"));
          break;

    } // end switch(slaveAddr)

    if (idleCount > 31) {
      Serial.print(F("\n"));
      idleCount = 0;
    }

    String myCmds = String("kspmfldAaEeNnDg12345678");

    if ((cmdTxD == true) | (remoteCommand == 0xA)) {
      remoteCommand = 0;
      idleCount = 0;
    }
    else if (myCmds.indexOf(remoteCommand) == -1) {
      Serial.print(F("\n- unknown remote control command 0x"));
      Serial.println(remoteCommand, HEX);
      remoteCommand = 0;
      idleCount = 0;
    }
    else if (remoteCommand != 0x0) {
      Serial.print(F("-q"));
    }
    
} // end requestEvent

uint8_t convertPitch2Adafruit7Seg(uint8_t pitch7seg) {
  uint8_t ada7seg = pitch7seg & 0x80; // digit decimal point same as pitch
  if (bitRead(pitch7seg, 6)) bitSet(ada7seg, 0); // seg a
  if (bitRead(pitch7seg, 5)) bitSet(ada7seg, 1); // seg b
  if (bitRead(pitch7seg, 4)) bitSet(ada7seg, 2); // seg c
  if (bitRead(pitch7seg, 3)) bitSet(ada7seg, 3); // seg d
  if (bitRead(pitch7seg, 2)) bitSet(ada7seg, 4); // seg e
  if (bitRead(pitch7seg, 1)) bitSet(ada7seg, 5); // seg f
  if (bitRead(pitch7seg, 0)) bitSet(ada7seg, 6); // seg g
  return ada7seg;
}

uint8_t init7SegDisplay(uint8_t addr7SegDisp, uint8_t adaBright) {
  uint8_t writeErr = swI2C.startWriteWait(addr7SegDisp);
  writeErr += swI2C.llWrite(AF7S_SYSOSC); // System Oscillator ON
  writeErr += swI2C.repeatedStartWrite(addr7SegDisp);
  writeErr += swI2C.llWrite(AF7S_DISPSET); // Display ON
  writeErr += swI2C.repeatedStartWrite(addr7SegDisp);
  writeErr += swI2C.llWrite(AF7S_DIMMSET | adaBright); // Brightness
  swI2C.stop();
  return writeErr;
}

uint8_t write7SegDisplay(uint8_t addr7SegDisp, byte digitData[]) {
  uint8_t ada7segData[5];
  for (int8_t digit = 0; digit < 4; digit++) {
    ada7segData[digit] = convertPitch2Adafruit7Seg(digitData[digit]);
  }
  ada7segData[4] = digitData[2]; // default set to 0
  uint8_t writeErr = swI2C.startWriteWait(addr7SegDisp);
  writeErr += swI2C.llWrite(0); // Digit RAM address 0
  writeErr += swI2C.llWrite(ada7segData[0]); // Digit 1
  writeErr += swI2C.llWrite(0); // unused
  writeErr += swI2C.llWrite(ada7segData[1]); // Digit 2
  writeErr += swI2C.llWrite(0);
  writeErr += swI2C.llWrite(ada7segData[4]); // middle Colon
  writeErr += swI2C.llWrite(0);
  writeErr += swI2C.llWrite(ada7segData[2]); // Digit 3
  writeErr += swI2C.llWrite(0);
  writeErr += swI2C.llWrite(ada7segData[3]); // Digit 4
//  writeErr += swI2C.llWrite(0);
  swI2C.stop();
  return writeErr;
}

uint8_t set7SegBright(uint8_t addr7SegDisp, uint8_t adaBright) {
  uint8_t writeErr = swI2C.startWriteWait(addr7SegDisp);
  writeErr += swI2C.llWrite(AF7S_DIMMSET | adaBright);
  swI2C.stop();
  return writeErr;
}
