///////////////////////////////////////////////////////////////////////////////
// $Id: astrometry.cpp,v 1.1.1.1 2006/01/18 20:18:51 ergin Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
/** \file astrometry.cpp
 * \brief Global astronometric routines.
 */

#include "astrometry.h"
extern double kLatitude;
extern double kLongitude;
extern ofstream kLogFile;
/** 
 *  Calculates local (altitude, azimuth) coords. given (RA, dec.) 
 */
void calcSkyPosition(double lat, double dec, double hr_ang, 
		       double &el, double &az)
{

  double h = hr_ang * DEG2RAD;
  double d = dec * DEG2RAD;
  double l = lat * DEG2RAD;    
  
  az = RAD2DEG * atan2( -sin(h) * cos(d), cos(l)*  sin(d) -
			sin(l) * cos(d) * cos(h) );
  el = RAD2DEG * asin(sin(l) * sin(d) + cos(l) * cos(d) * cos(h)) ;
  
  if (az < 0.0) az += 360.0;

}

/** 
 *  Calculates position in camera plane given local coords.  
 */
void calcCameraPosition(double el_cen, double az_cen,
			double el, double az,
			double &x, double &y, double scale)
{

  /**  Projection of spherical coordinates onto tangent plane
   **  ('gnomonic' projection - 'standard coordinates').
   */
  
  double sel_cen, sel, cel_cen, cel, az_dif, saz_dif, caz_dif, denom;
  double xr,yr;
  
  /* Trig functions */
  sel_cen = sin ( el_cen*DEG2RAD );
  sel = sin ( el*DEG2RAD );
  cel_cen = cos ( el_cen*DEG2RAD );
  cel = cos ( el*DEG2RAD );
  az_dif = (az - az_cen)*DEG2RAD;
  saz_dif = sin ( az_dif );
  caz_dif = cos ( az_dif );
 
  /* Reciprocal of star vector length to tangent plane */
  denom = sel * sel_cen + cel * cel_cen * caz_dif;
  
  /* Handle vectors too far from axis */
  
  if (( denom > 0.0 )&&( denom < TINY )) {
    denom = TINY;
  } else if (( denom < 0.0 )&&( denom > -TINY )) {
    denom = -TINY;
  } 
   
  /* Compute tangent plane coordinates in RADIANS */
  xr = cel * saz_dif / denom;
  yr = ( sel * cel_cen - cel * sel_cen * caz_dif ) / denom;
  
  /* Convert to focal plane scale and flip */
  x = -RAD2DEG*xr/scale;
  y = RAD2DEG*yr/scale;
  
}

/*--------------------------------------------------------*/

/** Get the current value of the Modified Julian Date */
double getCurrentMJD() {

  time_t sec1970 = time(0);
  time_t sec2001 = sec1970 - 978307200;
  double mjd = 51910.0 + (sec1970/SECSPERDAY);
  
  return(mjd);
}


/** Convert timestamp from database to MJD */
// database timestamps are unsigned long long and 
// have a format yyyymmddhhmmsssss (last 3 are millisecs)  
double convertDBTimeToMJDdouble( unsigned long long dbtime ) 
{
  double djm=0;
  ostringstream os;
  os.str("");
  os << dbtime;
  int yr = atoi(os.str().substr(0,4).c_str());
  int mo = atoi(os.str().substr(4,2).c_str());
  int dy = atoi(os.str().substr(6,2).c_str());
  int hr = atoi(os.str().substr(8,2).c_str());
  int mn = atoi(os.str().substr(10,2).c_str());
  int sc = atoi(os.str().substr(12,5).c_str());
  int status = 0;
  slaCldj( yr, mo, dy, &djm, &status );
  double mjd = djm + ( (sc/1000.+60*(mn+60*hr)) / SECSPERDAY );
//   cout << yr << " " << mo << " " << dy << " " << hr << " " << mn
//        << " " << sc << " " << status << " " << mjd << endl;
  if ( status ) {
    kLogFile << "convertDBTimeToMJDdouble: status is " << status
	     << " for dbtime " << dbtime << " - got mjd = " << mjd
	     << endl;
  }
  return(mjd);
}


