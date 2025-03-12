/**********************************************************************

Outputs.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman, edited by Lamplighter Scott 2018

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/
/**********************************************************************

DCC++ BASE STATION supports optional OUTPUT control of any unused Arduino Pins for custom purposes.
Pins can be activited or de-activated.  The default is to set ACTIVE pins HIGH and INACTIVE pins LOW.
However, this default behavior can be inverted for any pin in which case ACTIVE=LOW and INACTIVE=HIGH.

Definitions and state (ACTIVE/INACTIVE) for pins are retained in EEPROM and restored on power-up.
The default is to set each defined pin to active or inactive according to its restored state.
However, the default behavior can be modified so that any pin can be forced to be either active or inactive
upon power-up regardless of its previous state before power-down.

To have this sketch utilize one or more Arduino pins as custom outputs, first define/edit/delete
output definitions using the following variation of the "Z" command:

  <Z ID PIN IFLAG>:            creates a new output ID, with specified PIN and IFLAG values.
                               if output ID already exists, it is updated with specificed PIN and IFLAG.
                               note: output state will be immediately set to ACTIVE/INACTIVE and pin will be set to HIGH/LOW
                               according to IFLAG value specifcied (see below).
                               returns: <O> if successful and <X> if unsuccessful (e.g. out of memory)

  <Z ID>:                      deletes definition of output ID
                               returns: <O> if successful and <X> if unsuccessful (e.g. ID does not exist)

  <Z>:                         lists all defined output pins
                               returns: <Y ID PIN IFLAG STATE> for each defined output pin or <X> if no output pins defined

where

  ID: the numeric ID (0-32767) of the output
  PIN: the arduino pin number to use for the output
  STATE: the state of the output (0=INACTIVE / 1=ACTIVE)
  IFLAG: defines the operational behavior of the output based on bits 0, 1, and 2 as follows:

          IFLAG, bit 0:   0 = forward operation (ACTIVE=HIGH / INACTIVE=LOW)
                          1 = inverted operation (ACTIVE=LOW / INACTIVE=HIGH)

          IFLAG, bit 1:   0 = state of pin restored on power-up to either ACTIVE or INACTIVE depending
                              on state before power-down; state of pin set to INACTIVE when first created
                          1 = state of pin set on power-up, or when first created, to either ACTIVE of INACTIVE
                              depending on IFLAG, bit 2

          IFLAG, bit 2:   0 = state of pin set to INACTIVE uponm power-up or when first created
                          1 = state of pin set to ACTIVE uponm power-up or when first created

          IFLAG, bit 3:   1 = turnout
          IFLAG, bit 4:   1 = decoupler
          IFlAG, bit 5:   1 = semaphore, signal, roundtable, shed doors
          IFLAG, bit 6:   1 = lights


Once all outputs have been properly defined, use the <E> command to store their definitions to EEPROM.
If you later make edits/additions/deletions to the output definitions, you must invoke the <E> command if you want those
new definitions updated in the EEPROM.  You can also clear everything stored in the EEPROM by invoking the <e> command.

To change the state of outputs that have been defined use:

  <Z ID STATE>:                temporarily activates output ID and optionally switches from Rund to Gerade and visa versa
                               returns: <Y ID STATE>, or <X> if turnout ID does not exist

where

  ID: the numeric ID (0-32767) of the turnout to control
  STATE: the state of the output (0=INACTIVE / 1=ACTIVE)

When controlled as such, the Arduino updates and stores the direction of each output in EEPROM so
that it is retained even without power.  A list of the current states of each output in the form <Y ID STATE> is generated
by this sketch whenever the <s> status command is invoked.  This provides an efficient way of initializing
the state of any outputs being monitored or controlled by a separate interface or GUI program.

**********************************************************************/

