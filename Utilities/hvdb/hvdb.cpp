// This utility gets HV information out of the database and produces some
// diagnostic plots.

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <strstream>

#include <VDBACStruct.h>
#include <VDBArrayControl.h>
#include <VDBHVStruct.h>
#include <VDBHighVoltage.h>

using namespace std;
using namespace VDBAC;
using namespace VDBHV;

#define NUMBEROFPMTS 499

void getStatuses( unsigned long long startTime, unsigned long long endTime, 
		  vector< struct HVStatusInfo > *statusInfo );
void printStatus( int channel, unsigned long long datetime, 
		  struct HVStatusInfo statusInfo );
unsigned long long dateToSeconds( unsigned long long datetime );
unsigned long long secondsToDate( unsigned long long seconds );

int main( int argc, char *argv[] )
{
  // Process command-line arguments
  unsigned long long startTime = 0;
  unsigned long long endTime   = 0;
  if ( argc != 3 ) {
    cout << argc << " Usage: hvdb <starttime> <endtime>" << endl;
    exit( 1 );
  } else {
    istringstream istr1( argv[1] );
    istringstream istr2( argv[2] );
    istr1 >> startTime;
    istr2 >> endTime;
  }
  if ( startTime != 0 ) {
  cout << "Start Time:  " << startTime << endl;
  } else {
    cout << "Start Time = 0!  Keptain, we have trouble!" << endl;
  }
  if ( startTime != 0 ) {
  cout << "  End Time:  " << endTime << endl;
  } else {
    cout << "End Time = 0!  Keptain, we have trouble!" << endl;
  }

  // Declare some HVStatusInfo structures
  struct HVStatusInfo tmpInfo;
  vector< struct HVStatusInfo > statusInfo;

  // Get status information for the desired time period
  getStatuses( startTime, endTime, &statusInfo );

}

void getStatuses( unsigned long long startTime, unsigned long long endTime, 
		  vector< struct HVStatusInfo > *statusInfo )
{
  struct HVStatusInfo tmp;
  unsigned long long sTsec = dateToSeconds( startTime );
  unsigned long long eTsec = dateToSeconds( endTime );
  if ( eTsec < sTsec ) {
    cout << "End time before start time! Exitting." << endl;
    exit( 1 ) ;
  }
  try {
    for ( int j=sTsec;
	  j<eTsec; 
	  j+=60 ) {
      for ( int i=0; i<NUMBEROFPMTS; i++ ) {
	cout << "getting status." << endl;
	tmp = getHVStatus( 0, i+1, startTime );
	cout << "got status." << endl;
	statusInfo->push_back( tmp );
	cout << "copied status.  printing. " << endl;
// 	printStatus( i+1, startTime, statusInfo->back() );
	cout << "printed." <<endl;
      }
    }
  }
  catch(VException & e) {
        cerr << e << endl;
  }
  
}


void printStatus( int channel, unsigned long long datetime, 
		  struct HVStatusInfo statusInfo )
{
  cout<<"Retrieved status info for channel "<<channel<<" at time: "<<datetime<<endl;
  cout<<"voltage_measured: "<<statusInfo.voltage_measured<<endl;
  cout<<"voltage_target: "<<statusInfo.voltage_target<<endl;
  cout<<"current_measured: "<<statusInfo.current_measured<<endl;
  cout<<"is power ON: "<<statusInfo.isPowerOn<<endl<<endl;
}

unsigned long long dateToSeconds( unsigned long long datetime )
{
  int yr=0, mo=0, day=0, hr=0, min=0, sec=0;
  ostrstream buffer;
  buffer << datetime;
//   istringstream istr( buffer.str().c_str() );
  string dt = buffer.str();
//   istr >> dt;
  if ( dt == "a" ) {
    cout << "dt = " << dt << "!  That's a problem." << endl;
  } else {
    yr =  atoi( dt.substr( 0, 4 ).c_str() );
    mo =  atoi( dt.substr( 4, 2 ).c_str() );
    day = atoi( dt.substr( 6, 2 ).c_str() );
    hr =  atoi( dt.substr( 8, 2 ).c_str() );
    min = atoi( dt.substr( 10, 2 ).c_str() );
    sec = atoi( dt.substr( 12, 2 ).c_str() );
  }

  return 60*( 60*( 24*day + hr ) + min ) + sec;
}

unsigned long long secondsToDate( unsigned long long seconds )
{
}
