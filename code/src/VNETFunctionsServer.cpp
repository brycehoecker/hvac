//  VNETFunctionsServer.cpp
//=======================================================================||
//                                                                       ||
//	Sample CORBA server stuff - object implementation		 ||
//									 ||
//	see header file for description 				 ||
//                                                                       ||
//      J. Kildea 26 JAN 04                                              ||
//modified for imon by mkd 8/9/04
//=======================================================================||
// project specific includes
#include "VNETFunctionsServer.h"
#include "VHV.h"

// standard includes
#include <iostream>
#include <fstream>

using namespace std;

extern bool kDBLogging;
extern int kTelescopeID;
extern bool kAutoResponse;
extern bool kStarRestore;
extern char kBrokenPixelFile[200];
extern char kNoAutoPixelFile[200];
extern bool kOldSchool;

// Constructor
VNETTransfer_i::VNETTransfer_i( VHVControl *control, VHVData *data ) {
  fData = data;
  fControl = control;
  for( int short i=0; i<499; i++ ) {
//     fPixelStatus[i]=0;
    fPMTCurrent[i]=0.;
  }
} // end constructor


// Destructor
VNETTransfer_i::~VNETTransfer_i() {
} // end destructor


// Function to set a pixel's HV.
void VNETTransfer_i::pixelSetHV( in short pixel, in float voltage )
{
}


// Function to get a pixel's HV.
float VNETTransfer_i::pixelGetHV( in short pixel )
{
}


// Function to set all pixels' HV.
void VNETTransfer_i::allSetHV( in float voltage )
{
}


// Function to offset all pixels' HV.
void VNETTransfer_i::allOffsetHV( in float voltage )
{
}


// Function to scale all pixels' HV.
void VNETTransfer_i::allScaleHV( in float percent )
{
}


// Function to turn on all pixels.
void VNETTransfer_i::allOn(void)
{
  fControl->allOn();
}


// Function to turn off all pixels.
void VNETTransfer_i::allOff(void)
{
  fControl->allOff();
}


// Function to turn on one pixel.
void VNETTransfer_i::pixelOn( int short pixel )
{
  fData->setPower( pixel, ON, EUser );
  fControl->getGatherData()->logPowerOn( pixel );
}


// Function to turn off one pixel.
void VNETTransfer_i::pixelOff( int short pixel )
{
  fData->setPower( pixel, OFF, EUser );
  fControl->getGatherData()->logPowerOff( pixel );
}


// Function to suppress one pixel.
void VNETTransfer_i::pixelSuppress( int short pixel )
{
  int restore_voltage = fData->getVSet( pixel );
  fData->initSuppressed( pixel, SUPPRESSED );
  fData->setVSet( pixel, restore_voltage, EUser );
  fControl->getGatherData()->logVoltageChange( pixel );
}


// Function to restore one pixel.
void VNETTransfer_i::pixelRestore( int short pixel )
{
  int restore_voltage = fData->getVSet( pixel );
  fData->initSuppressed( pixel, RESTORED );
  fData->setVSet( pixel, restore_voltage, EUser );
  fControl->getGatherData()->logVoltageChange( pixel );
}


// Function to suppress all pixels.
void VNETTransfer_i::allSuppress(void)
{
  fControl->suppressAll();
}


// Function to restore all pixels.
void VNETTransfer_i::allRestore(void)
{
  fControl->restoreAll();
}


// Function to restore recently suppressed pixels.
void VNETTransfer_i::restoreRecent(void)
{
  fControl->restoreRecent();
}


// Function to turn on logging to the database.
void VNETTransfer_i::loggingOn( void )
{
}


// Function to turn off logging to the database.
void VNETTransfer_i::loggingOff( void )
{
}


// Function to check presence of server.  In Client, failure of call would
// generate an exception.
void VNETTransfer_i::alive( void )
{
}


// Function to order a telescope to quit VHV.
void VNETTransfer_i::quitVHV( void )
{
}


// Function to tell a telescope to connect its client to the HVAC server.
void VNETTransfer_i::connectToHVAC( void )
{
}


