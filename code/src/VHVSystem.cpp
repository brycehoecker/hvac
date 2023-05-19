///////////////////////////////////////////////////////////////////////////////
// $Id: VHVSystem.cpp,v 1.1.1.1 2006/01/18 20:18:51 ergin Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////

/** \file VHVSystem.cpp
 * \brief File for definition of the class VHVSystem.
 */

#include <string.h>

#include "VHVSystem.h"
#include "CAENHVWrapper.h"

/** \class VHVSystem VHVSystem.h
 * \brief A rewrapping of the CAEN HV communication functions
 provided by libcaenhvwrap and libsy1527 (in /usr/lib). 
 * \sa VGatherData
*/

/** Constructor - requires a crate IP address and a name */
VHVSystem::VHVSystem(char *ip, char *name)
{

   strncpy(fIPaddress, ip, 16);
   strncpy(fName, name, 16);

}

/** Destructor */
VHVSystem::~VHVSystem() { }

/** Connect to the remote system at IP address given in constructor. Returns status. */
int VHVSystem::connect()
{
   return (CAENHVInitSystem(fName, LINKTYPE_TCPIP, fIPaddress,
                            "admin", "admin"));
}

/** Disconnect from the remote system. Returns status. */
int VHVSystem::disconnect()
{
   return (CAENHVDeinitSystem(fName));
}

/** Set a high voltage value on the crate.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c v     - High voltage setting (in Volts)
 */
int VHVSystem::setVoltage(int board, int chan, float v)
{

   return (CAENHVSetChParam
           (fName, board, "V0Set", 1, (ushort *) & chan, &v));

}

/** Set the software high voltage limit on a given channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c v     - Maximum voltage (in Volts)
 */
int VHVSystem::setVMax(int board, int chan, float v)
{

   return (CAENHVSetChParam           (fName, board, "SVMax", 1, (ushort *) & chan, &v));

}

/** Set the software current limit on a given channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c v     - Maximum Current (in mA)
 *
 * \note The current limit actually applies to the whole board.
 */
int VHVSystem::setIMax(int board, int chan, float i)
{

   return (CAENHVSetChParam
           (fName, board, "I0Set", 1, (ushort *) & chan, &i));

}

/** Return the actual voltage supplied by this channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 */
float VHVSystem::getVTrue(int board, int chan)
{

   float v;
   int status =
       CAENHVGetChParam(fName, board, "VMon", 1, (ushort *) & chan, &v);
   if (status != CAENHV_OK)
      v = HV_ERROR;
   return (v);

}

/** Return the voltage setting on this channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 */
float VHVSystem::getVSet(int board, int chan)
{

   float v;
   int status =
       CAENHVGetChParam(fName, board, "V0Set", 1, (ushort *) & chan, &v);
   if (status != CAENHV_OK)
      v = HV_ERROR;
   return (v);

}

/** Return the alternative voltage setting (V1) on this channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 */
float VHVSystem::getV1(int board, int chan)
{
  
  float v;
  int status =
     CAENHVGetChParam(fName, board, "V1Set", 1, (ushort *) & chan, &v);
  if (status != CAENHV_OK)
    v = HV_ERROR;
  return (v);

}

/** Set the alternative voltage (V1) on this channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c v     - V1 setting (Volts)
 */
int VHVSystem::setV1(int board, int chan, float v)
{

  int status =
    CAENHVSetChParam(fName, board, "V1Set", 1, (ushort *) & chan, &v);
  
  return (status);

}

/** Return the software voltage limit on this channel (Volts).
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 */
float VHVSystem::getVMax(int board, int chan)
{

   float v;
   int status =
       CAENHVGetChParam(fName, board, "SVMax", 1, (ushort *) & chan, &v);
   if (status != CAENHV_OK)
      v = HV_ERROR;
   return (v);

}

/** Return the software current limit on this channel (mA).
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 */
float VHVSystem::getIMax(int board, int chan)
{

   float i;
   int status =
     CAENHVGetChParam(fName, board, "I0Set", 1, (ushort *) & chan, &i);
   if (status != CAENHV_OK)
     i = HV_ERROR;
   return (i);

}

/** Get both the true and set voltages on this channel (in Volts)
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c trueV - Actual voltage 
 * \arg \c setV  - Set voltage
 */
void VHVSystem::getVoltage(int board, int chan, float &trueV, float &setV)
{

   trueV = getVTrue(board, chan);
   setV = getVSet(board, chan);

}

/** Set the voltage ramp up and ramp down speeds on a given channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c up    - Ramp up speed   (volts/second)
 * \arg \c down  - Ramp down speed (volts/second)
 */
int VHVSystem::setRamp(int board, int chan, float up, float down)
{

  int status =
    CAENHVSetChParam(fName, board, "RUp", 1, (ushort *) & chan, &up);
  status +=
    CAENHVSetChParam(fName, board, "RDWn", 1, (ushort *) & chan, &down);
  
  return (status);
}

/** Set the voltage ramp up speed on a given channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c up    - Ramp up speed   (volts/second)
 */
