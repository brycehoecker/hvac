///////////////////////////////////////////////////////////////////////////////
// $Id: VHVControl.cpp,v 1.4 2006/03/30 17:11:40 humensky Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
/** \file VHVControl.cpp
 * \brief File for definition of VHVControl and related classes
 */

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <iostream>  // added 111303 tbh
#include <fstream>

#include <qdatetime.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qimage.h>
#include <qlayout.h>
#include <qtextview.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>

#include "VHVControl.h"
#include "VHVFrame.h"
#include "VeritasCam.h"

using namespace std;

extern bool kDBLogging[NUMTEL];
extern bool kRecover;
extern ofstream kLogFile;
extern string kHVFile;
extern bool kActiveTelescope[NUMTEL];

/*****************************************************************************/
/***********************VHVControl*******************************************/
/** \class VHVControl 
 * \brief This class contains the code for the central widget of the GUI.
 *
 * The central widget (invoked by VHVFrame) is an interactive camera
 * display based on the QCanvas class.

 * \sa VHVControlView and VHVFrame
 */

/** Constructor. Instantiates a VHVData object, reads
 * channel mapping/HV settings from a file and generates icons for PMTs.
 * \arg \c siz - Size of each PMT Icon (pixels)
 * \arg \c parent - Parent widget
 * \arg \c name  - Object identifier
 */
//VHVControl::VHVControl(int siz, QWidget* parent=0, const char* name=0):QCanvas(parent, name)
//SPW fix
VHVControl::VHVControl( int telID, int siz, QWidget* parent, const char* name )
  :QCanvas(parent, name)
{
  setBackgroundColor( QColor( 0, 0, 0 ) );
  fTelescopeID = telID;
  fRecentTimeLimit = 60;
  fData = new VHVData();
  fVHVClient = new VNETClientThread( fTelescopeID, fData );
  //   cout <<"created fVHVClient"<< endl;
  
  fNumberPMTs = 0;
  fFrame = (VHVFrame *)parent;
  
  
  // Initialization.
  if ( kActiveTelescope[fTelescopeID] ) {
    initClient();
    for ( int i=0; i<fNumberPMTs; ++i ) {
      fPMTs[i] = new VPMTIcon( this, fData, fFrame, i, fTelescopeID );
    }
  }

  fSize = siz;
  fSelection = new VSelector(fSize, this);

}


/** Destructor
 */
VHVControl::~VHVControl()
{
  delete fSelection;
  delete fData;
  delete fVHVClient;
  delete fStars;

  for (int i=0; i<MAX_PMTS; i++) {
    delete fPMTs[i];
  }

}


void VHVControl::initClient()
{
  fVHVClient->getNumberOfPMTs();
  fNumberPMTs = fData->getNumberOfPMTs();
  fVHVClient->getNumberOfBoards();
  fVHVClient->getNumberOfCrates();
  fVHVClient->getHVCrate();
  fVHVClient->getHVSlot();
  fVHVClient->getHVChannel();
  fVHVClient->getStatus();
  fVHVClient->getBoardCrate();
  fVHVClient->getBoardSlot();
  fVHVClient->getBoardActive();
  //fVHVClient->takeStarControl();
}


/** Draw each PMT Icon (VPMTIcon) on the canvas. PMTs are layed out in 
 *  a hexagonal grid.
 */
void VHVControl::drawCamera() 
{
  
  double x;
  double y;
  double radius =  CAMERA_RADIUS;

  double maxr2 = pow(radius*fSize,2);

  fCentreX = 70+(int)(radius*fSize);
//   fCentreY = 20+fCentreX;
  fCentreY = 10+fCentreX;
  
  int dsiz = (int) ( fSize * 0.5 + 1 );
  int n = 0;
  
  /* import PMT positions from Steve Fegan's file 
     The sign in front of VC499Ycoord determines whether the pixel map matches
     vdcmon's or is upside down relative to vdcmon.  Prior to 11/18/2003 there
     has been a "+" sign and the vhv map has been upside down relative to
     vdcmon.  On 11/18/2003 I switch to a "-" sign.  TBH
  */


  for (int in=0; in<499; in++){
  //for (int in=0; in<48; in++){
    n=in;
    x = fCentreX+VC499Xcoord[n]*fSize*2.0/PIXEL_SIZE;
    y = fCentreY-VC499Ycoord[n]*fSize*2.0/PIXEL_SIZE;
    fPMTs[n]->draw((int)x, (int)y, dsiz);  
  }
  char text[40];
  sprintf( text, "hvacT%d: Number of PMTs: %d<br>", fTelescopeID+1, n+1 );
  fFrame->sendMessage( text );
  fNumberPMTs = n+1;

}


/** Redraw each PMT Icon (VPMTIcon) without recalculating positions.
 */
void VHVControl::redraw() 
{

  for (int i=0; i<fNumberPMTs; i++) {
    fPMTs[i]->draw();
  }
}


/** Set the High Voltage power to ON for all PMTs.
 */
void VHVControl::allOn() 
{
  if ( kActiveTelescope[fTelescopeID] ) {
    try {
      fVHVClient->allOn();
      if ( !fFrame->getStillInConstructor() ) {
	char tmpStr[100];
	sprintf( tmpStr, "hvacT%d: all on<br>", fTelescopeID+1 );
	fFrame->sendMessage( tmpStr );
      }
    }
    catch(VException & e) {
      cerr << "VHVControl::allOn():  " << e << endl;
    }
  }
}

/** Set the High Voltage power to OFF for all PMTs.
 */
void VHVControl::allOff() 
{
  //   cout << "allOff(): fTelID=" << fTelescopeID << endl;
  if ( kActiveTelescope[fTelescopeID] ) {
    try {
      cout << "\tT" << fTelescopeID+1 
	   << ":  active, sending power off command."
	   << endl;
      fVHVClient->allOff();
      if ( !fFrame->getStillInConstructor() ) {
	char tmpStr[100];
	sprintf( tmpStr, "hvacT%d: all off<br>", fTelescopeID+1 );
	fFrame->sendMessage( tmpStr );
      }
    }
    catch(VException & e) {
      cerr << "VHVControl::allOff():  " << e << endl;
    }
  }
}

/** Restore the High Voltage for all PMTs.  Added by TBH 040218
 */
void VHVControl::restoreAll() 
{
  if ( kActiveTelescope[fTelescopeID] ) {
    try {
      fVHVClient->allRestore();
      char tmpStr[100];
      sprintf( tmpStr, "hvacT%d: restored all<br>", fTelescopeID+1 );
      fFrame->sendMessage( tmpStr );
    }
    catch(VException & e) {
      cerr << "VHVControl::restoreAll():  " << e << endl;
    }
  }
}


/** Restore the High Voltage for recently suppressed PMTs.
 */
void VHVControl::restoreRecent() 
{
  if ( kActiveTelescope[fTelescopeID] ) {
    try {
      fVHVClient->restoreRecent();
      char tmpStr[100];
      sprintf( tmpStr, "hvacT%d: restored recent<br>", fTelescopeID+1 );
      fFrame->sendMessage( tmpStr );
    }
    catch(VException & e) {
      cerr << "VHVControl::restoreRecent():  " << e << endl;
    }
  }
}


/** Suppress the High Voltage for all PMTs.  Added by TBH 20050211
 */
void VHVControl::suppressAll() 
{
  if ( kActiveTelescope[fTelescopeID] ) {
    try {
      fVHVClient->allSuppress();
      char tmpStr[100];
      sprintf( tmpStr, "hvacT%d: suppressed all<br>", fTelescopeID+1 );
      fFrame->sendMessage( tmpStr );
    }
    catch(VException & e) {
      cerr << "VHVControl::suppressAll():  " << e << endl;
    }
  }
}


/** Print a summary of PMT HV/current status to a given file.
 * \arg \c filename - Name of the file to report to.
 */
