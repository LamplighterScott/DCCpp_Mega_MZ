/**********************************************************************

Outputs.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "Arduino.h"

#ifndef Outputs_h
#define Outputs_h

struct OutputData {
  byte oStatus;
  int id;
  byte pin; 
  byte iFlag; 
};

struct Output{
  static Output *firstOutput;
  int num;
  struct OutputData data;
  Output *nextOutput;
  void activate(int s);
  static void init();
  static void parse(char *c);
  static Output* get(int);
  static void remove(int);
  static void load();
  static void shiftRegister();
  static void signal(byte, int, int, byte);
  static void store();
  static Output *create(int, int, int);
  static void show(int=0);
}; // Output
  
#endif
