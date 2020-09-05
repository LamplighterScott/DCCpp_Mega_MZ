This is a branched sketch of the DCC++ for a Marklin Z layout using a mega controller, ESP8266 WiFi, shift registers for turnout signals and MOSFETs for driving turnout solenoids.

For the Arduino IDE, download or clone all the the files in the folder "src" and target the DCCpp_Mega_MZ.ino file
for starting the upload.

* A Motor Shield mounted on the Mega with a jumper between pins 2 and 13
* A DF Player Mini connected to the Mega's Serial one Tx1/Rx1 pins
* The DF Player removable flash card loaded with mp3 sound files.  Each MP3 file name should start with four digits.
* The numbers should be in a continuous sequence: 0001xxx.mp3, 0002yyy.mp3 ... 0024zzz.mp3.
* xxx, yyy and zzz can be any numberic or alphabet letters or any length; no spaces; dash or underscore are okay.
* An ESP-01 and UART adapter connected to the Mega's Serial three Tx3/Rx3 pins
* The sketch FussenThrottleSafari.ino and data files FussenCCS.ccs, FussenHTML.html and FussenJS.js upload to the ESP
* Relays for turunouts/couplers/signals connected to the Mega's pins 22-53, not all pins need to be populated
* Mega's Pins A13, A14, A15 connected to four TI 74HC595 8 bit shift registers in series
*  A13 (aka pin 67 for shiftOut function) connected to DS of 74HC595 - data pin
*  A14 (aka pin 68 for shiftOut function) connected to ST_Cp of 74HC595 - latch pin
*  A15 (aka pin 69 for shiftOut function) connected to SH_CP of 74HC595 - clock pin
* LEDs connected to the outputs of the shifter resisters represent the turnouts as thrown (red) or straight (green)


Key sketch files (for Adruino IDE) are in the "src" folder.  The others are for Atom/PlatformIO IDE.

Minimal changes were made to the master.  DCC++ serial communication protocol is the same, except for the addition of 'H' and 'J' commands for serial print of ESP-8266 commands for debugging, and DF Player control, respectively.

Changes from DCC++:

Config.h
  35: Added COMM_INTERFACE = 4 for WiFi via ESP8266

DCCpp_Mega.ino
  4-32: Comments
  291: init Serial3 at 115,200 baud for WiFi
  305: added branch to announce SERIAL3

DCCpp_Uno.h
  102: COMM_TYPE = 0
  103: INTERFACE Serial3

Outputs.ccp
  81-98: shift register and pin variables
  223-259: set shift register and turnout pins
  276-344: turnout pin write commands and logic
  332-249: shiftRegister function: execute library commands for LEDs (library included in Arduino.h)

Outputs.h
  32: static void shiftRegister();
  33: static void signal(byte, int, byte, byte);

SerialCommand.ccp
  28: #include "DFRobotDFPlayerMini.h"
  38: DFRobotDFPlayerMini variable;
  53-63: init Serial1 and DF Player
  71-106: playSong function
  178-196: added case "H", serial print ESP8266 output tagged with "H" command
  192-194: added case "J", playSound(), command from ESP8288

SerialCommand.h
  25: playSound variable