void VHVControl::report(char* filename) 
{

  int vset,vtrue,power,pixel;
  float current;
  FILE *file = fopen(filename,"w");
 
  for ( int i=0; i<fNumberPMTs; i++ ) {
    vtrue = fData->getVTrue(i);
    vset  = fData->getVSet(i);
    power = (int) fData->getPower(i);
    current = fData->getCurrent(i);
    fprintf(file,"%3d  %1d  %5d  %5d %6.1f\n",
	    pixel,power,vset,vtrue,current);
  }

  fclose(file);
  
}


/** Draw the selection Icon at the position of the given PMT.
 * \arg \c p - Pointer to a VPMTIcon.
 */
void VHVControl::drawSelected(VPMTIcon *p) 
{
  QPainter paint;
  fSelection->move( p->x(), p->y() );
  fSelection->show();  
}


/** Return the number of assigned PMTs. 
 */
int VHVControl::getNumberOfPMTs() 
{
  if ( kActiveTelescope[fTelescopeID] ) {
    try {
      fVHVClient->getNumberOfPMTs();
      return fData->getNumberOfPMTs();
    }
    catch(VException & e) {
      cerr << "VHVControl::getNumberOfPMTs():  " << e << endl;
    }
  }
}


/** Return a pointer to the VPMTIcon of a given channel.
 * \arg \c chan - Software channel number.
 */
VPMTIcon* VHVControl::getPMT(int chan) 
{ 
  if ( kActiveTelescope[fTelescopeID] ) {
    return fPMTs[chan];
  } else {
    return 0;
  } 
}


/** Generate a new VStarField on this canvas 
 */
void VHVControl::loadStars()
{
  fCentreX = 70+(int)(CAMERA_RADIUS*fSize);
//   fCentreY = 20+fCentreX;
  fCentreY = 10+fCentreX;
  double pixel = PIXEL_SIZE/(double)(fSize*2.0+0.0001);
  fStars = new VStarField( this, fData, 166.11, 38.21, 3.5, pixel, 
			   fCentreX, fCentreY, 7.0 );
  fStars->readStars();
  fStars->calculateField();  
}


/** Draw all the VStar objects. 
 */
void VHVControl::drawStars() { fStars->draw(); }


/** Hide all the VStar objects. 
 */
void VHVControl::hideStars() { fStars->hide(); }


/** Draw all the VStarArc objects. 
 */
void VHVControl::showArcs() { fStars->timeArc( 28 ); }



/** Hide all the VStarArc objects. 
 */
void VHVControl::hideArcs() { fStars->hideArcs(); }


/** Change the limiting magnitude on the star display
 * \arg \c mag - Limiting B-band magnitude for display.
 */
void VHVControl::changeMag(double mag) { fStars->changeMagnitude(mag); }


/** Load High Voltage settings from a text file.
 * \arg \c file - Pointer to the file(handle).
 */
void VHVControl::loadHV( const char filename[] ) 
{
  if ( kActiveTelescope[fTelescopeID] ) {
    if ( !kRecover ) {
      allOff();
    }
    fData->reset();
    int pixel, index,chan,crate,board,v;
    ifstream infi( filename );
    for (int i=0; i<fNumberPMTs; i++) {
      infi >> pixel >> v;
      fData->getCrateAndSlot( pixel-1, crate, chan, board );
      fVHVClient->pixelSetVReq( i, v );
    } // end for
    if ( !kRecover ) {
      fVHVClient->allRestore();
      fVHVClient->setAllBoardPower( 0 );
    } // end if !kRecover
    infi.close();
    kRecover = false;
  }
}


/** Save High Voltage settings to a text file.  Since these are not default
 *  operating voltages, they are only saved locally on the hvac host.
 * \arg \c file - Pointer to the file(handle).
 */
void VHVControl::saveHV( const char filename[] ) 
{
  if ( kActiveTelescope[fTelescopeID] ) {
    ofstream outf( filename );

    int v, pixel;
  
    for (int i=0; i<fNumberPMTs; i++) {
      v = fData->getVReq(i);
      pixel = i+1;      // changed by sps 9/25/03 to provide pixel indexing
      outf << setw( 3 ) << pixel << " " << v << endl;  
    }
    outf.close();
  }
}


/** Return a pointer to the VStarField object associated with
 * this VHVControl object.
 */
VStarField* VHVControl::getStarField() { return(fStars); }


/** Returns a pointer to the fData member. **/
VHVData* VHVControl::getData()
{
  return fData;
}


/** Returns a pointer to the fFrame member. **/
VHVFrame* VHVControl::getFrame()
{
  return fFrame;
}


/** Returns a pointer to the fVHVClient member. **/
VNETClientThread* VHVControl::getVHVClient()
{
  return fVHVClient;
}


void VHVControl::updateData()
{
  bool alive = true;
  //  cout << "VHVControl::update()" << endl;
  if ( kActiveTelescope[fTelescopeID] ) {
    try {
      fVHVClient->alive();
    }
    catch(CORBA::SystemException &ex) {
      char text[200];
      sprintf( text, 
	       " Crisis! No CORBA Communications with T%d!  Restart vhv!<br>",
	       fTelescopeID+1 );
      fFrame->sendMessage( text );
      cerr << "Caught a CORBA::SystemException while using the naming service."
	   << ":  " << ex.NP_minorString() << ".  Must not be an active telescope."
	   << endl;
      alive = false;
      fVHVClient->begin();
    }
    if ( alive ) {
      for ( int i=0; i<NUM_CRATES; ++i ) {
	fVHVClient->getCrateStatus( i );
      }
      fVHVClient->allGetVReq();
      fVHVClient->allGetVSet();
      fVHVClient->allGetVTrue();
      fVHVClient->allGetPowerStatus();
      fVHVClient->allGetSuppressionStatus();
      fVHVClient->allGetDisabledStatus();
      fVHVClient->allGetAutoStatus();
      fVHVClient->getStatus();
      fVHVClient->getHWStatus();
      fVHVClient->getCurrent();
      fVHVClient->getBoardVReq();
      fVHVClient->getBoardVSet();
      fVHVClient->getBoardVTrue();
      fVHVClient->getBoardVMax();
      fVHVClient->getBoardPower();
      fVHVClient->getBoardRampUp();
      fVHVClient->getBoardRampDown();
      fVHVClient->getBoardIMax();
      fVHVClient->getBoardCurrent();
      fVHVClient->getBoardTemp();
      fVHVClient->getBoardActive();
      fVHVClient->getCurrentsStatus();
      //fVHVClient->getCameraTemp();
      //fVHVClient->getCameraHum();
      for ( int i=0; i<4; ++i )
	{
	  fVHVClient->getSensor( i );
	}
      fVHVClient->getLoggingState();
      fVHVClient->setStatusPeriod();
      fVHVClient->getMessages();
      //fVHVClient->setRA( fData->getRA() );
      //fVHVClient->setDec( fData->getDec() );
      fVHVClient->getFeedbackParams();
      fVHVClient->getFeedbackLevel();
      fVHVClient->isGlobalFeedbackEnabled();
      fVHVClient->isAllPixelFeedbackEnabled();
      if ( fData->getStateLimitBoardVoltages() )
	fVHVClient->enableLimitBoardVoltages();
      else fVHVClient->disableLimitBoardVoltages();
      //fVHVClient->getStateLimitBoardVoltages();
    }
  }
}


void VHVControl::takeStarControl()
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->takeStarControl();
}


void VHVControl::releaseStarControl()
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->releaseStarControl();
}


/** Reads voltage settings from the database.
 */
void VHVControl::readVoltagesFromDB()
{
  QString message;
  QString filename;
  if ( kHVFile.length() ) {
    loadHV( ((string)"/usr/local/veritas/SP05/hvac/HVFiles/" 
	     + kHVFile).c_str() );
    kHVFile.clear();
  } else {
    try {
      for ( unsigned int i=0; i<499; i++ ) {
	fVHVClient->pixelSetVReq( i, 
				  (int)VDBHV::getDefaultVoltage( fTelescopeID,
								 i+1 ) );
      }
    }
    catch(VException & e) {
      cerr << e << endl;
      message = " hvac: VHVControl::readVoltagesFromDB(): Reading voltages from DB failed.  Instead calling loadHV(). <br>";
      if ( !fFrame->getStillInConstructor() ) {
	fFrame->sendMessage( message );
      }
      cerr << "Instead calling loadHV()" << endl;
      //       filename = QFileDialog::getOpenFileName
      // 	( kDefaultHVFile[fTelescopeID], "HV files (*.hv)", fFrame );
      filename = QString( kDefaultHVFile[fTelescopeID] );
      if (!filename.isNull()) {
	cerr << "filename valid and file open.  let's read." << endl;
	loadHV( filename.latin1() );
	cerr << "file closed. " << endl;
	message = filename;
	message.prepend( " hvac: Loaded HV from file: " );
	message.append( "<br>" );
	if ( !fFrame->getStillInConstructor() ) {
	  fFrame->sendMessage( message );
	}
      } // end if !filename.isNull()
    }// end catch
  }
}


