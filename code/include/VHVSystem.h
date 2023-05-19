///////////////////////////////////////////////////////////////////////////////
// $Id: VHVSystem.h,v 1.1.1.1 2006/01/18 20:18:51 ergin Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#ifndef THVSYSTEM_H
#define THVSYSTEM_H

/** \file VHVSystem.h
 * \brief Header file for class VHVSystem (defined in VHVSystem.cpp).
 */

#define HV_OK  0             /**< Flag for success in CAEN system */
#define HV_ERROR  -9999      /**< Flag for error in CAEN system */

#ifndef ONOFF
#define ONOFF
/** \enum EnumPower
 * Enumeration for HV power status 
 */
enum EnumPower { OFF, ON }; 
#endif

class VHVSystem {

 public:
   VHVSystem(char *ip, char *n);
   ~VHVSystem();

   int connect();
   int disconnect();

   int setVoltage(int board, int chan, float v);
   int setV1(int board, int chan, float v);
   int setVMax(int board, int chan, float v);
   int setRamp(int board, int chan, float up, float down);
   int setRampUp(int board, int chan, float up);
   int setRampDown(int board, int chan, float down);
   int setPower(int board, int chan, int flag);
   int setPowerOptions(int board, int chan, int Pon, int Poff);
   int setPowerDown(int board, int chan, int Poff);
   int setIMax(int board, int chan, float i);
   void  getVoltage(int board, int chan, float &trueV, float &setV);
   float getV1(int board, int chan);
   int   getRamp(int board, int chan, float &up, float &down);
   float getRampUp(int board, int chan);
   float getRampDown(int board, int chan);
   int   getPowerStatus(int board, int chan);
   int   getPowerOptions(int board, int chan, int &Pon, int &Poff);
   int   getPowerDown(int board, int chan);
   float getCurrent(int board, int chan);
   float getTemperature(int board, int chan);
   float getVTrue(int board, int chan);
   float getVSet(int board, int chan);
   float getVMax(int board, int chan);
   float getIMax(int board, int chan);

 private:
   char fIPaddress[16];  /**< IP address of the high voltage crate */
   char fName[16];       /**< String to identify this system */

};

#endif
