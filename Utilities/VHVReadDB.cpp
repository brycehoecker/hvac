#include "VHVReadDB.hpp"

using namespace std;
using namespace VDBHV;

// Time format is yyyymmddhhmmss.  Eg, 20050306123510 is March 6, 2005 at 
// 12:35:10 GMT.

int main()
{

  try {

    unsigned long long datetime;
    long tStart; 
    long tEnd; 
    long tDiff;
    float voltages[NUM_PMTS];
    float currents[NUM_PMTS];
    string power[NUM_PMTS];
    ofstream outFile;
    outFile.open( "dbout.txt", ios::out );

    // Read settings at a fixed time for all channels.
    datetime = 20050307040000;
    tStart = time( 0 );
    for ( int i=0; i<NUM_PMTS; i++ ) {
      voltages[i] = getVoltage( TELESCOPE_ID, i, datetime );
      power[i] = getPowerStatus( TELESCOPE_ID, i, datetime );
    }
    tEnd = time( 0 );
    tDiff = tEnd - tStart;

    for ( int i=0; i<NUM_PMTS; i++ ) {
      outFile << "Pix " << i+1 << ": " << voltages[i] << " V\t" << power[i] 
	   << endl;
    }
    outFile << "Took " << tDiff << " seconds to read out." << endl;

    // Read settings at many times for one channel.
    datetime = 20050427230000;
    unsigned long long requesttime;
    float vset[300];
    tStart = time( 0 );
    for ( int i=0; i<300; i++ ) {
      requesttime = datetime + 60*i;
      vset[i] = getVoltage( TELESCOPE_ID, 484, requesttime );
    }
    tEnd = time( 0 );
    tDiff = tEnd - tStart;
    for ( int i=0; i<300; i++ ) {
      outFile << "Pix 5 time: " << datetime+60*i 
	      << " vset: " << vset[i] << endl;
    }
    outFile << "Took " << tDiff << " seconds to read out." << endl;



  }
  catch(VException & e) {
    cerr << e << endl;
  }

  return 0;
}
