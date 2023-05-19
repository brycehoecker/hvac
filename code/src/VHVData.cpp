///////////////////////////////////////////////////////////////////////////////
// $Id: VHVData.cpp,v 1.3 2006/03/28 04:50:53 humensky Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#include "VHVData.h"
// #include "include/VHVData.inl"

extern bool kRecover;
extern int  kTelescopeID;
extern bool kOldSchool[NUMTEL];

/** \file VHVData.cpp
 * Definition of class VHVData
 */

/** \class VHVData VHVData.h
 * \brief Class to share data between main process and VGatherData 
 *
 * This class contains all the information that can be read or set 
 * by the both the VGatherData thread and the main (VHVFrame) process.
 * It has a QMutex to prevent simultaneous access. 
*/

// Changelog:
// TBH 20041201.  Added fSuppressTimer[MAX_PMTS] array of type QTimer to allow
//                automatic restoration of pixels suppressed by current 
//                monitor.  Initially, I am hardwiring a time period for
//                suppression.  Changes are made to VHVData.h and to this file,
//                in constructor and suppressPixel method.

/** Constructor - initialise member variables */
VHVData::VHVData() {

  fMutex.lock();

  fHVStatus = EUnknown;
  for ( int i=0; i<NUM_CRATES; ++i ) {
    fCrateStatus[i] = false;
  }
  for ( int i=0; i<NUM_CRATES; ++i ) {
    fCrateStatus[i] = false;
  }
  fStatusPeriod = 60;
  fCurrentsStatus = EUnknown;
  fPixelPowerStatus = 0;
  fQuit = false;
  for ( int i=0; i<NUMSENSORS; ++i ) {
    fSensor[i] = -9999;
  }
//   fCameraTemp = -9999;
//   fCameraHum = -9999;
  fDisplayMode = IPMT;

  fNumberOfPMTs = 0;
  fNumberOfBoards = 11; // reasonable default initialization.
  fNumberOfCrates = 2;  // reasonable default.

  for (int i=0; i<MAX_BOARDS; ++i) { // UC i<1 otherwise i<MAX_BOARDS
    fBoardCrate[i] = 0;
    fBoardSlot[i] = 0;
    fBoardVSet[i] = 0;
    fBoardVReq[i] = 0;
    fBoardVTrue[i] = -9998;
    fBoardVMax[i] = MAXVOLTS;
    fBoardPower[i] = 0;
    fBoardRampUp[i] = 100;
    fBoardRampDown[i] = 500;
    fBoardIMax[i] = 30;
    fBoardCurrent[i] = -9.999;
    fBoardTemp[i] = -9998;
    fBoardActive[i] = 0;
  }
  for (int i=0; i<MAX_PMTS; ++i) { // Added TBH 040119
    fSuppressed[i] = true;
    fSuppressTimer[i] = new QTimer( this );
    fDisabled[i] = false;
    fCurrent[i] = 0.;
    fVReq[i] = 0;
    fVSet[i] = 0;
    fVTrue[i]=-9998;
    fHVCrate[i] = 0;
    fHVSlot[i] = 0;
    fHVChannel[i] = 0;
    fRampUp[i] = 100;
    fRampDown[i] = 500;
    fStatus[i] = EUnknown;
    fPower[i] = false;
    fPixelFeedbackEnabled[i] = true;
    fFeedbackLevel[i] = 0;
    fHWStatus[i] = 0;
  }
  fCurrentFlag = false;
  fPMTIndex = 7.2;

  fFeedbackEnabled = true;
  fThreshold = 40;
  fHiThreshold = 80;
  fPatternThreshold = 25;
  fPatternMultiplicity = 3;
  fRestoreMargin = 0.03*DEG2RAD; // radians
  fPatternRestoreMargin = 0.05*DEG2RAD; // radians
  fMeasurementsPerCycle = 9;
  fMinTimeBeforeRestore = 60; // seconds
  fNumberOfFeedbackLevels = 0;
  fGainScaling = 0.5;
  fSafetyFactor = 0.9;
  fRestoreAttemptTime = 60; // seconds - 300 is better; 60 for testing
  fMinAbsPositionChange = 0.0*DEG2RAD; // radians
  fPatternDistanceDrop = 0.15*DEG2RAD; // radians
  fPSF = 0.08*DEG2RAD; // radians
  fCyclesPerFeedbackTest = 20;

  fRA = 999.;
  fDec = 99.;

  fLimitBoardVoltages = true;

  fMutex.unlock(); // Unlock moved to after for loop 12-9-03 tbh

  cout << "Hardwired autoRestore/Feedback Parameters:" << endl;
  printAutoRestoreParameters();
}

/** Destructor */
VHVData::~VHVData() 
{
  for (int i=0; i<MAX_PMTS; ++i) {
    delete fSuppressTimer[i];
  }

}

/** Empty channel list */
void VHVData::reset()
{

  fMutex.lock();
  fNumberOfPMTs = 0;
  fMutex.unlock();

}