int VHVSystem::setRampUp(int board, int chan, float up)
{

  int status =
    CAENHVSetChParam(fName, board, "RUp", 1, (ushort *) & chan, &up);
  
  return (status);
}

/** Set the voltage ramp down speed on a given channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c down  - Ramp down speed   (volts/second)
 */
int VHVSystem::setRampDown(int board, int chan, float down)
{

  int status =
    CAENHVSetChParam(fName, board, "RDWn", 1, (ushort *) & chan, &down);
  
  return (status);
}

/** Get the voltage ramp up and ramp down speeds on a given channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c up    - Ramp up speed   (volts/second)
 * \arg \c down  - Ramp down speed (volts/second)
 */
int VHVSystem::getRamp(int board, int chan, float &up, float &down)
{

  int status =
       CAENHVGetChParam(fName, board, "RUp", 1, (ushort *) & chan, &up);
  status +=
       CAENHVGetChParam(fName, board, "RDWn", 1, (ushort *) & chan, &down);

  return (status);
}

/** Return the voltage ramp up speed (V/s) on a given channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 */
float VHVSystem::getRampUp(int board, int chan)
{

  float up;

  int status =
    CAENHVGetChParam(fName, board, "RUp", 1, (ushort *) & chan, &up);
  if (status != CAENHV_OK)
    up = HV_ERROR;
  
  return (up);
}

/** Return the voltage ramp down speed (V/s) on a given channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 */
float VHVSystem::getRampDown(int board, int chan)
{
  float down;
  
  int status =
    CAENHVGetChParam(fName, board, "RDWn", 1, (ushort *) & chan, &down);
  if (status != CAENHV_OK)
    down = HV_ERROR;
  
  return (down);
}

/** Set the power on/off status on a given channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c flag  - On/Off status (On = 1, Off = 0)
 */
int VHVSystem::setPower(int board, int chan, int flag)
{
   return (CAENHVSetChParam(fName, board, "Pw", 1, (ushort *) & chan,
                            (ulong *) & flag));
}

/** Return the power on/off status on a given channel.
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 */
int VHVSystem::getPowerStatus(int board, int chan)
{

  ulong flag;
  int status =
    CAENHVGetChParam(fName, board, "Pw", 1, (ushort *) & chan, &flag);
  
  if (status == CAENHV_OK) {
    status = flag;
  } else {
    status = HV_ERROR;
  }
  return (status);
  
}

/** Set power on/off options (ramp up/down, kill etc)
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c Pon   - Power On option
 * \arg \c Poff  - Power Off option
 */
int VHVSystem::setPowerOptions(int board, int chan, int Pon, int Poff)
{

  int status = CAENHVSetChParam(fName, board, "POn", 1,
				(ushort *) & chan, (ushort *) & Pon);
  status += CAENHVSetChParam(fName, board, "PDwn", 1,
			     (ushort *) & chan, (ushort *) & Poff);
  
  return (status);
  
}

/** Set power off option (ramp down or kill)
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c Poff  - Power Off option
 */
int VHVSystem::setPowerDown(int board, int chan, int Poff)
{

  int status = CAENHVSetChParam(fName, board, "PDwn", 1,
				(ushort *) & chan, (ushort *) & Poff);
  
  return (status);
  
}

/** Get power on/off options (ramp up/down, kill etc)
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 * \arg \c Pon   - Power On option
 * \arg \c Poff  - Power Off option
 */
int VHVSystem::getPowerOptions(int board, int chan, int &pOn, int &pOff)
{
  
  int status =
    CAENHVGetChParam(fName, board, "POn", 1, (ushort *) & chan, &pOn);
  status +=
    CAENHVGetChParam(fName, board, "PDwn", 1, (ushort *) & chan, &pOff);
  
  return (status);
}

/** Return power off option (ramp down or kill)
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number
 */
int VHVSystem::getPowerDown(int board, int chan)
{
  
  int pOff;
  int status =
    CAENHVGetChParam(fName, board, "PDwn", 1, (ushort *) &chan, &pOff);
  if (status != HV_OK) pOff = HV_ERROR; 
  
  return (pOff);
}


/** Return actual current drawn by a given board
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number 
 * \note The channel number is not used for the VERITAS boards.
 */
float VHVSystem::getCurrent(int board, int chan)
{

   float i;
   int status =
     CAENHVGetChParam(fName, board, "IMon", 1, (ushort *) & chan, &i);
     //     CAENHVGetChParam(fName, board, "I0Set", 1, (ushort *) & chan, &i);
   if (status != CAENHV_OK)
      i = HV_ERROR;
   return (i);

}

/** Return the temperature (in C)  on a given board
 * \arg \c board - Board/Slot number
 * \arg \c chan  - Channel number 
 * \note The channel number is not used for the VERITAS boards.
 */
float VHVSystem::getTemperature(int board, int chan)
{

   float t;
   int status =
       CAENHVGetChParam(fName, board, "Temp", 1, (ushort *) & chan, &t);
   if (status != CAENHV_OK)
     t = HV_ERROR;
   return (t);

}