/** Writes voltage settings to the database.
 */
void VHVControl::writeVoltagesToDB()
{
  try {
    if ( kActiveTelescope[fTelescopeID] ) {
      fVHVClient->writeVoltagesToDB();
    }
  }
  catch(VException & e) {
    cerr << e << endl;
    QString message;
    message = "hvac: VHVControl::writeVoltagesToDB(): Writing voltages to DB failed. <br>";
    fFrame->sendMessage( message );
  }
}


/** Reads camera map from the database.
 */
void VHVControl::readCameraMapFromDB()
{
}


/** Writes camera map to the database.
 */
void VHVControl::writeCameraMapToDB()
{
}


/** Scales all channels by an operator-selected percentage. Skips
 *  channels that have zero voltage.
 **/
void VHVControl::scaleVoltages( float percent )
{
  cout << "in VHVControl::scaleVoltages()" <<endl;
  int newVoltage;
  if ( kActiveTelescope[fTelescopeID] ) {
    fVHVClient->allScaleHV( (float)percent );
  }
} // end scaleVoltages


/** Offsets all channels by an operator-selected amount. 
 **/
void VHVControl::offsetVoltages( float voltage )
{
  int newVoltage;
  if ( kActiveTelescope[fTelescopeID] ) {
    fVHVClient->allOffsetHV( voltage );
  }
} // end offsetVoltages


/** Sets all channels to an operator-selected voltage. 
 **/
void VHVControl::setAllVoltages( float voltage )
{
  if ( kActiveTelescope[fTelescopeID] ) {
    fVHVClient->allSetHV( voltage );
  }
} // end setAllVoltages


/** Called when the "Disable Pixel" option is selected from the 
 *  HVParams menu.
 **/
void VHVControl::markDisabled( int brokenpixel )
{
  if ( kActiveTelescope[fTelescopeID] ) {
    fVHVClient->markPixelDisabled( brokenpixel );
  }
}


/** Called when the "Enable Pixel" option is selected from the 
 *  HVParams menu.
 **/
void VHVControl::unmarkDisabled( int brokenpixel )
{
  if ( kActiveTelescope[fTelescopeID] ) {
    fVHVClient->unmarkPixelDisabled( brokenpixel );
  }
}


void VHVControl::markPixelNoAuto( int pixel )
{
  if ( kActiveTelescope[fTelescopeID] ) {
    fVHVClient->markPixelNoAuto( pixel );
  }
}

void VHVControl::unmarkPixelNoAuto( int pixel )
{
  if ( kActiveTelescope[fTelescopeID] ) {
    fVHVClient->unmarkPixelNoAuto( pixel );
  }
}


/** Sets Recent Time Limit for restoring recently suppressed pixels.
 */
void VHVControl::setRecentTimeLimit( int limit)
{
  fRecentTimeLimit = limit;
}


/** Gets Recent Time Limit for restoring recently suppressed pixels.
 */
int VHVControl::getRecentTimeLimit()
{ 
  return fRecentTimeLimit;
}


int VHVControl::getPMTCentreX( int chan )
{
  return fCentreX+VC499Xcoord[chan]*fSize*2.0/PIXEL_SIZE;
}


int VHVControl::getPMTCentreY( int chan )
{
  return fCentreY-VC499Ycoord[chan]*fSize*2.0/PIXEL_SIZE;
}


int VHVControl::getTelescopeID()
{
  return fTelescopeID;
}


void VHVControl::quitVHV()
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->quitVHV();
}


void VHVControl::connectToHVAC()
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->connectToHVAC();
}


void VHVControl::disconnectFromHVAC()
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->disconnectFromHVAC();
}


void VHVControl::loggingOn()
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->loggingOn();
}


void VHVControl::loggingOff()
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->loggingOff();
}


void VHVControl::pixelSetVReq( int chan, int voltage )
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->pixelSetVReq( chan,
								  voltage );
}


float VHVControl::pixelGetVReq( int chan )
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->pixelGetVReq( chan );
  return fData->getVReq( chan );
}


float VHVControl::pixelGetVSet( int chan )
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->pixelGetVSet( chan );
  return fData->getVSet( chan );
}


float VHVControl::pixelGetVTrue( int chan )
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->pixelGetVTrue( chan );
  return fData->getVTrue( chan );
}


void VHVControl::setPower( int chan, bool power )
{
  if ( kActiveTelescope[fTelescopeID] ) {
    if ( power ) {
      fVHVClient->pixelOn( chan );
    } else {
      fVHVClient->pixelOff( chan );
    }
  }
}


void VHVControl::setBoardPower( int board, bool power )
{
  if ( kActiveTelescope[fTelescopeID] ) {
    fVHVClient->setBoardPower( board, power );
  }
}


bool VHVControl::getPower( int chan )
{
  if ( kActiveTelescope[fTelescopeID] ) 
    fVHVClient->pixelGetPowerStatus( chan );
  return fData->getPower( chan );
}


bool VHVControl::getBoardPower( int board )
{
  if ( kActiveTelescope[fTelescopeID] ) 
    fVHVClient->getBoardPower();
  return fData->getBoardPower( board );
}


void VHVControl::suppressPixel( int chan )
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->pixelSuppress( chan );
}


void VHVControl::restorePixel( int chan )
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->pixelRestore( chan );
}


void VHVControl::globalFeedbackReset()
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->globalFeedbackReset();
}


void VHVControl::pixelFeedbackReset( int ch )
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->pixelFeedbackReset( ch );
}


void VHVControl::globalFeedbackOn()
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->globalFeedbackOn();
}


void VHVControl::pixelFeedbackOn( int ch )
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->pixelFeedbackOn( ch );
}


void VHVControl::globalFeedbackOff()
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->globalFeedbackOff();
}


void VHVControl::pixelFeedbackOff( int ch )
{
  if ( kActiveTelescope[fTelescopeID] ) fVHVClient->pixelFeedbackOff( ch );
}


void VHVControl::globalRefreshVoltageLevels()
{
  if ( kActiveTelescope[fTelescopeID] ) 
    fVHVClient->globalRefreshVoltageLevels();
}

//
//  Changed scalings on 2013/03/28, based on AC elog #2900 - SBM
//
void VHVControl::setUpHiLoInner()
{
  int newVoltage;
  for (int i=0; i<MAX_PMTS; i++) {
    if ( i<250 ) {
      //newVoltage = (int)(fData->getVReq(i)*1.1+0.5);
      newVoltage = (int)(fData->getVReq(i)*1.0+0.5);
    }
    else {
      //newVoltage = (int)(fData->getVReq(i)*0.83+0.5);
      newVoltage = (int)(fData->getVReq(i)*0.81+0.5);
    }
    if (newVoltage < 0)        newVoltage = 0;
    if (newVoltage > MAXVOLTS) newVoltage = MAXVOLTS;
    pixelSetVReq(i, newVoltage);
  }
}

//
// Changed scalings on 2013/03/28, based on AC elog #2900 -SBM
//
void VHVControl::setUpHiLoOuter()
{
  int newVoltage;
  for (int i=0; i<MAX_PMTS; i++) {
    if ( i>=250 ) {
      //newVoltage = (int)(fData->getVReq(i)*1.1+0.5);
      newVoltage = (int)(fData->getVReq(i)*1.0+0.5);
    }
    else {
      //newVoltage = (int)(fData->getVReq(i)*0.83+0.5);
      newVoltage = (int)(fData->getVReq(i)*0.81+0.5);
    }
    if (newVoltage < 0)        newVoltage = 0;
    if (newVoltage > MAXVOLTS) newVoltage = MAXVOLTS;
    pixelSetVReq(i, newVoltage);
  }
}





