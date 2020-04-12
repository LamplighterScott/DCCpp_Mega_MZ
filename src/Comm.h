

/**********************************************************************

Comm.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "Config.h"

#if COMM_TYPE == 1                 // Ethernet Shield Card Selected

  #if COMM_INTERFACE == 1
    #define COMM_SHIELD_NAME      "ARDUINO-CC ETHERNET SHIELD (WIZNET 5100)"
    #include <Ethernet.h>         // built-in Arduino.cc library

  #elif COMM_INTERFACE == 2
    #define COMM_SHIELD_NAME      "ARDUINO-ORG ETHERNET-2 SHIELD (WIZNET 5500)"
    #include <Ethernet2.h>        // https://github.com/arduino-org/Arduino

  #elif COMM_INTERFACE == 3
    #define COMM_SHIELD_NAME      "SEEED STUDIO ETHERNET SHIELD (WIZNET 5200)"
    #include <EthernetV2_0.h>     // https://github.com/Seeed-Studio/Ethernet_Shield_W5200

  #endif

  extern EthernetServer INTERFACE;
#endif

//////////////////////////////////////////////////////////////////////////////
// DEFINE THE SERIAL1 EXECUTE COMMAND FOR MP3 PLAYER
//////////////////////////////////////////////////////////////////////////////

# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]