/** Set the number of PMTs in use at this time
 * \arg \c numberOfPMTs - Number of PMTs in use
 */
void VHVData::setNumberOfPMTs( int numberOfPMTs )
{
  fMutex.lock();
  fNumberOfPMTs = numberOfPMTs;
  fMutex.unlock();
}


/** Set the number of Boards in use at this time
 * \arg \c numberOfBoards - Number of Boards in use
 */
void VHVData::setNumberOfBoards(int numberOfBoards)
{
  fMutex.lock();
  fNumberOfBoards = numberOfBoards;
  fMutex.unlock();
}

/** Set the number of Crates in use at this time
 * \arg \c numberOfCrates - Number of Crates in use
 */
void VHVData::setNumberOfCrates(int numberOfCrates)
{
  fMutex.lock();
  fNumberOfCrates = numberOfCrates;
  fMutex.unlock();
}

/** Set the hardware slot associated with a given channel
 * \arg \c chan   - Software channel number
 * \arg \c hvchan - HV supply channel number
 * \arg \c hvslot - HV supply slot number
 * \note If \c hvslot is less than 0 this channel is disabled.
 */
void VHVData::setCrate(int chan, int crate ) 
{
  fMutex.lock();
  fHVCrate[chan] = crate;
  fMutex.unlock();
}


/** Set the hardware slot associated with a given channel
 * \arg \c chan   - Software channel number
 * \arg \c hvchan - HV supply channel number
 * \arg \c hvslot - HV supply slot number
 * \note If \c hvslot is less than 0 this channel is disabled.
 */
void VHVData::setSlot(int chan, int hvslot) 
{
  fMutex.lock();
  fHVSlot[chan] = hvslot;
  fMutex.unlock();
}


/** Set the hardware slot associated with a given channel
 * \arg \c chan   - Software channel number
 * \arg \c hvchan - HV supply channel number
 * \arg \c hvslot - HV supply slot number
 * \note If \c hvslot is less than 0 this channel is disabled.
 */
void VHVData::setChannel(int chan, int hvchan ) 
{
  fMutex.lock();
  fHVChannel[chan] = hvchan;
  fMutex.unlock();
}


/** Set the status of this channel 
 * \arg \c chan   - Software channel number
 * \arg \c status - ::EnumStatus 
 */
void VHVData::setStatus(int chan, int status) 
{

  fMutex.lock();
  fStatus[chan] = status;
  fMutex.unlock();
}


/** Set the current value of this channel
 * \arg \c chan   - Software channel number
 * \arg \c current - Channel current draw (uA) 
 */
void VHVData::setCurrent(int chan, float current) 
{
  fMutex.lock();
  fCurrent[chan] = current;
  fMutex.unlock();  
}


/** Set the actual value of the high voltage on this channel
 * \arg \c chan   - Software channel number
 * \arg \c vtrue -  Actual voltage (V) 
 */
void VHVData::setVTrue(int chan, int vtrue) 
{
  fMutex.lock();
  fVTrue[chan] = vtrue;
  fMutex.unlock();
}


/** Set the requested voltage for this HV channel
 * \arg \c chan   - channel number
 * \arg \c vreq   - Voltage requested by user (in Volts)
 */
void VHVData::setVReq(int chan, int vreq )
{ 
  fMutex.lock();
  fVReq[chan] = vreq;
  fMutex.unlock();
}


/** Set the set value of the high voltage on this channel
 * \arg \c chan   - Software channel number
 * \arg \c vset   - Actual voltage (V) 
 * \arg \c setby  - EThread or EUser (::EnumSetBy) 
 */
void VHVData::setVSet(int chan, int vset ) 
{
  fMutex.lock();
  if ( !fDisabled[chan] ) {
    fVSet[chan] = vset;
  } else {
    fVSet[chan] = 0;
  }
  fMutex.unlock();
//   cout << "setVSet():  Pix " << chan+1 << ":  vset=" << fVSet[chan]
//        << "  setby=" << fLastSetBy[chan] << endl;
}


/** Set the power status (on or off) of this channel
 * \arg \c chan   - Software channel number
 * \arg \c power  - ON/OFF (::EnumPower)
 * \arg \c setby  - EThread or EUser (::EnumSetBy) 
 */
void VHVData::setPower(int chan, bool power ) 
{

  fMutex.lock();
  fPower[chan] = power;
  fMutex.unlock();
}

/** Set the camera display mode
 * \arg \c mode  - Display mode (::EnumDisplay)
 */
void VHVData::setDisplayMode(int mode)
{ 
  fMutex.lock();
  fDisplayMode = mode;
  fMutex.unlock();
}


/** Get the hardware slot/channel associated with a software channel
 * \arg \c chan   - Software channel number
 * \arg \c hvchan - HV supply channel number
 * \arg \c hvslot - HV supply slot number
 * \note If \c hvslot is less than 0 this channel is disabled.
 */
