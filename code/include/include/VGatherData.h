///////////////////////////////////////////////////////////////////////////////
// $Id: VGatherData.h,v 1.16 2007/09/22 04:13:25 humensky Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#ifndef VGATHERDATA_H
#define VGATHERDATA_H

/** \file VGatherData.h
 *  \brief Header file for the VGatherData class (defined in VGatherData.cpp) 
 
 * This class runs as a thread - gathering data from the HV system and also
 * from the current monitor process and placing information in a VHVData 
 * object.
*/

#include <cmath>

#include <qobject.h>
#include <qthread.h>
#include <qwidget.h>
#include <qmessagebox.h>
#include <qtimer.h>

#include <VDBHighVoltage.h>
#include <VDBArrayControl.h>
#include <VDBPositioner.h>

#include "VHV.h"
#include "VHVSystem.h"
#include "VHVData.h"
#include "VHVPST.h"
#include "VHVDBLogger.h"
#include "txmlclient.h"
#include "tclienthandler.h"
#include "tdevicedata.h"
#include "astrometry.h"
#include "VeritasCam.h"

#include "VAAzElRADecXY.h"
#include "VATime.h"
#include "VACommon.h"
#include "VASlalib.h"

class VPixel;
struct VStarInfo{
  double fRA;
  double fDec;
  double fBMag;
  double fVMag;
}; 

//static const char kIPAddress[20] = "192.33.141.33";
//static const char kCurrentsHost[30] = "cvc.sao.arizona.edu";
//static const char kIPAddress[20] = "128.135.70.177";
//static const char kCurrentsHost[30] = "gritstone.uchicago.edu";
/* Comment next two lines out since no longer in High Bay 10/29/03 */
/* static const char kIPAddress[20] = "128.135.102.88"; */
/* static const char kCurrentsHost[30] = "veritas.uchicago.edu"; */
/* Add these two lines to use vhv in Lab at UChicago 10/29/03 */
// static const char kIPAddress[20] = "128.135.70.177";             // UC
// static const char kCurrentsHost[30] = "veritashv1.uchicago.edu"; // UC
/* static const char kIPAddressCrate0[20] = "10.0.20.117";              // UC */
/* static const char kIPAddressCrate1[20] = "10.0.20.118";              // UC */
/* static const char kIPAddressCrate0[20] = "128.135.61.197";              // UC */
/* static const char kIPAddressCrate1[20] = "128.135.61.198";              // UC */
/* static const char kCurrentsHost[30]    = "10.0.20.112";                   // UC */

/** \bool kDBLogging
 *  Flag for logging to Database.  Default is off (false).
 */

class VGatherData : public QObject, public QThread  {

Q_OBJECT
  
 public:
  VGatherData(VHVData *data, QWidget* grandparent );
  virtual ~VGatherData(); 
  void init();
  bool getTransientCheck( int index );
  void setTransientCheck( int index, bool check );
  void setTransientTime( int time );
  void setCurrentThreshold( int threshold ); 
  int getTransientCheckTime();
  int getCurrentThreshold();
  void updateCurrents();
  void feedbackDownwards();
  void feedbackUpwards();
  void updateHV(); 
  void logVoltageChange( int ch );
  void logAllVoltageChange();
  void logPowerOn( int ch );
  void logAllPowerOn();
  void logPowerOff( int ch );
  void logAllPowerOff();
  double Time( struct timeval time1 );
  double TimeDiff( struct timeval, struct timeval );


  int fNumberOfPixelWarningsOpen;
  VPixel* getPixel( int pixel );
 
 protected:
  void run();

 signals:
  void newData(); /**< signal the arrival of new data */
  void printMessage(const QString & string);

 public slots:
  void setMode(int mode);

 private:
  
  void processChannelLogging( int ch );
  void processCameraLogging();
  void voltageMonitor();
  bool checkCurrentsChanged();

  VHVData        *fData;
  VHVDBLogger    *fLogger;
  VHVPST         *fPST;
  VHVSystem      *fHV[NUM_CRATES];
  bool            fTransientCheck[MAX_PMTS];
  int             fTransientCheckTime;
  VPixel         *fPixel[MAX_PMTS];
  int             fHoldoffTime[MAX_PMTS];
  int             fHiThreshExceedCounter;
  bool            fHiThreshExceedFlag;
  float           fVmean[MAX_PMTS];
  float           fVrms[MAX_PMTS];
  int             fVMcounter;
  QTimer         *fAllPowerStatusTimer;
  QTimer         *fAllVoltageStatusTimer;
  float           fLastCurrent[MAX_PMTS];
  int             fLastChangeTime[MAX_PMTS];
  int             fLastCurrentWarningTime;
};


class VPixel : public QObject {
  
  Q_OBJECT
    
    public:
  VPixel(int pixel, VGatherData* gather, VHVData* data, 
	 int transientCheckTime, int currentThreshold, 
	 QWidget* ggparent, VHVDBLogger *logger );
  virtual ~VPixel();
  void startTimer();
  int getCurrentThreshold();
  int getTransientCheckTime();
  int getPixel();
  void setCurrentThreshold( int currentThreshold );
  void setTransientCheckTime( int transientCheckTime );
  void setPixel( int pixel );
  long int getSuppressTime();
  void resetSuppressTime();
  void suppress();
  void recordStatus();
  bool checkCurrentsChanged(); // returns false if not changed in last 10 sec.

  void reset(); // resets feedback completely
  void resetWithNewVoltage(); // resets feedback, updating to reflect a new
                              // user-set VReq (keeps voltage <= where it was)
  void resetUseNewVoltage(); // resets feedback, updating to reflect a new
                             // user-set VReq and uses that VReq
  void kill();  // kills voltage
  void feedbackDownwards(); // updates currents and decides whether to 
                            // drop voltage
  void feedbackUpwards();   // Checks whether voltage should be raised on pixel
  void setPatternMember( bool p );
  bool isPatternMember();
  void setFeedbackEnabled( bool f );
  bool isFeedbackEnabled();
  void refreshVoltageLevels( bool dropIfNecessary = true );
  void printStatus( ostream& creek = cout );
  void starSearch();
  void readStars();
  void refreshPSF();
  int getLevel();
 signals:
  void printMessage(const QString & string);
  
  public slots:
    void checkAndSuppress();
  void checkStar();
  void logStatus();

 private:
  int fTransientCheckTime;
  int fCurrentThreshold;
  long int fSuppressTime;
  QTimer *fAutoTimer;
  QTimer *fStatusTimer;
  QTimer *fStarCheckTimer;
  int fPixel;
  VGatherData *fGather;
  VHVData *fData;
  VHVDBLogger *fLogger;
  QWidget* fGreatGrandParent;
/*   QMessageBox* fSuppressMessage; */
  VDBPOS::StarInfo fStarInfo;
  double           fSafeDistance;

  bool             fFeedbackEnabled;
  bool             fPatternMember;
  bool             fStarPresent;
  int              fLevel;
  vector< int >    fVLevel;
  VDBPOS::StarInfo fStar;
  vector< VStarInfo >  fStarsTycho;
  VStarInfo        fStarTycho;
  deque< double >   fCurrentsHistory;
  double            fSumCurrent;
  double            fAveCurrent;
  int              fTimeOfLastDrop;
  int              fKillTime;
  VATime           fVAKillTime;
  VATime            fNow;
  double            fDistanceAtLastDrop;
  double            fRAAtLastDrop;
  double            fDecAtLastDrop;
  double            fRA;
  double            fDec;
  double            fPSF;
  VAAzElRADecXY   *fTrans;

};

#endif
