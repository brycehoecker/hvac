///////////////////////////////////////////////////////////////////////////////
//
// Filename : VHVAutoVoltage.cpp
// Created  : Sun Oct 24 2004 by Brian Humensky
///////////////////////////////////////////////////////////////////////////////
/*! \file VHVAutoVoltage.cpp
  \brief Defines classes for automatic response of high voltage to star positions and anode currents.
*/

#include <cstdlib>
#include <cstdio> 
#include <cmath>
#include <iostream>

#include "VHVAutoVoltage.h"

using namespace std;


/** Constructor:
 * - Instantiates a VHVAutoVoltage object
 *
 */
VHVAutoVoltage::VHVAutoVoltage()
{
  // Allocate array of QTimers for transients
  for (int i=0; i<MAX_STAR_TIMERS; i++) {
    fTimers[i] = new QTimer( this );
  }

  // Initialize member variables
  fTransientOffTime = 60. * 1000; // milliseconds.  default time 1 min.
  fApertureMargin = 0.05; // degrees.  Added to calculated angle to determine
                         //           distance star must be from pixel center
                         //           to turn pixel back on.
  fStarSearchRadius = 0.20; // degrees.  Search for stars within this radius.
  fMagnitudeLimit = 7; // magnitude.  Max magnitude of stars to worry about.
  fTurnOffFlag = false; // by default expect suppressPixel() call from QMon.
                        // True means to be constantly searching for
                        // problematic stars and act preemptively.
  fTurnOnFlag = true;   // by default decide for self when to restore pixels.
                        // False means QMon tells when to restore pixel.
  fApertureParam0 = 0;  // If vhv is searching for stars itself, these are
  fApertureParam1 = 0;  // empirically determined coefficients for calculating
  fApertureParam2 = 0;  // aperture as a function of star magnitude.
  fEpoch = 2000;        // epoch of observations... format??
} // end Constructor


/** Destructor:
 */
VHVAutoVoltage::~VHVAutoVoltage()
{
  delete fTimers;

} // end Destructor


/** suppressPixel
 *
 * \arg \c pixel Pixel to be suppressed.
 */
void VHVAutoVoltage::suppressPixel( int pixel )
{
  // Get current pointing of telescope in RA and Dec.
  getTelescopePointing();

  // Calculate pointing of this pixel.

  // Acquire vector of stars from DB.
  vector< StarInfo > nearbyStars = VDBPOS::getNearbyStars(pixelRA, pixelDEC, fEpoch, fMagnitudeLimit, fStarSearchRadius);

  // If # stars = zero, it's a transient.  Start a timer.

  // If # stars >= zero, it's a star.  If # stars == 1, just calculate angle
  // and append to list.  If # stars > 1, use furthest star (most conservative)

} // end suppressPixel

void VHVAutoVoltage::updateAutoVoltage()
{
  // Get current pointing of telescope in RA and Dec.
  getTelescopePointing();

  // Scan over list of star-suppressed pixels:  is it time to restore any yet?

} // end updateAutoVoltage

void VHVAutoVoltage::getTelescopePointing()
{
  // Request pointing information from positioner or DB.

  // Get system time.

  // Calculate current telescope RA and Dec.


} // end getTelescopePointing