void VHVData::getCrateAndSlot(int chan, int& hvcrate, int& hvchan, int& hvslot) 
{
  fMutex.lock();
  hvchan   = fHVChannel[chan];
  hvslot   = fHVSlot[chan];
  hvcrate  = fHVCrate[chan];
  fMutex.unlock();
}

/** Return the status of a given channel (::EnumStatus)
 * \arg \c chan   - Software channel number
 */
int VHVData::getStatus(int chan) 
{ 

  fMutex.lock();
  int stat = fStatus[chan];
  fMutex.unlock();
  
  return(stat);
}

/** Return the number of HV Boards in use at this time
 */
int VHVData::getNumberOfBoards()
{ 
  fMutex.lock();
  int numberOfBoards = fNumberOfBoards;
  fMutex.unlock();
  return(numberOfBoards);
}


/** Return the current draw of a given channel (in uA)
 * \arg \c chan   - Software channel number
 */
float VHVData::getCurrent(int chan) 
{
  fMutex.lock();
  float current = fCurrent[chan];
  fMutex.unlock();
  return(current);

};


/** Return the actual voltage of a given channel (in Volts)
 * \arg \c chan   - Software channel number
 */
int VHVData::getVTrue(int chan) 
{ 
  fMutex.lock();
  int vtrue = fVTrue[chan];
  fMutex.unlock();
  return(vtrue);
	 
};


/** Return the voltage requested by the user (in Volts)
 * \arg \c chan   - channel number
 */
int VHVData::getVReq(int chan) 
{ 
  fMutex.lock();
  int vreq = fVReq[chan];
  fMutex.unlock();
  return(vreq);
}


/** Return the voltage setting  of a given channel (in Volts)
 * \arg \c chan   - Software channel number
 */
int VHVData::getVSet(int chan) 
{
  fMutex.lock();
  int vset = fVSet[chan];
  fMutex.unlock();
  return(vset);
}


/** Return the power status setting  of a given channel (::EnumPower)
 * \arg \c chan   - Software channel number
 */
bool VHVData::getPower(int chan) 
{
  fMutex.lock();
  int power = fPower[chan];
  fMutex.unlock();
  return(power);
}


/** Return the number of PMTs/channels assigned at this time
 */
int VHVData::getNumberOfPMTs()
{ 
  fMutex.lock();
  int npmt = fNumberOfPMTs;
  fMutex.unlock();
  return(npmt);
};


/** Return the number of PMTs/channels assigned at this time
 */
int VHVData::getDisplayMode()
{ 
  fMutex.lock();
  int  mode = fDisplayMode;
  fMutex.unlock();
  return(mode);
}


/** Set the voltage limit for this HV board/slot
 * \arg \c slot   - High Voltage Slot number
 * \arg \c vmax   - Voltage limit (in Volts)
 * \arg \c setby  - EThread or EUser (::EnumSetBy) 
 */
void VHVData::setBoardVMax(int slot, int vmax )
{ 
  fMutex.lock();
  fBoardVMax[slot] = vmax;
  fMutex.unlock();
}

/** Set the current limit for this HV board/slot
 * \arg \c slot   - High Voltage Slot number
 * \arg \c imax   - Current limit (in uA?)
 * \arg \c setby  - EThread or EUser (::EnumSetBy) 
 */
void VHVData::setBoardIMax(int slot, float imax )
{ 
  fMutex.lock();
  fBoardIMax[slot] = imax;
  fMutex.unlock();
}


/** Set the user-requested voltage level setting for this HV board/slot
 * \arg \c slot   - High voltage slot number
 * \arg \c v      - User-requested supply voltage setting (in Volts)
 */
void VHVData::setBoardVReq(int slot, int v )
{ 
  fMutex.lock();
  fBoardVReq[slot] = v;
  fMutex.unlock();
}


/** Set the voltage level setting for this HV board/slot
 * \arg \c slot   - High voltage slot number
 * \arg \c v      - Supply voltage setting (in Volts)
 * \arg \c setby  - EThread or EUser (::EnumSetBy) 
 */
void VHVData::setBoardVSet(int slot, int v )
{ 
  fMutex.lock();
  fBoardVSet[slot] = v;
  fMutex.unlock();
}

/** Set the true voltage level for this HV board/slot
 * \arg \c slot   - High voltage slot number
 * \arg \c v      - Supply voltage level (in Volts)
 */
void VHVData::setBoardVTrue(int slot, int v)
{ 
  fMutex.lock();
  fBoardVTrue[slot] = v;
  fMutex.unlock();
}

/** Set the ramp up speed for this HV channel
 * \arg \c slot   - High voltage slot number
 * \arg \c ramp   - Ramp up speed (in Volts/second)
 * \arg \c setby  - EThread or EUser (::EnumSetBy) 
 */
void VHVData::setRampUp(int chan, int ramp)
{ 
  fMutex.lock();
  fRampUp[chan] = ramp;
  fMutex.unlock();
}