// Function to tell a telescope to disconnect its client from the HVAC server.
void VNETTransfer_i::disconnectFromHVAC( void )
{
}


// Function to mark a pixel as disabled.
void VNETTransfer_i::markPixelDisabled( int short pixel )
{
}


// Function to unmark a pixel as disabled.
void VNETTransfer_i::unmarkPixelDisabled( int short pixel )
{
}


// Function to mark a pixel as excluded from autoResponse.
void VNETTransfer_i::markPixelNoAuto( int short pixel )
{
}


// Function to unmark a pixel as excluded from autoResponse.
void VNETTransfer_i::unmarkPixelNoAuto( int short pixel )
{
}


// Function to set where a telescope is pointing -- sets target as database,
// manual, or from the list.
void VNETTransfer_i::setTelescopeTarget( int short targetIndex )
{
}

// Function to set RA for manual pointing.
void VNETTransfer_i::setRA( float ra )
{
}


// Function to set Dec for manual pointing.
void VNETTransfer_i::setDec( float dec )
{
}





// Function to get status of HV system.  Returns 1 if any pixel is ON.  Returns
// 0 if all pixels are off.
int short VNETTransfer_i::getHVStatus(void)
{
  return fData->getPixelPowerStatus();
}

// Function to set the status of an individual pixel.  Status can be:
//  0  pixel off
//  1  pixel on
//  2  pixel disabled
void VNETTransfer_i::setPixelStatus(int short pixel, int short status)
{
  //this is the status in the fPMTCurrent monitor configuration file 
  //or if an ADC offset goes bad (ie <= 0).
  //   cout << pixel << " " << status << endl;
  fData->setStatus(pixel-1, status);
} // end setPixelStatus


// 20070823 TBH The following methods are obsolete and unused - should be 
//              deleted in the future.

// // Function to update QMon current information for a single pixel.
// void VNETTransfer_i::sendCurrent(int short pixel,float value)
// {
//   //Current values are sent one at a time to fill an array
//   //then a display update command is transmitted once everything has been
//   //transferred. This is possibly slow, but as update in HV is probably of order 1Hz at best
//   //this should be fine, but can be done as a separate thread in vdcmon to stop it slowing the
//   //actual readout time
//   //
//   //   if (pixel == 1) {
//   //     cout << "Value received from client: " << pixel << " " << value << endl;
//   //   }
//   fData->setCurrent(pixel-1, value);
// } // end sendCurrent

// // Function to trigger updating of the HV GUI.
// void VNETTransfer_i::updateHVDisplay( void )
// {
//   //   cout << "The QT update commands QApplication::postEvent() " << endl
//   //        << "and qApp->wakeUpGuiThread() should be placed here." << endl;
//   //   cout << "Instead I shall fill your screen with values" << endl;
//   //   for ( int short i=0; i<499; i++ ) { 
//   //     fData->setCurrent(i, fPMTCurrent[i]);
//   // //     cout << "Pixel[" << i+1 <<"] is at " << fPMTCurrent[i] << " microAmps" << endl;
//   //   } // end for loop

// } // end updateHVDisplay


// // Function to suppress a single pixel.  Set voltage is remembered so that it 
// // can be restored when the star (or whatever) has passed.
// void VNETTransfer_i::setPixelLow(int short pixel)
// {
//   //if a pixel is in a bright field (ie a star in the fov) we must lower the voltage
//   cout << "Pixel " << pixel << " needs its sunglasses" << endl;
//   fData->suppressPixel(pixel-1);
// } // end setPixelLow


// // Function to restore voltage to a single pixel.
// void VNETTransfer_i::restorePixel(int short pixel)
// {
//   fData->restorePixel(pixel-1);
// } // end restorePixel

// // Function to set voltage to zero for one pixel that's gone bad.  We can't
// // just turn its power off, unfortunately, because that would kill power to a 
// // group of eight pixels.
// void VNETTransfer_i::killPixel(int short pixel)
// {
// //if a pixel goes way high, or bad we must remove the HV to it
//   cout << "Pixel " << pixel << " is to sleep with the fishes tonight" << endl;
//   fData->setVReq(pixel, 0, EUser);
// } // end killPixel


