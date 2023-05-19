// project specific includes
#include "VHVArrayFunctions.h"

// standard includes
#include <iostream>
#include <fstream>

using namespace std;


//===================================================================
// The commands go here
//===================================================================

// Constructor
VHVArrayFunctions_i::VHVArrayFunctions_i( VHVData *data, VHVControl *control, 
		      Vcorba::corba_server *corbaServer )
{
  fData = data;
  fControl = control;
  fCorbaServer = corbaServer;
} // end Constructor


// Destructor
VHVArrayFunctions_i::~VHVArrayFunctions_i() 
{
} // end Destructor

/** Function to quit the HV program.
 */
void VHVArrayFunctions_i::quitVHV()
{
    std::cout << "before shutdown" << std::endl;
    fData->setQuit();
    fCorbaServer->shutdown(false);
    std::cout << "after shutdown" << std::endl;
} // end quitVHV()

/** Function to load production HV settings from the Database.
 */
void VHVArrayFunctions_i::loadProductionHV()
{
  cout << "Loading Production HV settings from Database" << endl;

} // end loadProductionHV()


/** Function to save production HV settings to the Database.
 */
void VHVArrayFunctions_i::saveProductionHV()
{
  cout << "Saving Production HV settings to Database" << endl;

} // end saveProductionHV()


/** Function to load a file of HV settings into vhv.
 */
// void VHVArrayFunctions_i::loadHVFile( CORBA::String_var hv_filename )
// {
//   QString filename(hv_filename);
//   if (!filename.isNull()) {
//     file = fopen( filename.latin1(), "r");
//     fControl->loadHV( file );
//     fclose( file );
//   }
// } // end loadHVFile()


/** Function to save a file of HV settings to disk.
 */
// void VHVArrayFunctions_i::saveHVFile( CORBA::String_var hv_filename )
// {
//   QString filename(hv_filename);
//   if (!filename.isNull()) {
//     file = fopen( filename.latin1(), "w");
//     fControl->saveHV( file );
//     fclose( file );
//   }
// } // end saveHVFile()


/** Function to turn on all pixels.
 */
void VHVArrayFunctions_i::allON()
{
  fControl->allOn();
} // end allON()


/** Function to turn off all pixels.
 */
void VHVArrayFunctions_i::allOFF()
{
  fControl->allOff();
} // end allOFF()


/** Function to turn on a particular pixel.
 */
void VHVArrayFunctions_i::pixelON( int short pixel )
{
    fData->setPower(pixel,ON,EUser);
} // end pixelON()


/** Function to turn off a particular pixel.
 */
void VHVArrayFunctions_i::pixelOFF( int short pixel )
{
    fData->setPower(pixel,OFF,EUser);
} // end pixelOFF()


/** Function to suppress a particular pixel.  Set voltage is remembered but 
 *  changed to zero; used for disabling individual pixels since they turn off
 *  in groups of eight.
 */
void VHVArrayFunctions_i::pixelSuppress( int short pixel )
{
  fData->suppressPixel( pixel );
} // end pixelSuppress()


/** Function to restore a suppressed pixel.
 */
void VHVArrayFunctions_i::pixelRestore( int short pixel )
{
  fData->restorePixel( pixel );
} // end pixelRestore()


/** Function to set a particular pixel's HV.
 */
void VHVArrayFunctions_i::pixelSetHV( int short pixel, float voltage )
{
  fData->setVSet( pixel, voltage, EUser );    
} // end pixelSetHV()


/** Function to turn on logging to the Database.
 */
void VHVArrayFunctions_i::loggingON()
{

} // end loggingON()


/** Function to turn off logging to the Database.
 */
void VHVArrayFunctions_i::loggingOFF()
{

} // end loggingOFF()

// Function to check presence of server.  In Client, failure of call would
// generate an exception.
void VHVArrayFunctions_i::alive()
{

} // end alive()