/** Set the ramp down speed for this HV channel
 * \arg \c slot   - High voltage slot number
 * \arg \c ramp   - Ramp down speed (in Volts/second)
 * \arg \c setby  - EThread or EUser (::EnumSetBy) 
 */
void VHVData::setRampDown(int chan, int ramp)
{ 
  fMutex.lock();
  fRampDown[chan] = ramp;
  fMutex.unlock();
}

/** Set the ramp up speed for this HV board/slot
 * \arg \c slot   - High voltage slot number
 * \arg \c ramp   - Ramp up speed (in Volts/second)
 * \arg \c setby  - EThread or EUser (::EnumSetBy) 
 */
void VHVData::setBoardRampUp(int slot, int ramp )
{ 
  fMutex.lock();
  fRampUp[slot] = ramp;
  fMutex.unlock();
}

/** Set the ramp down speed for this HV board/slot
 * \arg \c slot   - High voltage slot number
 * \arg \c ramp   - Ramp down speed (in Volts/second)
 * \arg \c setby  - EThread or EUser (::EnumSetBy) 
 */
void VHVData::setBoardRampDown(int slot, int ramp )
{ 
  fMutex.lock();
  fRampDown[slot] = ramp;
  fMutex.unlock();
}

/** Set the current drawn by this HV board/slot
 * \arg \c slot   - High voltage slot number
 * \arg \c i      - Current drawn (uA?)
 */
void VHVData::setBoardCurrent(int slot, float i)
{ 
  fMutex.lock();
  fBoardCurrent[slot] = i;
  fMutex.unlock();
}

/** Set the temperature of this HV board/slot
 * \arg \c slot   - High voltage slot number
 * \arg \c temp   - Board temperature (in C)
 */
void VHVData::setBoardTemp(int slot, float temp)
{ 
  fMutex.lock();
  fBoardTemp[slot] = temp;
  fMutex.unlock();
}

/** Set the status of this board
 * \arg \c slot   - High voltage slot number
 * \arg \c active - Active/Inactive (true/false)
 */
void VHVData::setBoardActive(int slot, bool active)
{ 
  fMutex.lock();
  fBoardActive[slot] = active;
  fMutex.unlock();
}

/** Set the power status of this HV board/slot
 * \arg \c slot   - High voltage slot number
 * \arg \c power  - Power status (::EnumPower)
 * \arg \c setby  - EThread or EUser (::EnumSetBy) 
 */
void VHVData::setBoardPower(int slot, bool power )
{ 
  fMutex.lock();
  fBoardPower[slot] = power;
  fMutex.unlock();
}

void VHVData::setBoardCrate(int board, int crate )
{
  fMutex.lock();
  fBoardCrate[board] = crate;
  fMutex.unlock();
}

void VHVData::setBoardSlot(int board, int slot )
{
  fMutex.lock();
  fBoardSlot[board] = slot;
  fMutex.unlock();
}

/** Set the status of aquiring information from the current monitor
 * \arg \c status  - Current monitor communications status (:EnumStatus) 
 */
void VHVData::setCurrentsStatus(int status)
{ 
  fMutex.lock();
  fCurrentsStatus = status;
  fMutex.unlock();
}

/** Set the status of communications with the HV supply
 * \arg \c status  - HV supply communications status (:EnumStatus) 
 */
void VHVData::setHVStatus(int status)
{ 
  fMutex.lock();
  fHVStatus = status;
  fMutex.unlock();
}


/** Return the voltage limit on this board (in Volts)
 * \arg \c slot   - High voltage slot number
 */
int VHVData::getVMax(int slot) 
{ 
  fMutex.lock();
  int vmax = fBoardVMax[slot];
  fMutex.unlock();
  return(vmax);
}


/** Return the user-requested voltage setting on this board (in Volts)
 * \arg \c slot   - High voltage slot number
 */
int VHVData::getBoardVReq(int slot) 
{ 
  fMutex.lock();
  int v = fBoardVReq[slot];
  fMutex.unlock();
  return(v);     
}


/** Return the voltage setting on this board (in Volts)
 * \arg \c slot   - High voltage slot number
 */
int VHVData::getBoardVSet(int slot) 
{ 
  fMutex.lock();
  int v = fBoardVSet[slot];
  fMutex.unlock();
  return(v);	 
}

/** Return the actual supply voltage on this board (in Volts)
 * \arg \c slot   - High voltage slot number
 */
int VHVData::getBoardVTrue(int slot) 
{ 
  fMutex.lock();
  int v = fBoardVTrue[slot];
  fMutex.unlock();
  return(v);
}

/** Return the current drawn by this board (in uA?)
 * \arg \c slot   - High voltage slot number
 */
float VHVData::getBoardCurrent(int slot) 
{ 
  fMutex.lock();
  float i = fBoardCurrent[slot];
  fMutex.unlock();
  return(i);
}

/** Return the current limit on this board (in uA?)
 * \arg \c slot   - High voltage slot number
 */
