///////////////////////////////////////////////////////////////////////////////
// $Id: tdevicedata.cpp,v 1.1.1.1 2006/01/18 20:18:51 ergin Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#include "tdevicedata.h"
#include <qstring.h>
#include <cstdio>
#include <iostream>

extern bool kTestCurr;

using namespace std;
/*!
\class TDeviceData 
\brief The TimeDeviceData is derived from the DeviceData class.

This derived class adds functionality specific to storing Time information on the client side.
*/

/**  Constructor. */
TDeviceData::TDeviceData(VHVData *data) : QWidget()
{

  fData = data;

}
/*!
Destructor
*/
TDeviceData::~TDeviceData()
{

}

/*! The time data is passed as a QString. */
void TDeviceData::setTimeString( const QString& time )
{
  fTime = time.latin1();
}

/** The PMT data is filled and the data changed signal from the base class
 * TDeviceData is emitted.
 */
void TDeviceData::setPMT( int channel, float current, int status )
{
  if ( kTestCurr ) {
    if ( fData->getPower( channel ) &&
	 fData->getSuppressed( channel ) == RESTORED ) {
      fData->setCurrent(channel, current);
    } else {
      fData->setCurrent( channel, 0 );
    }
  } else {
    fData->setCurrent(channel, current);
  }
//     printf("XML: %d %f\n",channel,current);
//    fprintf(stdout, "XML: pixel %d\t current %f\n",channel,current);

}


void TDeviceData::setTemp( int temp )
{
//   cout << "TDeviceData: setting temp: " << temp << endl;
  fData->setCameraTemp( temp );
}


void TDeviceData::setHum( int hum )
{
//   cout << "TDeviceData: setting hum: " << hum << endl;
  fData->setCameraHum( hum );
}


void TDeviceData::setSensor( int sensor, int value )
{
//   cout << "TDeviceData: setting hum: " << hum << endl;
  fData->setSensor( sensor, value );
}


/*!
Returns the current time value
*/
const QString& TDeviceData :: getTime()
{
  return fTime;
}
