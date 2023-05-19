// VNETfunctionsServerThread.h 
//=======================================================================||
//                                                                       ||
//	Example CORBA server stuff - thread for server to run in	 ||
//									 ||
//                                                                       ||
//      J. Kildea 26 JAN 04                                              ||
//=======================================================================||
/* #include <zthread/Thread.h> */
#include "omniORB4/CORBA.h"
#include <iostream>           //for writing to screen, but not disk
#include <string>             //for beans
#include <stdlib.h>           //for rand()
#include <unistd.h>           //for sleep()
#include <exception>

#include <qobject.h>
#include <qthread.h>

#include "VNETFunctions.hh"
#include "VHV.h"
#include "VHVData.h"

using namespace std;

/* class VNETClientThread : public QObject, public QThread  */
class VNETClientThread : public QObject
{
  Q_OBJECT    

    public:
  VNETClientThread( int telID,
		    VHVData *data ) throw();
  ~VNETClientThread() throw() {};
  void begin() throw();
  void done();

  void pixelSetVReq( short int pixel, float voltage );
  void pixelGetVReq( short int pixel );
  void pixelGetVSet( short int pixel );
  void pixelGetVTrue( short int pixel );
  void allSetHV( float voltage );
  void allGetVReq();
  void allGetVSet();
  void allGetVTrue();
  void allOffsetHV( float voltage );
  void allScaleHV( float percent );

  void writeVoltagesToDB();
  void readVoltagesFromDB();
  //   void writeHVFile( in string filename ); // syntax!
  //   void readHVFile( in string filename );  // syntax!

  void allOn();
  void allOff();
  void allGetPowerStatus();
  void pixelGetPowerStatus( short int pixel );
  void pixelOn( short int pixel );
  void pixelOff( short int pixel );

  void pixelSuppress( short int pixel );
  void pixelRestore( short int pixel );
  void allSuppress();
  void allRestore();
  void restoreRecent();
  void pixelGetSuppressionStatus( short int pixel );
  void allGetSuppressionStatus();

  void loggingOn();
  void loggingOff();
  void getLoggingState();
  void alive();
  void quitVHV();
  void connectToHVAC();
  void disconnectFromHVAC();

  void markPixelDisabled( short int pixel );
  void unmarkPixelDisabled( short int pixel );
  void pixelGetDisabledStatus( short int pixel );
  void allGetDisabledStatus();
  void markPixelNoAuto( short int pixel );
  void unmarkPixelNoAuto( short int pixel );
  void pixelGetAutoStatus( short int pixel );
  void allGetAutoStatus();

  void setAutoResponseParams();
  void getAutoResponseParams();

  void setFeedbackParams();
  void getFeedbackParams();

  void setTelescopeTarget( short int targetIndex );
  void setRA( float ra );
  void setDec( float dec );
  void takeStarControl();
  void releaseStarControl();
  //   void showStars();
  //   void hideStars();
  //   void showArcs();
  //   void hideArcs();
  //   vector< any > getStarList(); // syntax!

  void getNumberOfPMTs();
  void getNumberOfBoards();
  void getNumberOfCrates();
  void getCrateStatus( short int crate );
  void getHVCrate();
  void getHVSlot();
  void getHVChannel();
  void getStatus();
  void getCurrent();
  void getBoardCrate();
  void getBoardSlot();
  void getBoardVReq();
  void getBoardVSet();
  void getHWStatus();
  void setBoardVReq( short int board, float voltage );
  void getBoardVTrue();
  void getBoardVMax();
  void getBoardPower();
  void setBoardPower( short int board, bool powerState );
  void setAllBoardPower( bool powerState );
  void getBoardRampUp();
  void getBoardRampDown();
  void setBoardRampUp( short int board, short int ramp );
  void setAllBoardRampUp( short int ramp );
  void setBoardRampDown( short int board, short int ramp );
  void setAllBoardRampDown( short int ramp );
  void getBoardIMax();
  void setBoardIMax( short int board, float iMax );
  void setAllBoardIMax( float iMax );
  void getBoardCurrent();
  void getBoardTemp();
  void getBoardActive();
  void setBoardActive( short int board, bool active );
  void setAllBoardActive( bool active );
  void getCurrentsStatus();
  void getCameraTemp();
  void getCameraHum();
  void getSensor( short int id );
  void getMessages();
  void globalFeedbackReset();
  void pixelFeedbackReset( short int ch );
  void globalFeedbackOn();
  void pixelFeedbackOn( short int ch );
  void globalFeedbackOff();
  void pixelFeedbackOff( short int ch );
  void globalRefreshVoltageLevels();
  void isGlobalFeedbackEnabled();
  void isPixelFeedbackEnabled( short int ch );
  void isAllPixelFeedbackEnabled();
  void getFeedbackLevel();
  void getStatusPeriod();
  void setStatusPeriod();
   
  bool getStateLimitBoardVoltages();
  void enableLimitBoardVoltages();
  void disableLimitBoardVoltages();

  void getHVStatus();//determine if HV is off before reading ADC offsets
  void setPixelStatus(short int pixel, short int status);//send pixel status according
                                                         //0=off
                                                         //1=on
                                                         //2=bad
  void sendCurrent(short int pixel, float value);//send values across 1 by 1
  void updateHVDisplay();//once the pixel value array has been filled
  //send a command to introduce the display update into the
  //QT event loop. (not currently used)
  void setPixelLow(short int pixel);//turn down a pixel in a bright field
  void restorePixel(short int pixel);//restore a low pixel (not currently used)
  void killPixel(short int pixel);//turn off a pixel
  void killAll();//turn them all off and hope for the best
  //    void setHVStatus(short int value);//could add this to ensure HV not switched on whilst imon not reading?

 signals:
  void printMessage(const QString & string);

 private:
  CORBA::Object_ptr getObjectReference(CORBA::ORB_ptr orb);

  CORBA::ORB_var fOrb;
  VHVCommunications::VNETTransfer_var fNETTransfer_ref;
  int fTelescopeID;
  VHVData *fData;
  string fTelMessColor[NUMTEL];

};