float VHVData::getIMax(int slot) 
{ 
  fMutex.lock();
  float imax = fBoardIMax[slot];
  fMutex.unlock();
  return(imax);
}

/** Return the ramp up speed (in Volts/second) on this channel.
 * \arg \c chan   - High voltage slot number
 */
int VHVData::getRampUp(int chan) 
{ 
  fMutex.lock();
  int ramp = fRampUp[chan];
  fMutex.unlock();
  return(ramp);
}

/** Return the ramp down speed (in Volts/second) on this channel.
 * \arg \c chan   - High voltage chan number
 */
int VHVData::getRampDown(int chan) 
{ 
  fMutex.lock();
  int ramp = fRampDown[chan];
  fMutex.unlock();
  return(ramp);
}

/** Return the ramp up speed (in Volts/second) on this board.
 * \arg \c slot   - High voltage slot number
 */
int VHVData::getBoardRampUp(int slot) 
{ 
  fMutex.lock();
  int ramp = fRampUp[slot];
  fMutex.unlock();
  return(ramp);
}

/** Return the ramp down speed (in Volts/second) on this board.
 * \arg \c slot   - High voltage slot number
 */
int VHVData::getBoardRampDown(int slot) 
{ 
  fMutex.lock();
  int ramp = fRampDown[slot];
  fMutex.unlock();
  return(ramp);
}

/** Return the temperature of this board (in C)
 * \arg \c slot   - High voltage slot number
 */
int VHVData::getBoardTemp(int slot) 
{ 
  fMutex.lock();
  int temp = fBoardTemp[slot];
  fMutex.unlock();
  return(temp);
}

/** Return the status of this board (true=active)
 * \arg \c slot   - High voltage slot number
 */
bool VHVData::getBoardActive(int slot) 
{ 
  fMutex.lock();
  bool active = fBoardActive[slot];
  fMutex.unlock();
  return(active);
}

/** Return the power status of this board (true=ON)
 * \arg \c slot   - High voltage slot number
 */
bool VHVData::getBoardPower(int slot) 
{ 
  //cout << "VHVData::getBoardPower(): requested slot is " << slot;
  fMutex.lock();
  bool power = fBoardPower[slot];
  //cout << "VHVData::getBoardPower(): power state is " << power;
  fMutex.unlock();
  return(power);
}

int VHVData::getBoardCrate(int slotIndex)
{
  fMutex.lock();
   int crate = fBoardCrate[slotIndex];
  fMutex.unlock();
  return(crate);
}

int VHVData::getBoardSlot(int slotIndex)
{
  fMutex.lock();
   int slot = fBoardSlot[slotIndex];
  fMutex.unlock();
  return(slot);
}

/** Return the status of communications with the current monitor (::EnumStatus)
 */
int VHVData::getCurrentsStatus() 
{ 
  fMutex.lock();
  int status = fCurrentsStatus;
  fMutex.unlock();
  return(status);
}

/** Return the status of communications with the HV supply (::EnumStatus)
 */
int VHVData::getHVStatus() 
{ 
  fMutex.lock();
  int status = fHVStatus;
  fMutex.unlock();
  return(status);
}

/** Return whether or not an individual channel has been suppressed (ie, due to
 *  star in its field of view).  TBH 040119.
 */
bool VHVData::getSuppressed(int chan)
{
  fMutex.lock();
  bool suppressed = fSuppressed[chan];
  fMutex.unlock();
  return(suppressed);
}

/** Restore the output of a given channel.  TBH 041024.
 */
void VHVData::restorePixel(int chan)
{
  fMutex.lock();
  fSuppressed[chan] = RESTORED;
  fMutex.unlock();
}

/** Set the suppression of a given channel.
 */
void VHVData::initSuppressed(int chan, bool suppressed)
{
  fMutex.lock();
  fSuppressed[chan] = suppressed;
  fMutex.unlock();
//   cout << "initSuppressed(): Pix " << chan+1 << ":  " << fSuppressed[chan]
//        << endl;
}

/** Returns the proper index to the fHVSlot array given the physical board and
 *  slot number.  TBH 20040608.  This function may be superfluous, and as
    implemented is same as getBoardIndex...
 */
int VHVData::getSlotIndex(int crate, int slot)
{
  int slotIndex;

  fMutex.lock();
  slotIndex = slot + 8*crate;
  fMutex.unlock();
  return slotIndex;
}

/** Returns the proper index to the fBoard* arrays given the physical board and
 *  slot number.  TBH 20040608.
 */
int VHVData::getBoardIndex(int crate, int slot)
{
  int boardIndex;

  fMutex.lock();
  boardIndex = slot/2 + 8*crate;
  fMutex.unlock();
  return boardIndex;
}

/** Returns the value of fPixelPowerStatus, which is 1 if one or more pixels
 *  have power ON and 0 if all pixels have power OFF.
 */