/****************************************************************************/
/***********************VHVControlView****************************************/

/** \class VHVControlView VHVControl.h
 * \brief Class to control the view of/interface to the VHVControl canvas
 * \sa VHVControl
 */

/** Constructor
 * \arg \c c - VHVControl object to be viewed.
 * \arg \c parent - Parent Qt Widget.
 * \arg \c name - Object indentifier
 * \arg \c flags - ?????
 */
VHVControlView::VHVControlView(
			       VHVControl* c, QWidget* parent, 
			       VHVFrame* frame,
			       const char* name, WFlags f) :
  QCanvasView( c,parent,name,f)
{

  setVScrollBarMode ( AlwaysOff );
  setHScrollBarMode ( AlwaysOff );
  fFrame = frame;
  fControl = c;
}

/** Destructor
 */
VHVControlView::~VHVControlView() { }

/** Action to be taken when a mouse button is pressed over a QCanvas object.
 * \arg \c e - A Qt mouse press event.
 */
void VHVControlView::contentsMousePressEvent(QMouseEvent* e)
{
  QCanvasItemList l;
  QPoint effective;
  int index = fFrame->getTab()->currentPageIndex();
  int xtrue = e->x();
  int ytrue = e->y();
  //   cout << "mouse at " << xtrue << ", " << ytrue;
  if ( index == NUMTEL ) {
    effective.setX( xtrue*2 );
    effective.setY( ytrue*2 );
    index = fControl->getTelescopeID();
    l=canvas()->collisions( effective );
    //     cout << "  :  " << effective.x() << ", " << effective.y();
  } else {
    l=canvas()->collisions( e->pos() );
  }
  //   cout << endl;
  for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
    if ( (*it)->rtti() == pmtRTTI ) {
      VPMTIcon* p = (VPMTIcon*)( *it );
      if ( p->getTelescopeID() == index ) {
	// 	cout << "icon for T" << p->getTelescopeID()+1 << " and index=" << index
	// 	     << " and channel=" << p->getChannel() << endl;
	emit clickedPMT( p, e, index );
      }
      return;
    }
  }
  //     int fullwidth = fControl->width();
  //     int fullheight = fControl->height();
  //     int halfwidth = fullwidth/2;
  //     int halfheight = fullheight/2;
  //     if ( xtrue < halfwidth ) {
  //       if ( ytrue < halfheight ) {
  // 	// T1
  // 	index = 0;
  // 	effective.setX( xtrue*2 );
  // 	effective.setY( ytrue*2 );
  // 	cout << "  :  T1:  " << effective.x() << ", " << effective.y()
  // 	     << endl;
  //       } else {
  // 	// T3
  // 	index = 2;
  // 	effective.setX( xtrue*2 );
  // 	effective.setY( (ytrue - halfheight)*2 );
  // 	cout << "  :  T3:  " << effective.x() << ", " << effective.y()
  // 	     << endl;
  //       }
  //     } else {
  //       if ( ytrue < halfheight ) {
  // 	// T2
  // 	index = 1;
  // 	effective.setX( (xtrue - halfwidth)*2 );
  // 	effective.setY( ytrue*2 );
  // 	cout << "  :  T2:  " << effective.x() << ", " << effective.y()
  // 	     << endl;
  //       } else {
  // 	// T4
  // 	index = 3;
  // 	effective.setX( (xtrue - halfwidth)*2 );
  // 	effective.setY( (ytrue - halfheight)*2 );
  // 	cout << "  :  T4:  " << effective.x() << ", " << effective.y()
  // 	     << endl;
  //       }
  //     }
  
}




/*****************************************************************************/
/***********************VPMTIcon**********************************************/
/** \class VPMTIcon VHVControl.h
 * \brief This class defines the graphical object representing a PMT.
 
 * In addition it provides a link to the information (stored in VHVData)
 * on this PMT.
 *
 * \sa VSelector
 */ 

/** Constructor
 * \arg \c canvas - QCanvas on which to draw this icon.
 * \arg \c data   - Pointer to VHVData object where PMT data lives.
 * \arg \c mf     - Pointer to overall application frame.
 * \arg \c chan   - Software channel number
 */
VPMTIcon::VPMTIcon( QCanvas* canvas, VHVData *data, VHVFrame *mf, int chan,
		    int tel )
  :  QCanvasPolygon(canvas)
{
  fChannel = chan;
  fTelescopeID = tel;
  fData    = data;
  fFrame   = mf;
  fCanvas  = canvas;
}


/** Destructor
 */
VPMTIcon::~VPMTIcon() {}


/** Generate and draw the icon 
 * \arg \c x - Pixel coordinate x (Right)
 * \arg \c y - Pixel coordinate y (Up)
 * \arg \c size - Icon radius (pixels)
 */
void VPMTIcon::draw( int x, int y, int size ) 
{
  QPointArray pa(6);
  pa[0] = QPoint(0, 2*size);
  pa[1] = QPoint(-size*173/100, size);
  pa[2] = QPoint(-size*173/100, -size);
  pa[3] = QPoint(0, -2*size);
  pa[4] = QPoint(size*173/100, -size);
  pa[5] = QPoint(size*173/100, size);
  setPoints(pa);
  move(x,y);
  setZ(1);
  draw();

  /* Label pixels with pixel number. 11-18-2003 TBH */
  static char number[10] = "1";
  sprintf(number, "%d", fChannel+1);
  QCanvasText* i = new QCanvasText(fCanvas);
  i->setColor(Qt::white);
  i->setText(number);
  QFont font( "Times", 13 );
  i->setFont( font );
  if ( fChannel < 9 ) {
    i->setX(x-6);
  } else if ( fChannel < 99 ) {
    i->setX(x-10);
  } else {
    i->setX(x-14);
  }
  i->setY(y-12);
  i->setZ(3);
  i->show();

  static QRect r(0,0,0,0); 
  r = pa.boundingRect(); // get the bounding rectangle for this sensor image

}

/** Draw the icon */
void VPMTIcon::draw() 
{
  if ( fData->getStatus(fChannel) == EInactive) {
    setBrush( getColor2(0.9,0.0,1.0,CS_GREY) );
    show();
    return;
  }
  
  switch (fFrame->getDisplayOpt()) {
  case POWER:
    if (fData->getPower(fChannel) == ON) {
      setBrush( green );
    } else {
      setBrush( red );
    }
    break;
  case IBOARD:
    int crate,slot,chan;
    fData->getCrateAndSlot(fChannel,crate,chan,slot);
    setBrush( getColor2(fData->getBoardCurrent(slot),0.0,30.0,CS_GREY) );
    break;
    // TBH 040323 : For IPMT, modify color scheme and range.
  case IPMT:

    //     if (fData->getCurrent(fChannel) < 40) {
    setBrush( getColor2(fData->getCurrent(fChannel),0.0,125.0,CS_PMT) );
    //     } else {
    //       setBrush( getColor2(fData->getCurrent(fChannel),40.0,125.0,CS_PMT_HOT) );
    //     }
    break;
    // TBH 040218 : For VTRUE and VSET, expand voltage range from (800,1300) to
    //              (600,1400).  Also set channels with voltage < 10V to black 
    //              and channels with voltage between 10V and 600V to darkRed.
    //              The darkRed channels will mainly correspond to those whose
    //              voltages have been set to zero but which cannot reach zero
    //              because the board primary is too high.
  case VTRUE:
    if (fData->getVTrue(fChannel) < 10) {
      setBrush( darkGray );
    }
    else if ((fData->getVTrue(fChannel) >= 10) && (fData->getVTrue(fChannel) < 
						   600)) {
      setBrush( darkRed );
    } else {
      setBrush( getColor2(fData->getVTrue(fChannel),600,1400,CS_STD) );
    }
    break;
  case VSET:
    if (fData->getVSet(fChannel) < 10) {
      setBrush( darkGray );
    }
    else if ((fData->getVSet(fChannel) >= 10) && (fData->getVSet(fChannel) < 
						  600)) {
      setBrush( darkRed );
    } else {
      setBrush( getColor2(fData->getVSet(fChannel),600,1400,CS_STD) );
    }
    break;
  case VREQ:
    if (fData->getVReq(fChannel) < 10) {
      setBrush( darkGray );
    }
    else if ((fData->getVReq(fChannel) >= 10) && (fData->getVReq(fChannel) < 
						  600)) {
      setBrush( darkRed );
    } else {
      setBrush( getColor2(fData->getVReq(fChannel),600,1400,CS_STD) );
    }
    break;
  case DELTAV1:
    setBrush( getColor2(fData->getVTrue(fChannel) -
			fData->getVReq(fChannel),-10,10,CS_GREY) );
    break;
  case DELTAV2:
    setBrush( getColor2(fData->getVSet(fChannel) -
			fData->getVReq(fChannel),-10,10,CS_GREY) );
    break;
  default:
    setBrush( black );
  }

  show();

}