#include "Outputs.h"
#include "SerialCommand.h"
#include "DCCpp_Uno.h"
#include "EEStore.h"
#include <EEPROM.h>
#include "Comm.h"
#include "Wire.h"


  // CHANGE SWITCH LIGHT
  // Arduino pins: 11,8,12
  // Mega Analog pins: 67,68,69

  // Pin connected to ST_CP of 74HC595 (TI Pin 12 ST_CP) RCLK
  const int latchPin = 68; // 8 A14 (MEGA)

  // Pin connected to SH_CP of 74HC595 (TI Pin 10 SH_P) SRCLK
  const int clockPin = 69; // 12 A15 (MEGA)

  // Pin connected to DS of 74HC595  (TI Pin 14 DS) SERIN
  const int dataPin = 67; // 11 A13 (MEGA)

  //  ILLUMINATE LED'S
  //  MEGA shiftOut() to TI 74HC595N Shift Register IC's, 5v DC
  // const byte byteOfOne = 1;
  // static byte switchByteA;
  // static byte switchByteB;
  int shiftBytes[4]={0};

  //  MCP2017 I/O expansion boards addresses
  unsigned char boardAddresses[4] = {0x20,0x21,0x22,0x23};

///////////////////////////////////////////////////////////////////////////////
void Output::init() {

//  Start up I2C for I/O Extender Boards used in Outputs with a MEGA
Wire.begin();

uint8_t boardAddresses[3]={0x20,0x22,0x23};
for (int i=0; i<3; ++i) {
  Wire.beginTransmission(boardAddresses[i]);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // Set all pins as outputs
  Wire.endTransmission();
  Wire.beginTransmission(boardAddresses[i]);
  Wire.write(0x01); // IODIRB register
  Wire.write(0x00); // Set all pins as outputs
  Wire.endTransmission();
  delay(5);
}

}

///////////////////////////////////////////////////////////////////////////////

void Output::activate(int s){
  data.oStatus=(s>0);
                                                 // if s>0, set status to active, else inactive
  //int pinValue;                                                     // set state of output pin to HIGH or LOW depending on whether bit zero of iFlag is set to 0 (ACTIVE=HIGH) or 1 (ACTIVE=LOW)
  //pinValue = data.oStatus ^ bitRead(data.iFlag,0);
  //int pinToActivate = data.pin;
  //if (pinValue > 0){
    //pinToActivate = data.pin + 1;
  //}

  //digitalWrite(pinToActivate,HIGH);
  //delay(200);
  //digitalWrite(pinToActivate,LOW);
  Output::signal(data.oStatus, data.id, data.pin, data.iFlag);

  if(num>0)
    // EEPROM.put(num,data.oStatus);
    INTERFACE.print("<Y");
    INTERFACE.print(data.id);
    if(data.oStatus==0)
      INTERFACE.print(" 0>");
  else
    INTERFACE.print(" 1>");

}  // end activate


///////////////////////////////////////////////////////////////////////////////

Output *Output::create(int id, int pin, int iFlag){
  Output *tt;

  // Creates a tt and sets output(s)

  if(firstOutput==NULL){
    firstOutput=(Output *)calloc(1,sizeof(Output));
    tt=firstOutput;
  } else if((tt=get(id))==NULL){
    tt=firstOutput;
    while(tt->nextOutput!=NULL)
      tt=tt->nextOutput;
    tt->nextOutput=(Output *)calloc(1,sizeof(Output));
    tt=tt->nextOutput;
  }

  if(tt==NULL){       // problem allocating memory
    
      INTERFACE.println("<X " + String(pin) + ">");
    return(tt);
  }

  tt->data.id=id;
  tt->data.pin=pin;
  tt->data.iFlag=iFlag;
  // tt->data.oStatus=bitRead(tt->data.iFlag,1)?bitRead(tt->data.iFlag,2):0;      // sets status to 0 (INACTIVE) if bit 1 of iFlag=0, otherwise set to value of bit 2 of iFlag
  tt->data.oStatus=bitRead(iFlag,2);
  // int oStatus = tt->data.oStatus;

  int pinOutEven = tt->data.pin;
  int pinOutOdd = pinOutEven+1;
  pinMode(pinOutEven,OUTPUT);
  if (bitRead(iFlag, 3) || bitRead(iFlag, 5)) // Turnouts and signals
  {
    pinMode(pinOutOdd,OUTPUT);
    signal(tt->data.oStatus, id, pin, iFlag);
  }

  // INTERFACE.println("<0>");
  // INTERFACE.println("<" + String(pinOutEven) + ">");
  if (Serial3) {  // request next output, if any
    Serial3.println("<C>");
  } 
  

  return(tt);

}  // end create

