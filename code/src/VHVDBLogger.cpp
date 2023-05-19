// #include <cstdio>
// #include <cstdlib>
// #include <iostream> 

#include <ctime>
#include "VHVDBLogger.h"

extern bool kDBLogging;
extern char kDBLogFile[200];
extern int  kTelescopeID;

using namespace std;

VHVDBLogger::VHVDBLogger(VHVData *data, bool freeRunning ) : QObject(0,0)
{
  // Set fData.
  fData = data;
  fFreeRunning = freeRunning;
  fTimer = new QTimer( this );
  // Set filename for local text log files.
  time_t tmpTime;
  time( &tmpTime );
  strftime( fLocalTextLog, 200, 
	    "/usr/local/veritas/SP05/vhv/DBLogs/log%Y%m%d.log", 
	    gmtime( &tmpTime ) );
  strcpy( kDBLogFile, fLocalTextLog );
  cout << "T" << kTelescopeID+1
       << ":  VHVDBLogger::VHVDBLogger():  Local Text Log filename is " 
       << fLocalTextLog << endl;
  connect(fTimer, SIGNAL( timeout() ),
	  this, SLOT( logStatusToDB2() ) );
  if ( fFreeRunning ) {
    fTimer->start( (int)60e3, FALSE );
  }
}

VHVDBLogger::~VHVDBLogger() 
{
  delete fTimer;
}

// void VHVDBLogger::run() 
// {
// //   cout << "In VHVDBLogger::run" << endl;
// //   try {
// //     while( TRUE ) {
// //       msleep( (int)60e3 );
// //       logStatusToDB2();
// //     }
// //   }
// //   catch(VException & e) {
// //     cerr << e << endl;
// //   }
// }


/** Logs HV settings and readback values to the Database. **/
void VHVDBLogger::logStatusToDB2()
{
  ofstream logtime("/usr/local/veritas/SP05/vhv/DBLogs/logStatusToDBTime.log", 
		   ios::app);
  ofstream logfile( fLocalTextLog, ios::app);
  time_t tmpTime;
  time( &tmpTime );
  char tmpString[200];
  strftime( tmpString, 200, "%Y%m%d %T", gmtime( &tmpTime ) );
  long tStart, tEnd, tDiff;
  float voltages[fData->getNumberOfPMTs()];
  float currents[fData->getNumberOfPMTs()];
  if ( kDBLogging ) {
    tStart=time(0);
    for (int chan=0; chan<fData->getNumberOfPMTs(); chan++) {
      voltages[chan] = fData->getVTrue( chan );
      currents[chan] = fData->getCurrent( chan );
      logfile << "status\t" << time( 0 ) << "\t" << kTelescopeID+1 << "\t" 
	      << chan+1 << "\t" 
	      << voltages[chan] << "\t" << currents[chan] 
	      << "\t" << tmpString << endl;
    }
    try {
      VDBHV::putHVArrayStatus( kTelescopeID, 1, 499, voltages, currents );
    }
    catch(VException & e) {
      cerr << e << endl;
    }
    for ( int bd=0; bd<fData->getNumberOfBoards(); bd++ ) {
      logfile << "board status\t" << time( 0 ) 
	      << "\t" << kTelescopeID 
	      << "\t" << fData->getBoardCrate( bd ) 
	      << "\t" << fData->getBoardSlot( bd )
	      << "\t" << fData->getBoardVTrue( bd ) 
	      << "\t" << fData->getBoardCurrent( bd )
	      << "\t" << fData->getBoardTemp( bd ) 
	      << "\t" << tmpString << endl;
    }
    logfile << "camera temp\t" << time( 0 ) << "\t" 
	    << fData->getCameraTemp() << "\t" << tmpString << endl;
    logfile << "camera hum\t" << time( 0 ) << "\t"
	    << fData->getCameraHum() << "\t" << tmpString << endl;
    tEnd=time(0);
    tDiff = tEnd - tStart;
    logtime << "Start:  " << tStart << "  End:  " << tEnd 
	    << "  Diff:  " << tDiff << endl;
    logtime.close();
    logfile.close();
    try {
      VDBCAM::putCameraStatus( kTelescopeID, 
			       fData->getSensor( SENSOR_CAMTEMP ), 
			       fData->getSensor( SENSOR_AMBTEMP ), 
			       fData->getSensor( SENSOR_CAMHUM ), 
			       fData->getSensor( SENSOR_AMBHUM ), 
			       fData->getSensor( SENSOR_LIGHT1 ), 
			       fData->getSensor( SENSOR_LIGHT2 ) );
    }
    catch(VException & e) {
      cerr << e << endl;
    }
  }
  
}

/** Logs HV settings and readback values for one channel to the Database. **/
void VHVDBLogger::logOneStatusToDB( int chan )
{
  //   ofstream logtime("/usr/local/veritas/SP05/vhv/DBLogs/logStatusToDBTime.log", 
  // 		   ios::app);
  ofstream logfile( fLocalTextLog, ios::app);
  time_t tmpTime;
  time( &tmpTime );
  char tmpString[200];
  strftime( tmpString, 200, "%Y%m%d %T", gmtime( &tmpTime ) );
  float voltage;
  float current;
  if ( kDBLogging ) {
    voltage = fData->getVTrue( chan );
    current = fData->getCurrent( chan );
    logfile << "status\t" << time( 0 ) << "\t" << kTelescopeID+1 << "\t" 
	    << chan+1 << "\t" 
	    << voltage << "\t" << current 
	    << "\t" << tmpString << endl;
    try {
      VDBHV::putHVStatus( kTelescopeID, chan+1, voltage, current );
    }
    catch(VException & e) {
      cerr << e << endl;
    }
    logfile.close();
  }
  
}