/** Return pointer to the VHVData associated with this PMT 
 */
VHVData* VPMTIcon::getData() { return(fData); }


/** Return the software channel of this PMT 
 */
int VPMTIcon::getChannel() { return(fChannel); }


/** Return the software channel of this PMT 
 */
int VPMTIcon::getTelescopeID() { return(fTelescopeID); }





/*****************************************************************************/
/*****************************************************************************/

/** \class VSelector VHVControl.h
 * \brief This class defines the icon of the PMT selection cursor
 * \sa VPMTIcon
 */

/** Constructor
 * \arg \c size   - Icon radius (pixels)
 * \arg \c canvas - QCanvas on which to draw this object.
 */
VSelector::VSelector(int size, QCanvas *canvas ) : QCanvasPolygonalItem(canvas)
{
  fControl = (VHVControl*)canvas;
  const int s=(int)(size*0.5+1);
  const int w=s*173/100;
  const int x[] = {0  ,-w, -w,   0,  w, w, 0};
  const int y[] = {2*s, s, -s,-2*s, -s, s, 2*s};

  for (int i=0;i<6;i++) {
    fStart[i] = QPoint(x[i],y[i]);
    fEnd[i] = QPoint(x[i+1],y[i+1]);
  }
  setZ( 3 );
}  

/** Destructor
 */
VSelector::~VSelector() {}

/** Draw the selector icon.
 * \arg \c p - QPainter pen object.
 */
void VSelector::drawShape( QPainter &p )
{
  int o = fControl->getFrame()->getDisplayOpt();
  switch ( o ) {
  case IBOARD:
  case IPMT:
    p.setPen( QPen( red, 2, SolidLine ) );
    break;
  case POWER:
  case VTRUE:
  case VSET:
  case VREQ:
  case DELTAV1:
  case DELTAV2:
  default:
    setBrush( black );
  }
  //   p.setPen( QPen( black, 2, SolidLine ) );

  QPoint pos = QPoint((int)x(),(int)y());
  for (int i=0;i<6;i++) {
    QPoint s = fStart[i] + pos;
    QPoint e = fEnd[i] + pos;
    p.drawLine( s, e );
  }

}

/** Return an array of points defining the icon boundary
 */
QPointArray VSelector::areaPoints() const
{

  QPointArray pa(6);
  for (int i=0;i<6;i++) {
    pa[i] = QPoint((int)(fStart[i].x()*1.2 + x()),
                   (int)(fStart[i].y()*1.2 + y()));
  }

  return(pa);

}

/*****************************************************************************/

/** Global function getColor defines a colour associated with a 
 *   value (val) between min and max 
 * \arg \c val - Value for which a color is to be assigned
 * \arg \c min - Minimum acceptable value.
 * \arg \c max - Maximum acceptable value.
 * \arg \c scheme - Colour scheme to use (EnumColorScheme) 
 */
QColor getColor(double val, double min, double max, int scheme)
{

  double f = (val-min)/(max-min);      
  if (f < 0.0) f = 0.0;
  if (f > 1.0) f = 1.0;

  int x,y,z;
  QColor::Spec cs;

  switch (scheme) {
  case(CS_GREY):    
    x = (int)(240.0*f)+10;      
    y = x;
    z = x;
    cs = QColor::Rgb;
    break;
  case(CS_STD):
    x = (int)(240.0*f);
    y = 120;
    z = (int)(10+230.0*f);
    cs = QColor::Hsv;
    break;
  case(CS_PMT_COOL):
    f = log10(9*f+1);
    if ( f<0.5 ) {
      x = (int)10;
      y = (int)10;
      z = (int)(490*f+10);
    } else {
      x = (int)10;
      y = (int)(490*f-245);
      z = (int)255;
    }
    cs = QColor::Rgb;
    break;
  case(CS_PMT_HOT):
    x = (int)(155*(1-log10(9*f+1))+100);
    y = (int)220*(1-log10(9*f+1));
    z = (int)220*(1-log10(9*f+1));
    cs = QColor::Rgb;
    break;
  case(CS_STAR):
  default:
    x = (int)(300.0*f);
    y = 255;
    z = 255;
    cs = QColor::Hsv;    
  }

  QColor c(x, y, z, cs);
  //   cout << x << " " << y << " " << z << endl;

  //  printf("%f - %d %d %d %d - %d\n",f,x,y,z,cs,scheme);

  return(c);
}

/** Global function getColor defines a colour associated with a 
 *   value (val) between min and max 
 * \arg \c val - Value for which a color is to be assigned
 * \arg \c min - Minimum acceptable value.
 * \arg \c max - Maximum acceptable value.
 * \arg \c scheme - Colour scheme to use (EnumColorScheme) 
 */
QColor getColor(int val, int min, int max, int scheme)
{
  
  return(getColor((double)val,(double)min,(double)max,scheme));

}

/*****************************************************************************/

/** Global function getColor defines a colour associated with a 
 *   value (val) between min and max 
 * \arg \c val - Value for which a color is to be assigned
 * \arg \c min - Minimum acceptable value.
 * \arg \c max - Maximum acceptable value.
 * \arg \c scheme - Colour scheme to use (EnumColorScheme) 
 */
QColor getColor2(double val, double min, double max, int scheme)
{

  double f = (val-min)/(max-min);      
  if (f < 0.0) f = 0.0;
  if (f > 1.0) f = 1.0;

  int x,y,z;
  QColor::Spec cs;
  QColor cc;

  switch (scheme) {
  case(CS_GREY):    
    x = (int)(240.0*f)+10;      
    y = x;
    z = x;
    cs = QColor::Rgb;
    break;
  case(CS_STD):
    //cs = QColor::Rgb;
    if ( f < 0.333 ) {
      x=10;
      y=(int)(f*3*245)+10;
      z=10;
      //cout << "f=" << f 
      //	   << "  x=" << x << "  y=" << y << "  z=" << z << endl;
    } 
    else if (f < 0.666) {
      x=10;
      y=255;
      z=(int)((f-0.333)*3*245)+10;
      //cout << "f=" << f 
      //	   << "  x=" << x << "  y=" << y << "  z=" << z << endl;
    } 
    else {
      x=(int)((f-0.666)*3*245)+10;
      y=255;
      z=255;
      //cout << "f=" << f 
      //	   << "  x=" << x << "  y=" << y << "  z=" << z << endl;
    }
    break;
  case(CS_PMT):
    if ( f < 50./125. )
      {
	f *= 125./50.;
	f = log10(9*f+1);
	if ( f < 0.333 )
	  {
	    x=60;
	    z=(int)(f*3*195)+60;
	    y=60;
	    //cout << "f=" << f 
	    //	   << "  x=" << x << "  y=" << y << "  z=" << z << endl;
	  } 
	else if (f < 0.666)
	  {
	    x=40;
	    z=255;
	    y=(int)((f-0.333)*3*215)+40;
	    //cout << "f=" << f 
	    //	   << "  x=" << x << "  y=" << y << "  z=" << z << endl;
	  } 
	else
	  {
	    x=(int)((f-0.666)*3*215)+40;
	    y=255;
	    z=255;
	    //cout << "f=" << f 
	    //	   << "  x=" << x << "  y=" << y << "  z=" << z << endl;
	  }
      }
    else
      {
	f = (f-50./125.)*(125./75.);
	x=255;
	z=(int)(255-f*215);
	y=(int)(255-f*215);
      }
    break;
  case(CS_PMT_HOT):
    x = (int)(155*(1-log10(9*f+1))+100);
    y = (int)220*(1-log10(9*f+1));
    z = (int)220*(1-log10(9*f+1));
    cs = QColor::Rgb;
    break;
  case(CS_STAR):
  default:
    x = (int)(300.0*f);
    y = 255;
    z = 255;
    cs = QColor::Hsv;    
  }

  QColor c(x, y, z, cs);
  //   cout << x << " " << y << " " << z << endl;

  //  printf("%f - %d %d %d %d - %d\n",f,x,y,z,cs,scheme);
  cc.setRgb (x, y, z);

  return(cc);
}