int VHVData::getPixelPowerStatus()
{
  int pixelPowerStatus;
  fMutex.lock();
  pixelPowerStatus = fPixelPowerStatus;
  fMutex.unlock();
  return pixelPowerStatus;
}

/** Sets the value of fPixelPowerStatus, which is 1 if one or more pixels
 *  have power ON and 0 if all pixels have power OFF.
 */
void VHVData::setPixelPowerStatus( int status )
{
  fMutex.lock();
  fPixelPowerStatus = status;
//   cout << "fPixelPowerStatus is now " << fPixelPowerStatus << endl;
  fMutex.unlock();
}

bool VHVData::getDisabled( int chan )
{
  bool disabled;
  fMutex.lock();
  disabled = fDisabled[chan];
  fMutex.unlock();
  return disabled;
}


void VHVData::setDisabled( int chan, bool disabled )
{
  fMutex.lock();
  fDisabled[chan] = disabled;
  fMutex.unlock();
}


int VHVData::getCameraTemp() 
{
  fMutex.lock();
  int temp = fSensor[SENSOR_CAMTEMP];
  fMutex.unlock();
  return temp;
}


int VHVData::getCameraHum()
{
  fMutex.lock();
  int hum = fSensor[SENSOR_CAMHUM];
  fMutex.unlock();
  return hum;
}


int VHVData::getSensor( int sensor )
{
  fMutex.lock();
  int hum = fSensor[sensor];
  fMutex.unlock();
  return hum;
}


void VHVData::setCameraTemp( int temp ) 
{
  fMutex.lock();
  fSensor[SENSOR_CAMTEMP] = temp;
  fMutex.unlock();
}


void VHVData::setCameraHum( int hum )
{
  fMutex.lock();
  fSensor[SENSOR_CAMHUM] = hum;
  fMutex.unlock();
}


void VHVData::setSensor( int sensor, int value )
{
  fMutex.lock();
  fSensor[sensor] = value;
  fMutex.unlock();
}


/** Sets the fQuit flag to true. **/
void VHVData::setQuit()
{
  fMutex.lock();
  fQuit = true;
  fMutex.unlock();
}

/** Returns the fQuit flag. **/
bool VHVData::getQuit()
{
  bool quitFlag;
  fMutex.lock();
  quitFlag = fQuit;
  fMutex.unlock();
  return quitFlag;
}


void VHVData::setCurrentFlag( bool flag )
{
  fMutex.lock();
  fCurrentFlag = flag;
  fMutex.unlock();
}


bool VHVData::getCurrentFlag()
{
  fMutex.lock();
  bool flag = fCurrentFlag;
  fMutex.unlock();
  return flag;
}


int VHVData::getThreshold()
{
  fMutex.lock();
  int thr = fThreshold;
  fMutex.unlock();
  return thr;
}


int VHVData::getHiThreshold()
{
  fMutex.lock();
  int thr = fHiThreshold;
  fMutex.unlock();
  return thr;
}


int VHVData::getPatternThreshold()
{
  fMutex.lock();
  int thr = fPatternThreshold;
  fMutex.unlock();
  return thr;
}


int VHVData::getPatternMultiplicity()
{
  fMutex.lock();
  int mult = fPatternMultiplicity;
  fMutex.unlock();
  return mult;
}


float VHVData::getRestoreMargin()
{
  fMutex.lock();
  float mar = fRestoreMargin;
  fMutex.unlock();
  return mar;
}


float VHVData::getPatternRestoreMargin()
{
  fMutex.lock();
  float mar = fPatternRestoreMargin;
  fMutex.unlock();
  return mar;
}


int VHVData::getCheckTime()
{
  fMutex.lock();
  int tim = fCheckTime;
  fMutex.unlock();
  return tim;
}


int VHVData::getRecentTime()
{
  fMutex.lock();
  int tim = fRecentTime;
  fMutex.unlock();
  return tim;
}


void VHVData::setHiThreshold( int thr )
{
  fMutex.lock();
  fHiThreshold = thr;
  fMutex.unlock();
}


void VHVData::setThreshold( int thr )
{
  fMutex.lock();
  fThreshold = thr;
  fMutex.unlock();
}


void VHVData::setPatternThreshold( int thr )
{
  fMutex.lock();
  fPatternThreshold = thr;
  fMutex.unlock();
}


void VHVData::setPatternMultiplicity( int mult )
{
  fMutex.lock();
  fPatternMultiplicity = mult;
  fMutex.unlock();
}


void VHVData::setRestoreMargin( float mar )
{
  fMutex.lock();
  fRestoreMargin = mar;
  fMutex.unlock();
}


void VHVData::setPatternRestoreMargin( float mar )
{
  fMutex.lock();
  fPatternRestoreMargin = mar;
  fMutex.unlock();
}


void VHVData::setCheckTime( int tim )
{
  fMutex.lock();
  fCheckTime = tim;
  fMutex.unlock();
}


void VHVData::setRecentTime( int tim )
{
  fMutex.lock();
  fRecentTime = tim;
  fMutex.unlock();
}


