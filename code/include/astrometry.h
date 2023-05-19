///////////////////////////////////////////////////////////////////////////////
// $Id: astrometry.h,v 1.1.1.1 2006/01/18 20:18:51 ergin Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#ifndef ASTROMETRY_H
#define ASTROMETRY_H

/**
 * \file astrometry.h
 * \brief File containing definitions of functions in astrometry.cpp
 */

#include <math.h>
#include <time.h>
#include <cstdio>
#include "VStar.h"
#include "VASlalib.h"
#include "VASlamac.h"

#ifndef TRIG_CONV
#define TRIG_CONV
#define DEG2RAD    0.017453  /**< Degree to Radian conversion factor */
#define RAD2DEG    57.29578  /**< Radian to Degree conversion factor */
#endif

#define TINY 1e-7 /**< Precision limit for trigonometry */

/** 2 * pi */
#define D2PI 6.2831853071795864769252867665590057683943387987502

/** pi/(12*3600):  seconds of time to radians */
#define DS2R 7.2722052166430399038487115353692196393452995355905e-5

/** dmod(A,B) - A modulo B (double) */
#define dmod(A,B) ((B)!=0.0?((A)*(B)>0.0?(A)-(B)*floor((A)/(B))\
                                        :(A)+(B)*floor(-(A)/(B))):(A))

/** number of seconds in a day */
#define SECSPERDAY (60.0*60*24.0)


void calcSkyPosition(double lat, double dec, double hr_ang, 
		     double &el, double &az);

void calcCameraPosition(double el_cen, double az_cen,
			double el, double az,
			double &x, double &y, double scale);

double getCurrentMJD();
double convertDBTimeToMJDdouble( unsigned long long dbtime );
double NormalizeAngle ( double angle );
double MJD2GMST ( double ut1 );

void AzEl2RADec2000(double fAz, double fEl, double mjdTime,
		    double& fRA2000,double& fDec2000);

#endif