/** Global function getColor defines a colour associated with a 
 *   value (val) between min and max 
 * \arg \c val - Value for which a color is to be assigned
 * \arg \c min - Minimum acceptable value.
 * \arg \c max - Maximum acceptable value.
 * \arg \c scheme - Colour scheme to use (EnumColorScheme) 
 */
QColor getColor2(int val, int min, int max, int scheme)
{
  
  return(getColor2((double)val,(double)min,(double)max,scheme));

}

/*---------------------------------------------------------------*/

/*****************************************************************************/
/*****************************************************************************/

/** \class VCamInfo VHVControl.h
 * \brief This class displays some useful information about the camera.
 * \sa VCamInfo
 */

/** Constructor
 * \arg \c size   - Icon radius (pixels)
 * \arg \c canvas - QCanvas on which to draw this object.
 */
VCamInfo::VCamInfo(VHVFrame *frame, QCanvas *canvas, VHVData *data, int id )
{
  fTelescopeID = id;
  fFrame = frame;
  fCanvas = canvas;
  fData = data;
  fNumberOfPMTs = fData->getNumberOfPMTs();
  fMaxPMTCurrent = new QCanvasText( fCanvas );
  fAvePMTCurrent = new QCanvasText( fCanvas );
  QFont currfont( "Helvetica", 14, QFont::Bold );
  QFont bigfont( "Helvetica", 32, QFont::Bold );
  fMaxPMTCurrent->setFont( currfont );
  fAvePMTCurrent->setFont( currfont );
  fMaxPMTCurrent->setColor( Qt::white );
  fAvePMTCurrent->setColor( Qt::white );
  fMaxPMTCurrent->move( 580, 5 );
  fMaxPMTCurrent->setZ( 2 );
  fAvePMTCurrent->move( 580, 25 );
  fAvePMTCurrent->setZ( 2 );
  fMaxPMTCurrent->show();
  fAvePMTCurrent->show();
  for ( int i=0; i<NUMLINES; i++ ) {
    fLegend[i] = new QCanvasLine( fCanvas );
  }
  for ( int i=0; i<NUMLABELS; i++ ) {
    fText[i] = new QCanvasText( fCanvas );
    fText[i]->setFont( currfont );
    fText[i]->setColor( Qt::white );
  }
  for (int i=0;i<NUMLINES;i++) {
    fLegend[i]->setPoints(5,12+i,20,12+i);
    fLegend[i]->setZ(2);
    fLegend[i]->show();
  }
  for (int i=0;i<NUMLABELS;i++) {
    fText[i]->move( 30, -5+i*200/(NUMLABELS-1) );
    fText[i]->setZ(2);
    fText[i]->show();
  }
  //   fTemp = new QCanvasText( fCanvas );
  //   fHum = new QCanvasText( fCanvas );
  //   fTemp->setFont( currfont );
  //   fHum->setFont( currfont );
  //   fTemp->move( 545, 45 );
  //   fHum->move( 545, 65 );
  //   fTemp->setZ( 2 );
  //   fHum->setZ( 2 );
  for ( int i=0; i<NUMSENSORS-2; ++i ) {
    fSensor[i] = new QCanvasText( fCanvas );
    fSensor[i]->setFont( currfont );
    fSensor[i]->setColor( Qt::white );
    fSensor[i]->move( 650, 745+20*i );
    fSensor[i]->setZ( 2 );
    fSensor[i]->show();
  }
  //  fSensor[NUMSENSORS-2]->move( 5, 785 );
  //fSensor[NUMSENSORS-1]->move( 5, 805 );


  fSuppressedIcon = new QCanvasPolygon( fCanvas );
  fDisabledIcon = new QCanvasPolygon( fCanvas );
  fNoAutoIcon = new QCanvasPolygon( fCanvas );
  int size = 5;
  QPointArray pa(6);
  pa[0] = QPoint(0, 2*size);
  pa[1] = QPoint(-size*173/100, size);
  pa[2] = QPoint(-size*173/100, -size);
  pa[3] = QPoint(0, -2*size);
  pa[4] = QPoint(size*173/100, -size);
  pa[5] = QPoint(size*173/100, size);
  fSuppressedIcon->setPoints(pa);
  fSuppressedIcon->move( 100, 35 );
  fSuppressedIcon->setZ( 1 );
  fSuppressedIcon->setBrush( Qt::darkYellow );
  fDisabledIcon->setPoints(pa);
  fDisabledIcon->move( 100, 60 );
  fDisabledIcon->setZ( 1 );
  fDisabledIcon->setBrush( Qt::magenta );
  fNoAutoIcon->setPoints(pa);
  fNoAutoIcon->move( 100, 10 );
  fNoAutoIcon->setZ( 1 );
  fNoAutoIcon->setBrush( Qt::darkGreen );
  fSuppressedLabel = new QCanvasText( "Suppressed", fCanvas );
  fSuppressedLabel->setFont( currfont );
  fSuppressedLabel->setColor( Qt::white );
  fSuppressedLabel->move( 115, 20 );
  fSuppressedLabel->setZ( 1 );
  fDisabledLabel = new QCanvasText( "Disabled", fCanvas );
  fDisabledLabel->setFont( currfont );
  fDisabledLabel->setColor( Qt::white );
  fDisabledLabel->move( 115, 45 );
  fDisabledLabel->setZ( 1 );
  fNoAutoLabel = new QCanvasText( "AutoSuppress Disabled", fCanvas );
  fNoAutoLabel->setFont( currfont );
  fNoAutoLabel->setColor( Qt::white );
  fNoAutoLabel->move( 115, -5 );
  fNoAutoLabel->setZ( 1 );
  fSuppressedIcon->show();
  fDisabledIcon->show();
  fNoAutoIcon->show();
  fSuppressedLabel->show();
  fDisabledLabel->show();
  fNoAutoLabel->show();
  char tel[10];
  sprintf( tel, "T%d", fTelescopeID+1 );
  fTelescopeLabel = new QCanvasText( tel, fCanvas );
  fTelescopeLabel->setFont( bigfont );
  fTelescopeLabel->move( 380, -15 );
  fTelescopeLabel->setColor( Qt::white );
  fTelescopeLabel->setZ( 1 );
  fTelescopeLabel->show();

  // Alert indicator
  QFont giantfont( "Helvetica", 17, QFont::Bold );
  fAlert1 = new QCanvasText( "PMT Currents", fCanvas );
  fAlert2 = new QCanvasText( "  Unchanged ", fCanvas );
  fAlert3 = new QCanvasText( "vhv & vdcmon OK? ", fCanvas );
  //giantfont.setStretch( 150 );
  fAlert1->setFont( giantfont );
  fAlert1->setColor( Qt::red );
  fAlert1->move( 20, 720 );
  fAlert1->setZ( 4 );
  fAlert1->hide();
  fAlert2->setFont( giantfont );
  fAlert2->setColor( Qt::red );
  fAlert2->move( 20, 750 );
  fAlert2->setZ( 4 );
  fAlert2->hide();
  fAlert3->setFont( giantfont );
  fAlert3->setColor( Qt::red );
  fAlert3->move( 20, 780 );
  fAlert3->setZ( 4 );
  fAlert3->hide();
  fAlertStatus = false;
}