void VHVData::setRA( float ra )
{
  fMutex.lock();
  fRA = ra;
  fMutex.unlock();
}


void VHVData::setDec( float dec )
{
  fMutex.lock();
  fDec = dec;
  fMutex.unlock();
}


float VHVData::getRA()
{
  fMutex.lock();
  float ra = fRA;
  fMutex.unlock();
  return ra;
}


float VHVData::getDec()
{
  fMutex.lock();
  float dec = fDec;
  fMutex.unlock();
  return dec;
}


float VHVData::getRARad()
{
  fMutex.lock();
  float ra = fRA*DEG2RAD;
  fMutex.unlock();
  return ra;
}


float VHVData::getDecRad()
{
  fMutex.lock();
  float dec = fDec*DEG2RAD;
  fMutex.unlock();
  return dec;
}


void VHVData::setElevation( float el )
{
  fMutex.lock();
  fElevation = el;
  fMutex.unlock();
}


void VHVData::setAzimuth( float az )
{
  fMutex.lock();
  fAzimuth = az;
  fMutex.unlock();
}

float VHVData::getElevation()
{
  fMutex.lock();
  float el = fElevation;
  fMutex.unlock();
  return el;
}

float VHVData::getAzimuth()
{
  fMutex.lock();
  float az = fAzimuth;
  fMutex.unlock();
  return az;
}

void VHVData::setLoopGain( float gain )
{
  fMutex.lock();
  fLoopGain = gain;
  fMutex.unlock();
}

void VHVData::setPMTIndex( float index )
{
  fMutex.lock();
  fPMTIndex = index;
  fMutex.unlock();
}

void VHVData::setZeroAmpsFlag( bool zero )
{
  fMutex.lock();
  fZeroAmps = zero;
  fMutex.unlock();
}

void VHVData::setMaxAllowedVoltage( int max )
{
  fMutex.lock();
  fMaxAllowedVolts = max;
  fMutex.unlock();
}

void VHVData::setTargetCurrent( float target )
{
  fMutex.lock();
  fTargetCurrent = target;
  fMutex.unlock();
}

float VHVData::getLoopGain()
{
  float gain;
  fMutex.lock();
  gain = fLoopGain;
  fMutex.unlock();
  return gain;
}

float VHVData::getPMTIndex()
{
  float index;
  fMutex.lock();
  index = fPMTIndex;
  fMutex.unlock();
  return index;
}

bool VHVData::getZeroAmpsFlag()
{
  bool flag;
  fMutex.lock();
  flag = fZeroAmps;
  fMutex.unlock();
  return flag;
}

int VHVData::getMaxAllowedVoltage()
{
  int max;
  fMutex.lock();
  max = fMaxAllowedVolts;
  fMutex.unlock();
  return max;
}

float VHVData::getTargetCurrent()
{
  float target;
  fMutex.lock();
  target = fTargetCurrent;
  fMutex.unlock();
  return target;
}

void VHVData::setAutoStatus( int chan, bool status )
{
  fMutex.lock();
  fAutoStatus[chan] = status;
  fMutex.unlock();
}


bool VHVData::getAutoStatus( int chan )
{
  bool status;
  fMutex.lock();
  status = fAutoStatus[chan];
  fMutex.unlock();
  return status;
}

 
void VHVData::setCrateStatus( int crate, bool state )
{
  fMutex.lock();
  fCrateStatus[crate] = state;
  fMutex.unlock();
}


bool VHVData::getCrateStatus( int crate )
{
  bool status;
  fMutex.lock();
  status = fCrateStatus[crate];
  fMutex.unlock();
  return status;
}


void VHVData::setMeasurementsPerCycle( int m )
{
  fMutex.lock();
  fMeasurementsPerCycle = m;
  fMutex.unlock();
}


void VHVData::setMinTimeBeforeRestore( int t )
{
  fMutex.lock();
  fMinTimeBeforeRestore = t;
  fMutex.unlock();
}


void VHVData::setNumberOfFeedbackLevels( int n )
{
  fMutex.lock();
  fNumberOfFeedbackLevels = n;
  fMutex.unlock();
}


void VHVData::setGainScaling( float g )
{
  fMutex.lock();
  fGainScaling = g;
  fMutex.unlock();
}


void VHVData::setSafetyFactor( float s )
{
  fMutex.lock();
  fSafetyFactor = s;
  fMutex.unlock();
}


void VHVData::setRestoreAttemptTime( int t )
{
  fMutex.lock();
  fRestoreAttemptTime = t;
  fMutex.unlock();
}


void VHVData::setMinAbsPositionChange( float m )
{
  fMutex.lock();
  fMinAbsPositionChange = m;
  fMutex.unlock();
}


void VHVData::setPatternDistanceDrop( float p )
{
  fMutex.lock();
  fPatternDistanceDrop = p;
  fMutex.unlock();
}


int VHVData::getMeasurementsPerCycle()
{
  fMutex.lock();
  int m = fMeasurementsPerCycle;
  fMutex.unlock();
  return m;
}