/** Convert Modified Julian Date to Greenwich Mean Sidereal Time) */
double MJD2GMST ( double ut1 )
//double slaGmst ( double ut1 )
/*
**  - - - - - - - -
**   s l a G m s t
**  - - - - - - - -
**
**  Conversion from Universal Time to Sidereal Time.
**
**  (double precision)
**
**  Given:
**    ut1    double     Universal Time (strictly UT1) expressed as
**                      Modified Julian Date (JD-2400000.5)
**
**  The result is the Greenwich Mean Sidereal Time (double
**  precision, radians).
**
**  The IAU 1982 expression (see page S15 of the 1984 Astronomical
**  Almanac) is used, but rearranged to reduce rounding errors.
**  This expression is always described as giving the GMST at
**  0 hours UT.  In fact, it gives the difference between the
**  GMST and the UT, which happens to equal the GMST (modulo
**  24 hours) at 0 hours UT each day.  In this routine, the
**  entire UT is used directly as the argument for the
**  standard formula, and the fractional part of the UT is
**  added separately;  note that the factor 1.0027379... does
**  not appear.
**
**  See also the routine slaGmsta, which delivers better numerical
**  precision by accepting the UT date and time as separate arguments.
**
**  Called:  slaDranrm
**
**  Defined in slamac.h:  D2PI, DS2R, dmod
**
**  Last revision:   19 March 1996
**
**  Copyright P.T.Wallace.  All rights reserved.
*/
{
   double tu;

/* Julian centuries from fundamental epoch J2000 to this UT */
   tu = ( ut1 - 51544.5 ) / 36525.0;

/* GMST at this UT */
   return NormalizeAngle
     ( dmod ( ut1, 1.0 ) * D2PI + (24110.54841 + ( 8640184.812866 
      + (0.093104 - 6.2e-6 * tu ) * tu ) * tu ) * DS2R );
}


/** Normalize angle to the range 0-2 Pi */

double NormalizeAngle ( double angle )
//double slaDranrm ( double angle )
/*
**  - - - - - - - - - -
**   s l a D r a n r m
**  - - - - - - - - - -
**
**  Normalize angle into range 0-2 pi.
**
**  (double precision)
**
**  Given:
**     angle     double      the angle in radians
**
**  The result is angle expressed in the range 0-2 pi (double).
**
**  Defined in slamac.h:  D2PI, dmod
**
**  Last revision:   19 March 1996
**
**  Copyright P.T.Wallace.  All rights reserved.
*/
{
   double w;

   w = dmod ( angle, D2PI );
   return ( w >= 0.0 ) ? w : w + D2PI;
}


void AzEl2RADec2000(double fAz, double fEl, double mjdTime,
		    double& fRA2000,double& fDec2000)
// ************************************************************************                          
// Convert Az and El at fTime to RaDec epoch 2000 - borrowed from vegas                      
// ************************************************************************                     
{
  double fHourangle=0;
  double fDecApparent=0;
  slaDh2e(fAz, fEl, kLatitude*DEG2RAD, &fHourangle, &fDecApparent);

  //convert hour angle back to ra                                                                                                                                        
  //double fLocalSidereal = slaGmst(fTime.getMJDDbl()) + fEastLongitude;                                                                                                 
  //radians                                                                                                                                                              
  double fLocalSidereal = slaGmst(mjdTime) + kLongitude*DEG2RAD +
    slaEqeqx(mjdTime);
  double fRAApparent=(fLocalSidereal-fHourangle);

  // Convert the apparent RA and Dec to the RA Dec for epoch 2000                                                                                                        
  slaAmp(fRAApparent,fDecApparent,mjdTime,2000,&fRA2000,&fDec2000);
  fRA2000=slaDranrm(fRA2000);
  fDec2000=slaDrange(fDec2000);
  return;
}
// ************************************************************************                                                                                              
