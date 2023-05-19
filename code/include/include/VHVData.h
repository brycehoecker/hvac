///////////////////////////////////////////////////////////////////////////////
// $Id: VHVData.h,v 1.3 2006/03/28 04:50:53 humensky Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#ifndef VHVDATA_H
#define VHVDATA_H

/** \file VHVData.h
 * \brief Header file for class VHVData (defined in VHVData.cpp).
 */

#include <iostream> 
#include <fstream> 

#include <qthread.h>
#include <qobject.h>
#include <qtimer.h>

#include "VHV.h"
#include <VDBHighVoltage.h>
#include <VDBArrayControl.h>
#include <VDBPositioner.h>
#include <VDBTransaction.h>
#include <ctime>

using namespace std;

class VHVData : public QObject {

  Q_OBJECT
  
    public:
  VHVData();
  ~VHVData();
  void reset();
  void setNumberOfPMTs( int numberOfPMTs );
  void setNumberOfBoards(int numberOfBoards);
  void setNumberOfCrates(int numberOfCrates);

  void setCrate( int chan, int crate );
  void setSlot(int chan, int hvslot );
  void setChannel( int chan, int hvchan );
  void setStatus(int chan, int stat);
  void setCurrent(int chan, float current);
  void setVTrue(int chan, int vtrue);
  void setVReq( int chan, int vreq );
  void setVSet(int chan, int vset );
  void setPower(int chan, bool power );
  void setRampUp(int slotIndex, int rampup );
  void setRampDown(int slotIndex, int rampdown );
  void setDisplayMode(int mode);
  void setBoardPower(int slotIndex, bool power );
  void setBoardVReq(int slotIndex, int vreq );
  void setBoardVMax(int slotIndex, int vmax );
  void setBoardIMax(int slotIndex, float imax );
  void setBoardVSet(int slotIndex, int vset );
  void setBoardVTrue(int slotIndex, int vset);
  void setBoardRampUp(int slotIndex, int rampup );
  void setBoardRampDown(int slotIndex, int rampdown );
  void setBoardCurrent(int slotIndex, float current);
  void setBoardTemp(int slotIndex, float temperature);
  void setBoardActive(int slotIndex, bool active);
  void setBoardCrate(int board, int crate );
  void setBoardSlot(int board, int slot );
  void setHVStatus(int status);
  void setCurrentsStatus(int status);
  void initSuppressed(int chan, bool suppressed); /* Added TBH 040120. */
  void setPixelPowerStatus( int status );
  void setQuit(); /* Added TBH 20041012. */
  void setDisabled( int chan, bool disabled );
  void setCameraTemp( int temp );
  void setCameraHum( int hum );
  void setSensor( int sensor, int value );
  void setHWStatus( int chan, unsigned short status );

  void setThreshold( int thr );
  void setHiThreshold( int thr );
  void setPatternThreshold( int thr );
  void setPatternMultiplicity( int mult );
  void setRestoreMargin( float mar );
  void setPatternRestoreMargin( float mar );
  void setCheckTime( int tim );
  void setRecentTime( int tim );

  void setRA( float ra );
  void setDec( float dec );
  void setElevation( float el );
  void setAzimuth( float az );
  void setLoopGain( float gain );
  void setPMTIndex( float index );
  void setZeroAmpsFlag( bool zero );
  void setMaxAllowedVoltage( int max );
  void setTargetCurrent( float target );
  void setCurrentFlag( bool flag );
  void setAutoStatus( int chan, bool status );

  void setFeedbackEnabled( bool f );
  void setPixelFeedbackEnabled( int ch, bool f );
  void setMeasurementsPerCycle( int m );
  void setMinTimeBeforeRestore( int t );
  void setNumberOfFeedbackLevels( int n );
  void setGainScaling( float g );
  void setSafetyFactor( float s );
  void setRestoreAttemptTime( int t );
  void setMinAbsPositionChange( float m );
  void setPatternDistanceDrop( float p );
  void setPSF( float p );
  void setCyclesPerFeedbackTest( int c );
  void setFeedbackLevel( int ch, int lev );
  void setStatusPeriod( int period );

  void getCrateAndSlot(int chan, int& hvcrate, int& hvchan, int& hvslot);
  int getStatus(int chan);
  float getCurrent(int chan);
  int getVTrue(int chan);
  int getVSet(int chan);
  int getVReq( int chan );
  bool getPower(int chan);
  int getNumberOfPMTs();
  int getNumberOfBoards();
  int getNumberOfCrates();
  int getDisplayMode();
  bool getBoardPower(int slotIndex);
  int getVMax(int slotIndex);
  int getBoardVTrue(int slotIndex);
  int getBoardVReq(int slotIndex);
  int getBoardVSet(int slotIndex);
  float getBoardCurrent(int slotIndex);
  float getIMax(int slotIndex);
  int getRampUp(int chan);
  int getRampDown(int chan);
  int getBoardRampUp(int slotIndex);
  int getBoardRampDown(int slotIndex);
  int getBoardTemp(int slotIndex);
  bool getBoardActive(int slotIndex);
  int getHVStatus();
  int getCurrentsStatus();
  int getBoardCrate(int slotIndex);
  int getBoardSlot(int slotIndex);
  bool getSuppressed(int chan); /* Added TBH 041119 */
  int getSlotIndex(int crate, int slot);
  int getBoardIndex(int crate, int slot);
  int getPixelPowerStatus();
  bool getQuit(); /* Added TBH 20041012. */
  bool getDisabled( int chan );
  int getCameraTemp();
  int getCameraHum();
  int getSensor( int sensor );
  int getStatusPeriod();
  unsigned short getHWStatus( int ch );