int VHVData::getMinTimeBeforeRestore()
{
  fMutex.lock();
  int t = fMinTimeBeforeRestore;
  fMutex.unlock();
  return t;
}


int VHVData::getNumberOfFeedbackLevels()
{
  fMutex.lock();
  int n = fNumberOfFeedbackLevels;
  fMutex.unlock();
  return n;
}


float VHVData::getGainScaling()
{
  fMutex.lock();
  float g = fGainScaling;
  fMutex.unlock();
  return g;
}


float VHVData::getSafetyFactor()
{
  fMutex.lock();
  float s = fSafetyFactor;
  fMutex.unlock();
  return s;
}


int VHVData::getRestoreAttemptTime()
{
  fMutex.lock();
  int t = fRestoreAttemptTime;
  fMutex.unlock();
  return t;
}


float VHVData::getMinAbsPositionChange()
{
  fMutex.lock();
  float m = fMinAbsPositionChange;
  fMutex.unlock();
  return m;
}


float VHVData::getPatternDistanceDrop()
{
  fMutex.lock();
  float p = fPatternDistanceDrop;
  fMutex.unlock();
  return p;
}


void VHVData::setPSF( float p )
{
  fMutex.lock();
  fPSF = p;
  fMutex.unlock();
}


float VHVData::getPSF()
{
  float p;
  fMutex.lock();
  p = fPSF;
  fMutex.unlock();
  return p;
}


void VHVData::printAutoRestoreParameters( ostream& creek )
{
  fMutex.lock();
  creek << "fCheckTime=" << fCheckTime
        << "  kOldSchool=" << kOldSchool[kTelescopeID]
        << "  fThreshold=" << fThreshold
        << "  fHiThreshold=" << fHiThreshold
        << "  fPatternThreshold=" << fPatternThreshold
        << "  fPatternMultiplicity=" << fPatternMultiplicity
        << "  fRestoreMargin=" << fRestoreMargin*RAD2DEG
        << "  fMeasurementsPerCycle=" << fMeasurementsPerCycle
        << "  fMinTimeBeforeRestore=" << fMinTimeBeforeRestore
        << "  fNumberOfFeedbackLevels=" << fNumberOfFeedbackLevels
        << "  fGainScaling=" << fGainScaling
        << "  fSafetyFactor=" << fSafetyFactor
        << "  fRestoreAttemptTime=" << fRestoreAttemptTime
        << "  fMinAbsPositionChange=" << fMinAbsPositionChange*RAD2DEG
        << "  fPatternDistanceDrop=" << fPatternDistanceDrop*RAD2DEG
        << "  fPSF=" << fPSF*RAD2DEG
        << "  fPMTIndex=" << fPMTIndex
        << "  fRA=" << fRA
        << "  fDec=" << fDec
        << endl;
  fMutex.unlock();
}


void VHVData::setCyclesPerFeedbackTest( int c )
{
  fMutex.lock();
  fCyclesPerFeedbackTest = c;
  fMutex.unlock();
}


int VHVData::getCyclesPerFeedbackTest()
{
  fMutex.lock();
  int c = fCyclesPerFeedbackTest;
  fMutex.unlock();
  return c;
}


void VHVData::setFeedbackEnabled( bool f )
{
  fMutex.lock();
  fFeedbackEnabled = f;
  fMutex.unlock();
}


bool VHVData::isFeedbackEnabled()
{
  fMutex.lock();
  bool f = fFeedbackEnabled;
  fMutex.unlock();
  return f;
}


void VHVData::setPixelFeedbackEnabled( int ch, bool f )
{
  fMutex.lock();
  fPixelFeedbackEnabled[ch] = f;
  fMutex.unlock();
}


bool VHVData::isPixelFeedbackEnabled( int ch )
{
  fMutex.lock();
  bool f = fPixelFeedbackEnabled[ch];
  fMutex.unlock();
  return f;
}


void VHVData::setFeedbackLevel( int ch, int lev )
{
  fMutex.lock();
  fFeedbackLevel[ch] = lev;
  fMutex.unlock();
}


int VHVData::getFeedbackLevel( int ch )
{
  fMutex.lock();
  int lev = fFeedbackLevel[ch];
  fMutex.unlock();
  return lev;
}


void VHVData::setHWStatus( int ch, unsigned short status )
{
  fMutex.lock();
  fHWStatus[ch] = status;
  fMutex.unlock();
}


unsigned short VHVData::getHWStatus( int ch )
{
  short int status;
  fMutex.lock();
  status = fHWStatus[ch];
  fMutex.unlock();
  return status;
}


void VHVData::setStatusPeriod( int period )
{
  fMutex.lock();
  fStatusPeriod = period;
  fMutex.unlock();
}


int VHVData::getStatusPeriod()
{
  int per;
  fMutex.lock();
  per = fStatusPeriod;
  fMutex.unlock();
  return per;
}