///////////////////////////////////////////////////////////////////////////////

Output* Output::get(int n){
  Output *tt;
  for(tt=firstOutput;tt!=NULL && tt->data.id!=n;tt=tt->nextOutput);
  return(tt);
}  // end get


///////////////////////////////////////////////////////////////////////////////

void Output::load(){
  // struct OutputData data;
  // Output *tt;

  #if COMM_INTERFACE == 4 // ESP8266, shift registers and output to MOSFET's

    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    if (Serial3) {
      Serial3.println("<S>");
    }

    /*
    if (Serial){
      Serial.println("Loading pins as output: ");
    }

    for(int i=0;i<EEStore::eeStore->data.nOutputs;i++){
      EEPROM.get(EEStore::pointer(),data);
      tt=create(data.id,data.pin,data.iFlag);

       tt->data.oStatus=bitRead(tt->data.iFlag,1)?bitRead(tt->data.iFlag,2):data.oStatus;      // restore status to EEPROM value is bit 1 of iFlag=0, otherwise set to value of bit 2 of iFlag
         int pinOutEven = tt->data.pin;
         int pinOutOdd = pinOutEven+1;
         pinMode(pinOutEven,OUTPUT);
         if (bitRead(iFlag, 3) || bitRead(iFlage, 5)) // Turnouts and signals
         {
           pinMode(pinOutOdd,OUTPUT);
         }
         if (Serial) {
           if (i>0) Serial.print(", ");
             Serial.print(pinOutEven);

          }
      
      
      Output::signal(data.oStatus, data.id, data.pin, data.iFlag);

      tt->num=EEStore::pointer();
      EEStore::advance(sizeof(tt->data));
    }
     if (Serial) {
      Serial.println();
    }
     
   */
  /*#else

    for(int i=0;i<EEStore::eeStore->data.nOutputs;i++){
      EEPROM.get(EEStore::pointer(),data);
      tt=create(data.id,data.pin,data.iFlag);
      tt->data.oStatus=bitRead(tt->data.iFlag,1)?bitRead(tt->data.iFlag,2):data.oStatus;      // restore status to EEPROM value is bit 1 of iFlag=0, otherwise set to value of bit 2 of iFlag
      digitalWrite(tt->data.pin,tt->data.oStatus ^ bitRead(tt->data.iFlag,0));
      pinMode(tt->data.pin,OUTPUT);
      tt->num=EEStore::pointer();
      EEStore::advance(sizeof(tt->data));
    }
*/
  #endif

}  // end load

///////////////////////////////////////////////////////////////////////////////

void Output::parse(char *c){
  // Serial.println("<Parse: " + String(c) + ">");
  int n,s,m;
  Output *t;
  switch(sscanf(c,"%d %d %d",&n,&s,&m)){

    case 2:                     // argument is string with id number of output followed by zero (LOW) or one (HIGH)
      t=get(n);
      if(t!=NULL)
        t->activate(s);
      else
        INTERFACE.print("<X>");
      break;

    case 3:                     // argument is string with id number of output followed by a pin number and invert flag
      create(n,s,m);
    break;

    case 1:                     // argument is a string with id number only
      remove(n);
    break;

    case -1:                    // no arguments
      if (Serial) {
        Serial.println("Z command received without parameters");
      }
      show(1);                  // verbose show
    break;
  }

}  // end parse


///////////////////////////////////////////////////////////////////////////////

void Output::remove(int n){
  Output *tt,*pp;

  for(tt=firstOutput;tt!=NULL && tt->data.id!=n;pp=tt,tt=tt->nextOutput);

  if(tt==NULL){
    INTERFACE.print("<X>");
    return;
  }

  if(tt==firstOutput)
    firstOutput=tt->nextOutput;
  else
    pp->nextOutput=tt->nextOutput;

  free(tt);

  INTERFACE.print("<O>");

}  // end remove