VCamInfo::~VCamInfo()
{
  delete fMaxPMTCurrent;
  delete fAvePMTCurrent;
  for ( int i=0; i<NUMLINES; i++ ) {
    delete fLegend[i];
  }
  for ( int i=0; i<NUMLABELS; i++ ) {
    delete fText[i];
  }
  for ( int i=0; i<NUMSENSORS; ++i ) {
    delete fSensor[i];
  }
  delete fSuppressedIcon;
  delete fDisabledIcon;
  delete fNoAutoIcon;
  delete fSuppressedLabel;
  delete fDisabledLabel;
  delete fNoAutoLabel;
  delete fAlert1;
  delete fAlert2;
  delete fAlert3;
  //   delete fTemp;
  //   delete fHum;

}

void VCamInfo::redraw()
{
  //   cout << "In redraw" << endl;
  char maxPMTCurrentString[200];
  char avePMTCurrentString[200];
  //   char temperatureString[100];
  //   char humidityString[100];
  char sensorString[50];
  float max = 0;
  float ave = 0;
  int counter = 0;
  int maxpmt = -9999;

  for (int i=0; i<fNumberOfPMTs; i++) {
    if ( fData->getStatus(i) != EInactive && !fData->getAutoStatus(i) ) {
      ave += fData->getCurrent(i);
      if ( fData->getCurrent(i) > max ) {
	max = fData->getCurrent(i);
	maxpmt = i+1;
      }
      counter++;
    }
  }
  ave = ave/counter;
  sprintf( maxPMTCurrentString, "Max Cur: %4.1f uA pix %d", max,  maxpmt );
  sprintf( avePMTCurrentString, "Ave Cur: %5.2f uA", ave );
  //   cout << "VHVControl: temp=" << fData->getCameraTemp() << " hum=" 
  //        << fData->getCameraHum() << endl;
  //   sprintf( temperatureString, "Cam Temp: %d C", fData->getCameraTemp() );
  //   sprintf( humidityString, "Cam Hum:  %d", fData->getCameraHum() );
  //   cout << maxPMTCurrentString << endl;
  //   cout << avePMTCurrentString << endl;
  fMaxPMTCurrent->setText( maxPMTCurrentString );
  //   fMaxPMTCurrent->show();
  fAvePMTCurrent->setText( avePMTCurrentString );
  sprintf( sensorString, "Cam Temp1: %d C", 
	   fData->getCameraTemp() );
  fSensor[SENSOR_CAMTEMP]->setText( sensorString );
  sprintf( sensorString, "Cam Hum1:  %d \%", 
	   fData->getCameraHum() );
  fSensor[SENSOR_CAMHUM]->setText( sensorString );
  sprintf( sensorString, "Cam Temp2: %d C", 
	   fData->getSensor( SENSOR_AMBTEMP ) );
  fSensor[SENSOR_AMBTEMP]->setText( sensorString );
  sprintf( sensorString, "Cam Hum2: %d \%", 
	   fData->getSensor( SENSOR_AMBHUM ) );
  fSensor[SENSOR_AMBHUM]->setText( sensorString );
//   sprintf( sensorString, "Light1:   %d", 
// 	   fData->getSensor( SENSOR_LIGHT1 ) );
//   fSensor[SENSOR_LIGHT1]->setText( sensorString );
//   sprintf( sensorString, "Light2:   %d", 
// 	   fData->getSensor( SENSOR_LIGHT2 ) );
//   fSensor[SENSOR_LIGHT2]->setText( sensorString );
  //   fAvePMTCurrent->show();
  //   fTemp->setText( temperatureString );
  //   fTemp->show();
  //   fHum->setText( humidityString );
  //   fHum->show();

  drawLegend();
  kLogFile << "aveCur=" << ave << " ";
  if ( fAlertStatus )
    {
      //cout << "uh oh:  ";
      if ( time(0) % 2 ) 
	{
	  fAlert1->show();
	  fAlert2->show();
	  fAlert3->show();
	  //cout << "show alert";
	}
      else
	{
	  fAlert1->hide();
	  fAlert2->hide();
	  fAlert3->hide();
	  //cout << "hide alert";
	}
    }
  else 
    {
      fAlert1->hide();
      fAlert2->hide();
      fAlert3->hide();
      //cout << "phew";
    }
  //cout << endl;

}

/** Draw a legend defining the color map being used for this camera display.
 *  TBH 20040817
 */
void VCamInfo::drawLegend()
{
  //cout << "VCamInfo: in drawLegend()" << endl;
  int displayOption = fFrame->getDisplayOpt();
  char legendstring[100];

  //cout << "VCamInfo: entering switch" << endl;
  switch ( displayOption ) {
  case ( IBOARD ):
    //cout << "VCamInfo: case iboard" << endl;
    for (int i=0;i<NUMLINES;i++) {
      fLegend[i]->setPen( QPen( getColor2( (int)(30./NUMLINES*i), 
					   0, 30, CS_GREY ), 1));
    }
    for (int i=0;i<NUMLABELS;i++) {
      sprintf( legendstring, "%d mA", (int)(30.*i/(NUMLABELS-1)) );
      fText[i]->setText( legendstring );
    }
    //     fSuppressedIcon->hide();
    //     fDisabledIcon->hide();
    //     fSuppressedLabel->hide();
    //     fDisabledLabel->hide();
    break;
  case ( IPMT ):
    //cout << "VCamInfo: case ipmt" << endl;
    for (int i=0;i<NUMLINES;i++) {
      fLegend[i]->setPen( QPen( getColor2( (int)(125./NUMLINES*i), 
					   0, 125, CS_PMT ), 1));
    }
    for (int i=0;i<NUMLABELS;i++) {
      sprintf( legendstring, "%d uA", 125*i/(NUMLABELS-1) );
      fText[i]->setText( legendstring );
    }
    break;
  case ( VREQ ):
    //cout << "VCamInfo: case vreq" << endl;
    for (int i=0;i<NUMLINES;i++) {
      if ( i < 10./1400.*NUMLINES ) {
	fLegend[i]->setPen( QPen( Qt::black, 1) );
      } else if ( i < 600./1400.*NUMLINES ) {
	fLegend[i]->setPen( QPen( Qt::darkRed, 1 ) );
      } else {
	fLegend[i]->setPen( QPen( getColor2( 1400/NUMLINES*i, 
					     600, 1400, CS_STD), 1 ) );
      }
    }
    for (int i=0;i<NUMLABELS;i++) {
      sprintf( legendstring, "%d V", 1400*i/(NUMLABELS-1) );
      fText[i]->setText( legendstring );
    }
    break;
  case ( VSET ):
    //cout << "VCamInfo: case vset" << endl;
    for (int i=0;i<NUMLINES;i++) {
      if ( i < 10./1400.*NUMLINES ) {
	fLegend[i]->setPen( QPen( Qt::black, 1) );
	// 	cout << "i=" << i << " and color is black" << endl;;
      } else if ( i < 600./1400.*NUMLINES ) {
	fLegend[i]->setPen( QPen( Qt::darkRed, 1 ) );
	// 	cout << "i=" << i << " and color is red" << endl;;
      } else {
	fLegend[i]->setPen( QPen( getColor2( 1400/NUMLINES*i, 
					     600, 1400, CS_STD), 1 ) );
	// 	cout << "i=" << i << " and color is " ;
      }
    }
    for (int i=0;i<NUMLABELS;i++) {
      sprintf( legendstring, "%d V", 1400*i/(NUMLABELS-1) );
      fText[i]->setText( legendstring );
    }
    break;
  case ( VTRUE ):
    //cout << "VCamInfo: case vtrue" << endl;
    for (int i=0;i<NUMLINES;i++) {
      if ( i < 10./1400.*NUMLINES ) {
	fLegend[i]->setPen( QPen( Qt::black, 1) );
      } else if ( i < 600./1400.*NUMLINES ) {
	fLegend[i]->setPen( QPen( Qt::darkRed, 1 ) );
      } else {
	fLegend[i]->setPen( QPen( getColor2( 1400/NUMLINES*i, 
					     600, 1400, CS_STD), 1 ) );
      }
    }
    for (int i=0;i<NUMLABELS;i++) {
      sprintf( legendstring, "%d V", 1400*i/(NUMLABELS-1) );
      fText[i]->setText( legendstring );
    }
    break;
  case ( DELTAV1 ):
    //cout << "VCamInfo: case deltav1" << endl;
    for (int i=0;i<NUMLINES;i++) {
      fLegend[i]->setPen( QPen( getColor2( (int)(20./NUMLINES*i-10), 
					   -10, 10, CS_GREY), 1 ) );
    }
    for (int i=0;i<NUMLABELS;i++) {
      sprintf( legendstring, "%d V", 20*i/(NUMLABELS-1)-10 );
      fText[i]->setText( legendstring );
    }
    break;
  case ( DELTAV2 ):
    //cout << "VCamInfo: case deltav2" << endl;
    for (int i=0;i<NUMLINES;i++) {
      fLegend[i]->setPen( QPen( getColor2( (int)(20./NUMLINES*i-10), 
					   -10, 10, CS_GREY), 1 ) );
    }
    for (int i=0;i<NUMLABELS;i++) {
      sprintf( legendstring, "%d V", 20*i/(NUMLABELS-1)-10 );
      fText[i]->setText( legendstring );
    }
    break;
  case ( POWER ):
    //cout << "VCamInfo: case power" << endl;
    for (int i=0;i<NUMLINES;i++) {
      if ( i < NUMLINES/2 ) {
	fLegend[i]->setPen( QPen( Qt::red, 1) );
      } else {
	fLegend[i]->setPen( QPen( Qt::green ) );
      }
    }
    for (int i=0;i<NUMLABELS;i++) {
      if ( i < NUMLABELS/2 ) {
	sprintf( legendstring, "OFF" );
      } else {
	sprintf( legendstring, "ON" );
      }
      fText[i]->setText( legendstring );
    }
    break;
  }
  
}


