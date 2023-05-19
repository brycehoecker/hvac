// VNETFunctionsServer.h
//=======================================================================||
//                                                                       ||
//	Sample CORBA server stuff - object implementation      		 ||
//									 ||
//      J. Kildea 26 JAN 04                                              ||
//=======================================================================||
#include "VNETFunctions.hh"// objects for which CORBA interface is defined 
#include "omniORB4/CORBA.h"
#include "VHVData.h"
#include "VHVControl.h"
#include "VHVSystem.h"
#include "qobject.h"

using namespace std;

//===================================================================
// Below follow the object classes.
// They all inherent from:
// Their POA_objectname (specified in the .hh file for that object)
// PortableServer::RefCountServantBase. Counts number of object connections
//===================================================================
class VNETTransfer_i : 
public QObject, 
public POA_VHVCommunications::VNETTransfer,
public PortableServer::RefCountServantBase
{
  Q_OBJECT
  
 public:
  VNETTransfer_i( VGatherData *gather, 
		  VHVData *data );
  virtual ~VNETTransfer_i();

  virtual void pixelSetVReq( short int pixel, float voltage );
  virtual float pixelGetVReq( short int pixel );
  virtual float pixelGetVSet( short int pixel );
  virtual float pixelGetVTrue( short int pixel );
  virtual void allSetHV( float voltage );
  virtual void allOffsetHV( float voltage );
  virtual void allScaleHV( float percent );
  virtual VHVCommunications::VHVFloatSeq* allGetVReq();
  virtual VHVCommunications::VHVFloatSeq* allGetVSet();
  virtual VHVCommunications::VHVFloatSeq* allGetVTrue();

  virtual void writeVoltagesToDB();
  virtual void readVoltagesFromDB();

  virtual void allOn();
  virtual void allOff();
  virtual VHVCommunications::VHVBoolSeq* allGetPowerStatus();
  virtual bool pixelGetPowerStatus( short int pixel );
  virtual void pixelOn( short int pixel );
  virtual void pixelOff( short int pixel );

  virtual void pixelSuppress( short int pixel );
  virtual void pixelRestore( short int pixel );
  virtual void allSuppress();
  virtual void allRestore();
  virtual void restoreRecent();
  bool pixelGetSuppressionStatus( short int pixel );
  VHVCommunications::VHVBoolSeq* allGetSuppressionStatus();

  virtual void loggingOn();
  virtual void loggingOff();
  bool getLoggingState();
  virtual void alive();
  virtual void quitVHV();
  virtual void connectToHVAC();
  virtual void disconnectFromHVAC();

  virtual void markPixelDisabled( short int pixel );
  virtual void unmarkPixelDisabled( short int pixel );
  bool pixelGetDisabledStatus( short int pixel );
  VHVCommunications::VHVBoolSeq* allGetDisabledStatus();
  virtual void markPixelNoAuto( short int pixel );
  virtual void unmarkPixelNoAuto( short int pixel );
  bool pixelGetAutoStatus( short int pixel );
  VHVCommunications::VHVBoolSeq* allGetAutoStatus();

  virtual void setAutoResponseParams( const VHVCommunications::autoParams& params );
  VHVCommunications::autoParams getAutoResponseParams();

 public:
  VNETTransfer_i( VHVControl *control, VHVData *data );//constructor
  virtual ~VNETTransfer_i();//destructor

  virtual void pixelSetHV( int short pixel, float voltage );
  virtual float pixelGetHV( int short pixel );
  virtual void allSetHV( float voltage );
  virtual void allOffsetHV( float voltage );
  virtual void allScaleHV( float percent );

  virtual void writeVoltagesToDB();
  virtual void readVoltagesFromDB();

  virtual void allOn();
  virtual void allOff();
  virtual void pixelOn( int short pixel );
  virtual void pixelOff( int short pixel );
  virtual void pixelSuppress( int short pixel );
  virtual void pixelRestore( int short pixel );
  virtual void allSuppress();
  virtual void allRestore();
  virtual void restoreRecent();

  virtual void loggingOn();
  virtual void loggingOff();
  virtual void alive();
  virtual void quitVHV();
  virtual void connectToHVAC();
  virtual void disconnectFromHVAC();

  virtual void markPixelDisabled( int short pixel );
  virtual void unmarkPixelDisabled( int short pixel );
  virtual void markPixelNoAuto( int short pixel );
  virtual void unmarkPixelNoAuto( int short pixel );

  virtual void setTelescopeTarget( int short targetIndex );
  virtual void setRA( float ra );
  virtual void setDec( float dec );






  virtual int short getHVStatus();//0=off;1=on
  virtual void setPixelStatus(int short pixel, int short status);//0=off;1=on;2=bad
  virtual void sendCurrent(int short pixel, float value);//fill an array 1 at a time
  virtual void updateHVDisplay();//notify QT event loop once all values have been sent
  virtual void setPixelLow(int short pixel);//turn down bright tube
  virtual void restorePixel(int short pixel); // restore voltage to tube
  virtual void killPixel(int short pixel);//turn off a tube
  virtual void killAll();//When things go real bad
  //        virtual float getVal(int short i);
  //        virtual void setHVStatus(int short value);
  

  void saveHV( const char filename[] );

  virtual void globalFeedbackReset();
  virtual void pixelFeedbackReset( short int ch );
  virtual void globalFeedbackOn();
  virtual void pixelFeedbackOn( short int ch );
  virtual void globalFeedbackOff();
  virtual void pixelFeedbackOff( short int ch );
  virtual void globalRefreshVoltageLevels();
  virtual bool isGlobalFeedbackEnabled();
  virtual bool isPixelFeedbackEnabled( short int ch );
  VHVCommunications::VHVBoolSeq* isAllPixelFeedbackEnabled();
  VHVCommunications::VHVShortSeq* getFeedbackLevel();

 signals:
  void printMessage(const QString & string);

 private:
  int x;
/*   short int pixel_status[499]; */
  float fPMTCurrent[499];
/*   short int HVStatus;//0=off, 1=on */
  VHVData *fData;
  VHVControl *fControl;
};