///////////////////////////////////////////////////////////////////////////////

void Output::shiftRegister()
{

  // test
  //shiftBytes[0] = 0;
  //shiftBytes[1] = 0;
  //shiftBytes[2] = 0;
  //shiftBytes[3] = 85; //170

  // turn off the output so the pins don't light up while shifting bits:
  digitalWrite(latchPin, LOW);

  // Set green (4th IC)
  // shiftOut(dataPin, clockPin, LSBFIRST, switchByteB);
  // Not to invert all for red (3rd IC)
  // shiftOut(dataPin, clockPin, LSBFIRST, ~switchByteB);
  // Set green (2nd IC)
  // shiftOut(dataPin, clockPin, LSBFIRST, switchByteA);
  // NOT to invert all for red (1st IC)
  // shiftOut(dataPin, clockPin, LSBFIRST, ~switchByteA);
  

  for (int i=3; i>-1; i--) {
    shiftOut(dataPin, clockPin, MSBFIRST, shiftBytes[i]);
  }

  // turn on the output to illuminate LEDs
  digitalWrite(latchPin, HIGH);

}  // end shiftRegister



///////////////////////////////////////////////////////////////////////////////

void Output::show(int n){
  Output *tt;

  if(firstOutput==NULL){
    INTERFACE.print("<X>");
    return;
  }

  for(tt=firstOutput;tt!=NULL;tt=tt->nextOutput){
    INTERFACE.print("<Y");
    INTERFACE.print(tt->data.id);
    if(n==1){
      INTERFACE.print(" ");
      INTERFACE.print(tt->data.pin);
      INTERFACE.print(" ");
      INTERFACE.print(tt->data.iFlag);
    }
    if(tt->data.oStatus==0)
       INTERFACE.print(" 0>");
     else
       INTERFACE.print(" 1>");
  }
}  // end show


///////////////////////////////////////////////////////////////////////////////