void VCamInfo::drawAlert()
{
  fAlertStatus = true;
}


void VCamInfo::hideAlert()
{
  fAlertStatus = false;
}


/*---------------------------------------------------------------*/

/*****************************************************************************/
/*****************************************************************************/

/** \class VPMTIndicator VHVControl.h
 * \brief This class displays a color-coded halo about a pixel to provide
 * additional information about its state.
 * \sa VPMTIndicator
 */

/** Constructor
 * \arg \c size   - Icon radius (pixels)
 * \arg \c canvas - QCanvas on which to draw this object.
 */

// If this pixel is disabled or suppressed, indicate it with a magenta (disabled)
// or gold (suppressed) ring about it.

VPMTIndicator::VPMTIndicator( VHVFrame *frame, QCanvas *canvas, 
			      VHVData *data, int size, int chan )
{
  fFrame = frame;
  fCanvas = canvas;
  fData = data;
  fChannel = chan;
  //   fCounter = 0;

  fl1 = new QCanvasLine( fCanvas );
  fl2 = new QCanvasLine( fCanvas );
  fl3 = new QCanvasLine( fCanvas );
  fl4 = new QCanvasLine( fCanvas );
  fl5 = new QCanvasLine( fCanvas );
  fl6 = new QCanvasLine( fCanvas );
  int rsize = size/2;
  fl1->setPoints( 0, 2*rsize, -rsize*173/100, rsize );
  fl2->setPoints( -rsize*173/100, rsize, -rsize*173/100, -rsize );
  fl3->setPoints( -rsize*173/100, -rsize, 0, -2*rsize );
  fl4->setPoints( 0, -2*rsize, rsize*173/100, -rsize );
  fl5->setPoints( rsize*173/100, -rsize, rsize*173/100, rsize );
  fl6->setPoints( rsize*173/100, rsize, 0, 2*rsize );
  fl1->move( fX, fY );
  fl2->move( fX, fY );
  fl3->move( fX, fY );
  fl4->move( fX, fY );
  fl5->move( fX, fY );
  fl6->move( fX, fY );
  fl1->setZ( 2 );
  fl2->setZ( 2 );
  fl3->setZ( 2 );
  fl4->setZ( 2 );
  fl5->setZ( 2 );
  fl6->setZ( 2 );
}

VPMTIndicator::~VPMTIndicator()
{
  delete fl1;
  delete fl2;
  delete fl3;
  delete fl4;
  delete fl5;
  delete fl6;

}

void VPMTIndicator::redraw() 
{
  //   if ( fCounter == 20 ) {
  //     cout << "fChannel = " << fChannel << "  position = (" << fl1->x() << ", " 
  // 	 << fl1->y() << ")" << endl;
  //   }
  //   fCounter++;

  QBrush disabledBrush( Qt::magenta );
  QBrush noautoBrush( Qt::darkGreen );
  QBrush suppressedBrush( Qt::darkYellow );
  QPen noautoPen( Qt::darkGreen, 3, Qt::SolidLine, Qt::RoundCap, 
		  Qt::RoundJoin );
  QPen disabledPen( Qt::magenta, 4, Qt::SolidLine, Qt::RoundCap, 
		    Qt::RoundJoin );
  QPen suppressedPen( Qt::darkYellow, 4, Qt::SolidLine, Qt::RoundCap, 
		      Qt::RoundJoin );
  if ( !fData->getSuppressed( fChannel ) ) {
    fl1->setBrush( suppressedBrush );
    fl2->setBrush( suppressedBrush );
    fl3->setBrush( suppressedBrush );
    fl4->setBrush( suppressedBrush );
    fl5->setBrush( suppressedBrush );
    fl6->setBrush( suppressedBrush );
    fl1->setPen( suppressedPen );
    fl2->setPen( suppressedPen );
    fl3->setPen( suppressedPen );
    fl4->setPen( suppressedPen );
    fl5->setPen( suppressedPen );
    fl6->setPen( suppressedPen );
  }
  if ( fData->getAutoStatus( fChannel ) ) {
    fl1->setBrush( noautoBrush );
    fl2->setBrush( noautoBrush );
    fl3->setBrush( noautoBrush );
    fl4->setBrush( noautoBrush );
    fl5->setBrush( noautoBrush );
    fl6->setBrush( noautoBrush );
    fl1->setPen( noautoPen );
    fl2->setPen( noautoPen );
    fl3->setPen( noautoPen );
    fl4->setPen( noautoPen );
    fl5->setPen( noautoPen );
    fl6->setPen( noautoPen );
  }
  if ( fData->getDisabled( fChannel ) ) {
    fl1->setBrush( disabledBrush );
    fl2->setBrush( disabledBrush );
    fl3->setBrush( disabledBrush );
    fl4->setBrush( disabledBrush );
    fl5->setBrush( disabledBrush );
    fl6->setBrush( disabledBrush );
    fl1->setPen( disabledPen );
    fl2->setPen( disabledPen );
    fl3->setPen( disabledPen );
    fl4->setPen( disabledPen );
    fl5->setPen( disabledPen );
    fl6->setPen( disabledPen );
  }
  if ( !fData->getSuppressed( fChannel ) || 
       fData->getAutoStatus( fChannel ) || 
       fData->getDisabled( fChannel ) ) {
    fl1->show();
    fl2->show();
    fl3->show();
    fl4->show();
    fl5->show();
    fl6->show();
    //     cout << "fChannel = " << fChannel << ":  suppressed or disabled." << endl;
  } else {
    fl1->hide();
    fl2->hide();
    fl3->hide();
    fl4->hide();
    fl5->hide();
    fl6->hide();
    //     cout << "fChannel = " << fChannel << ":  happy." << endl;
  }
}

void VPMTIndicator::setPosition( int x, int y )
{
  fX = x;
  fY = y;
  fl1->move( fX, fY );
  fl2->move( fX, fY );
  fl3->move( fX, fY );
  fl4->move( fX, fY );
  fl5->move( fX, fY );
  fl6->move( fX, fY );
}