  int getThreshold();
  int getHiThreshold();
  int getPatternThreshold();
  int getPatternMultiplicity();
  float getRestoreMargin();
  float getPatternRestoreMargin();
  int getCheckTime();
  int getRecentTime();

  float getRA();
  float getDec();
  float getRARad();
  float getDecRad();
  float getElevation();
  float getAzimuth();
  float getLoopGain();
  float getPMTIndex();
  bool getZeroAmpsFlag();
  int getMaxAllowedVoltage();
  float getTargetCurrent();
  bool getAutoStatus( int chan );
  bool isFeedbackEnabled();
  bool isPixelFeedbackEnabled( int ch );
  int getMeasurementsPerCycle();
  int getMinTimeBeforeRestore();
  int getNumberOfFeedbackLevels();
  float getGainScaling();
  float getSafetyFactor();
  int getRestoreAttemptTime();
  float getMinAbsPositionChange();
  float getPatternDistanceDrop();
  float getPSF();
  int getCyclesPerFeedbackTest();
  void printAutoRestoreParameters( ostream& creek = cout );
  int getFeedbackLevel( int ch );

  void enableLimitBoardVoltages() { fLimitBoardVoltages = true; };
  void disableLimitBoardVoltages() { fLimitBoardVoltages = false; };
  bool getStateLimitBoardVoltages() { return fLimitBoardVoltages; };

  bool getCurrentFlag();
  void setCrateStatus( int crate, bool state );
  bool getCrateStatus( int crate );

  public slots:
    void restorePixel(int chan); /* Added TBH 041024 */

 private:
  
  QMutex fMutex;
  QTimer* fSuppressTimer[MAX_PMTS];

  int fNumberOfPMTs;
  int fNumberOfBoards;
  int fNumberOfCrates;

  int fHVCrate[MAX_PMTS];
  int fHVSlot[MAX_PMTS];
  int fHVChannel[MAX_PMTS];
  int fStatus[MAX_PMTS];
  bool fPower[MAX_PMTS];
  int fVReq[MAX_PMTS]; // Voltage that vhv has asked HV crate to set a pixel to.
  int fVSet[MAX_PMTS];
  int fVTrue[MAX_PMTS];
  float fCurrent[MAX_PMTS];
  bool fSuppressed[MAX_PMTS]; // true = restored, false = suppressed
  bool fDisabled[MAX_PMTS]; // true = disabled, false = working
  bool fAutoStatus[MAX_PMTS]; // true = autosuppress enabled, false = disabled.
  int fRampUp[MAX_PMTS];
  int fRampDown[MAX_PMTS];
  short int fHWStatus[MAX_PMTS];

  int fBoardCrate[MAX_BOARDS];
  int fBoardSlot[MAX_BOARDS];
  int fBoardVReq[MAX_BOARDS];
  int fBoardVSet[MAX_BOARDS];
  int fBoardVTrue[MAX_BOARDS];
  int fBoardVMax[MAX_BOARDS];
  bool fBoardPower[MAX_BOARDS];
  int fBoardRampUp[MAX_BOARDS];
  int fBoardRampDown[MAX_BOARDS];
  float fBoardIMax[MAX_BOARDS];
  float fBoardCurrent[MAX_BOARDS];
  float fBoardTemp[MAX_BOARDS];
  bool fBoardActive[MAX_BOARDS];
  bool            fCrateStatus[NUM_CRATES];

  int fHVStatus;
  int fCurrentsStatus;
  int fPixelPowerStatus;
  bool fQuit;
  int fSensor[NUMSENSORS];
/*   int fCameraTemp; */
/*   int fCameraHum; */
  int fDisplayMode;
  bool fCurrentFlag;

  bool fFeedbackEnabled;
  bool fPixelFeedbackEnabled[MAX_PMTS];
  int fFeedbackLevel[MAX_PMTS];
  int fThreshold;
  int fHiThreshold;
  int fPatternThreshold;
  int fPatternMultiplicity;
  float fRestoreMargin;
  float fPatternRestoreMargin;
  int fCheckTime;
  int fRecentTime;
  int fMeasurementsPerCycle;   // Measurements of PMT current per fdbk cycle
  int fMinTimeBeforeRestore;   // Min time in seconds before allowing HV to rise
  int fNumberOfFeedbackLevels; // Number of levels in the feedback (in addition
                               // to nominal voltage and 0 V)
  float fGainScaling;          // Fractional change in gain between levels
  float fSafetyFactor;           // For voltage to rise, current must drop to 
        // < fSafetyFactor * fGainScaling^VPixel::fLevel * fThreshold
                              
  int fRestoreAttemptTime;     // Min time to wait before restoring a pixel
                               // that suppressed not due to a star
  float fMinAbsPositionChange; // Absolute change in position of pixel on sky
                               // to require before restoring pixel that 
                               // suppressed not due to a star (rad)
  float fPatternDistanceDrop;  // If a pixel is initially suppressed as part of
                               // a pattern, it must move by this distance
                               // before it's no longer considered part of a
                               // pattern (rad)
  float fPSF;                  // Approximate optical PSF of telescope.
  int fCyclesPerFeedbackTest;   // 

  float fRA;
  float fDec;
  float fElevation;
  float fAzimuth;
  float fLoopGain;
  float fPMTIndex;
  bool fZeroAmps;
  int fMaxAllowedVolts;
  float fTargetCurrent;
  int fStatusPeriod;
  bool            fLimitBoardVoltages;

};

#endif