void Output::signal(byte oStatus, int id, int pinOut, byte iFlag) {

  // ENERGIZE SWITCH AND DECOUPLER SOLENOIDS, 12v DC
  // MEGA pinouts to Toshiba ULN2803APG

  // Serial.println(String(oStatus));

  // For FussenWeb version N (Withrottle): convert one based to zero based; id 1 to x ==> pinBit 0 to x-1

  int order = pinOut-22;  // order for LED shift registers starting with pin 22 as 0 shift register output number
  int byteNo = order/8;  // determine which shift register IC
  int pinBit = order-8*byteNo; // create bit address for LED
  // Serial.println("Order: " + String(order) + " byteNo: " + String(byteNo) + " pinBit: " + String(pinBit));


  if (bitRead(iFlag, 3)) // Turnouts
  {
    byte firstSolenoidState = 1;
      byte secondSolenoidState = 0;  
      if (oStatus>0) {
        pinOut++;
        firstSolenoidState = 0;
        secondSolenoidState = 1;
      }
    
    if (pinOut <100)  // Turnouts connected directly to Mega
    {
    // use main board pinouts connected to MOSFET Array 32 board 0
      digitalWrite(pinOut,HIGH);
      delay(20);
      digitalWrite(pinOut,LOW);
      delay(50);

    } else  // Turnouts connected via I/O Extenders to MOSFET 32 Array boards 1 and 2
    {
      byte boardNumber = 0;  // Four expansion boards: 0, 1, 2, 3; board 1 reserved for future use
      uint8_t portAddress = (pinOut % 2 == 0) ? 0x13 : 0x12;  // Assign even pinOuts to port B and odd pinouts to port A
      int fromAddress = 0;
      if (pinOut < 138)
      {
        // Board 0: A0, A1 and A2 all low - using only one I/O extender board on MOSFET 32 Array board 1
        order = pinOut - 80;  // pinOut(x) - startAddress (122) + previousLastAddress (41) + 1
        fromAddress = 122; // start of 8 bit series, use first address of series

      /*} else if (pinOut < 154)  // exansion board 1 not implemented yet
      {
          boardNumber = 1; // A0 high - second extender boards on MOSFET 32 Array board 1
          order = pinOut - 80;  // same LED shift registrer offset as expansion board 0
          fromAddress = 153;  // Last address used because pins order is reversed for second expansion board
      */
      } else if (pinOut < 238) 
      {
        boardNumber = 2; // A1 high - one of two extender boards on MOSFET 32 Array board 2
        order = pinOut - 84;  // increase offset by 4 for two more dual solenoid non-LED users
        fromAddress = 222; // first address of series
  
      } else
      {
        boardNumber = 3; // A0 and A1 high - secondo extender boards on MOSFET 32 Array board 2
        order = pinOut - 84;  // same LED shift registrer offset as expansion board 2
        fromAddress = 253;  // Last address used because pin order is reversed for second expansion board
      }

      int pinAddress = 1 << ((abs(fromAddress-pinOut))/2); // create 8-bit binary for pin address

      // Energize turnout solenoid
      uint8_t boardAddress = boardAddresses[boardNumber];
      Serial.println("boardAdress: " + String(boardAddress) + " portAddress: " + String(portAddress) + " pinAddress: " + String(pinAddress));
      Wire.beginTransmission(boardAddress);
      Wire.write(portAddress); // address port
      Wire.write(pinAddress);  // value to send
      Wire.endTransmission();
      delay(10);

      // De-energize turnout solenoids
      Wire.beginTransmission(boardAddress);
      Wire.write(portAddress); // address port
      Wire.write(0);  // value to send
      Wire.endTransmission();
      delay(20);

    }

    // Recalculate pinBit because order might have been altered
    byteNo = order/8;  // determine which shift register IC
    pinBit = order-8*byteNo; // create bit address for LED

    bitWrite(shiftBytes[byteNo], pinBit, firstSolenoidState); // set rund/red/turnout on/off
    if (pinBit<8) {
      pinBit++; 
    } else if (byteNo<3) {
      byteNo++;
      pinBit=0;
    }
    bitWrite(shiftBytes[byteNo], pinBit, secondSolenoidState); // set gerade/green/straight on/off

  }
  else if (bitRead(iFlag, 4))  // Decouplers
  {

    int status = (oStatus>0) ? 0 : 1;
    bitWrite(shiftBytes[byteNo], pinBit, status);
    Output::shiftRegister();

    for(int i=0;i<30;i++){
      digitalWrite(pinOut,HIGH);
      delay(10);
      digitalWrite(pinOut,LOW);
      delay(10);
    }
    delay(50);
    bitWrite(shiftBytes[byteNo], pinBit, oStatus);

  }
  else if (bitRead(iFlag, 5))  // Semaphores, turntable, sheds
  {
    if (oStatus>0) {
      pinOut++;
    }
    
    digitalWrite(pinOut,HIGH);
    delay(20);
    digitalWrite(pinOut,LOW);
    delay(50);
    
  }
  else if (bitRead(iFlag, 6))  // Lamps
  {
    bitWrite(shiftBytes[byteNo], pinBit, oStatus);
    // if ((oStatus ^ bitRead(iFlag,0)) > 0)
    // {
    //  bitWrite(shiftBytes[byteNo], pinBit, oStatus);
    // }
  }

  // switches 1-8, decoupler 17-20, 21-24 building lights, 25 semiphore
  // if (Serial) {
  //  Serial.println("<Signal: " + String(pinOut) + " " + String(oStatus) + ">");
  // }
  Output::shiftRegister();

}  // end Signal


///////////////////////////////////////////////////////////////////////////////

void Output::store(){
  Output *tt;

  tt=firstOutput;
  EEStore::eeStore->data.nOutputs=0;

  while(tt!=NULL){
    tt->num=EEStore::pointer();
    EEPROM.put(EEStore::pointer(),tt->data);
    EEStore::advance(sizeof(tt->data));
    tt=tt->nextOutput;
    EEStore::eeStore->data.nOutputs++;
  }

}  // end store

///////////////////////////////////////////////////////////////////////////////

Output *Output::firstOutput=NULL;
