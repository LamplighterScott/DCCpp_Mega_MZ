This is a branched sketch of the DCC++ for a Marklin Z layout using a mega controller.

These files are for using the Atom IDE to upload the DCC++ sketch up to an Arduino Mega assuming the following.
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
*