// // Function to kill power to all PMTs.  Is this the fastest way to implement
// // it?  Note:  above function "kills" a single pixel by setting its voltage to
// // zero, but this function kills all channels by first shutting off their power
// // and then setting all voltages to zero.
// void VNETTransfer_i::killAll()
// {
// //if the whole is bright we must kill the HV to them all
//   cout << "Kill them! Kill them all!" << endl;
//   for (int i=0; i<MAX_PMTS; i++) {
//     fData->setPower(i, OFF, EUser);
//   }
//   for (int i=0; i<MAX_PMTS; i++) {
//     fData->setVReq(i, 0, EUser);
//   }
// } // end killAll


/** Save High Voltage settings to a text file.
 * \arg \c filename - Name of file to save.
 */
void VNETTransfer_i::saveHV( const char filename[] ) 
{
  ofstream outf( filename );

  int v, pixel;
  
  for (int i=0; i<fData->getNumberOfPMTs(); i++) {
    if ( fData->getDisabled( i ) ) {
      v = (int)VDBHV::getDefaultVoltage( kTelescopeID, i+1 );
    } else {
      v = fData->getVReq(i);
    }
    pixel = i+1;
    outf << setw( 3 ) << pixel << " " << v << endl;  
  }
  outf.close();
}


void VNETTransfer_i::globalFeedbackReset()
{
  for ( int i=0; i<MAX_PMTS; ++i ) {
    fGather->getPixel( i )->reset();
  }
}


// Function to trigger updating of the HV GUI.
void VNETTransfer_i::updateHVDisplay( void )
{
  fGather->getPixel( ch )->reset();
}


void VNETTransfer_i::globalFeedbackOn()
{
  for ( int i=0; i<MAX_PMTS; ++i ) {
    fGather->getPixel( i )->setFeedbackEnabled( true );
  }
  fData->setFeedbackEnabled( true );
}


void VNETTransfer_i::pixelFeedbackOn( short int ch )
{
  fGather->getPixel( ch )->setFeedbackEnabled( true );
}


void VNETTransfer_i::globalFeedbackOff()
{
  for ( int i=0; i<MAX_PMTS; ++i ) {
    fGather->getPixel( i )->setFeedbackEnabled( false );
    fGather->getPixel( i )->reset();
  }
  fData->setFeedbackEnabled( false );
}


void VNETTransfer_i::pixelFeedbackOff( short int ch )
{
  fGather->getPixel( ch )->setFeedbackEnabled( false );
  fGather->getPixel( ch )->reset();
}


void VNETTransfer_i::globalRefreshVoltageLevels()
{
  for ( int i=0; i<MAX_PMTS; ++i ) {
    fGather->getPixel( i )->refreshVoltageLevels();
  }
}


bool VNETTransfer_i::isGlobalFeedbackEnabled()
{
  return fData->isFeedbackEnabled();
}


bool VNETTransfer_i::isPixelFeedbackEnabled( short int ch )
{
  return fGather->getPixel( ch )->isFeedbackEnabled();
}


VHVCommunications::VHVShortSeq* VNETTransfer_i::getFeedbackLevel()
{
  VHVCommunications::VHVShortSeq_var level = 
    new VHVCommunications::VHVShortSeq( MAX_PMTS );
  level->length( MAX_PMTS );
  for ( int i=0; i<MAX_PMTS; ++i ) {
    level[i] = fGather->getPixel( i )->getLevel();
  }
  return level._retn();
}


VHVCommunications::VHVBoolSeq* VNETTransfer_i::isAllPixelFeedbackEnabled()
{
  VHVCommunications::VHVBoolSeq_var en = 
    new VHVCommunications::VHVBoolSeq( MAX_PMTS );
  en->length( MAX_PMTS );
  for ( int i=0; i<MAX_PMTS; ++i ) {
    en[i] = fGather->getPixel( i )->isFeedbackEnabled();
  }
  return en._retn();
}


