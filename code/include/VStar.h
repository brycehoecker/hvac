///////////////////////////////////////////////////////////////////////////////
// $Id: VStar.h,v 1.2 2006/03/30 17:11:40 humensky Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#ifndef VSTAR_H
#define VSTAR_H

/** \file VStar.h
    \brief Header file for objects in VStar.cpp
*/

#include <stdio.h>
#include <math.h>

#include <qcanvas.h>
#include <qcolor.h>

#include "VHV.h"
#include "astrometry.h"
#include "VHVData.h"

#define MAX_STARS  100 /**< Maximum number of stars in FoV */
#define DATA_WORDS 4   /**< Number of words/record in Catalog file */

/**< Catalog file name */
#define CATALOG   "/usr/local/veritas/SP05/hvac/DataFiles/tycho_catalog_7mag" 

/**********************************************/

class VStar : public QCanvasEllipse {
  
 public:
  VStar(double RA, double Dec,double bmag, double vmag, QCanvas* canvas);
  VStar(double RA, double Dec,double bmag, double vmag, QCanvas* canvas,
       double elevation, double azimuth, double xpos, double ypos);
  virtual ~VStar();
  void draw();
  void setPars(double RA, double Dec, double bmag, double vmag,
	       double el, double az, double xpos, double ypos );
  void setPos(double xpos, double ypos);
  double getRA();
  double getDec();
  void print();
  int getX();
  int getY();
  void setX( int x );
  void setY( int y );
  double getVMagnitude();
  double getBMagnitude();

 private:
  double fRA;
  double fDec;
  double fElevation;
  double fAzimuth;
  double fBMag;
  double fVMag;
  QColor fColour;
  int    fSize;
  int fX; // position of star in GUI.
  int fY; // position of star in GUI.

};

/**********************************************/

class VStarLabel : public QCanvasText {
  
 public:
  VStarLabel(double RA, double Dec,double bmag, double vmag, QCanvas* canvas);
  VStarLabel(double RA, double Dec,double bmag, double vmag, QCanvas* canvas,
       double elevation, double azimuth, double xpos, double ypos);
  virtual ~VStarLabel();
  void draw();
  void setPars(double RA, double Dec, double bmag, double vmag,
	       double el, double az, double xpos, double ypos );
  void setPos(double xpos, double ypos);
  double getRA();
  double getDec();
  
 private:
  double fRA;
  double fDec;
  double fElevation;
  double fAzimuth;
  double fBMag;
  double fVMag;
  QColor fColour;
  int    fSize;

};

/**********************************************/

class VStarArc : public QCanvasPolygonalItem  {
  
 public:
  VStarArc(QPointArray points, QCanvas *canvas );
  virtual ~VStarArc();
  void drawShape( QPainter &p );
  QPointArray areaPoints() const;
  void newTrack( QPointArray points );

 private:
  QPointArray fTrack;

};

/**********************************************/

class VStarField {

 public:
  VStarField(QCanvas *c, VHVData *data, double Ra, double Dec, double fov, 
	    double degs_per_pixel, int x, int y, double maxmag);
  virtual ~VStarField();
  void setPointing(double Ra, double Dec);
  void getPointing(double &Ra, double &Dec);
  void readStars();
  void calculateField();
  void cameraPos( VStar *star, VStarLabel *l, double st );
  void draw();
  void hide();
  void timeArc( double length );
  void hideArcs();
  void calcSourcePos();
  void changeMagnitude( double maxmag );
  double getRA();
  double getDec();

 private:
  QCanvas* fCanvas;
  VHVData* fData;
  VStar* fStars[MAX_STARS];
  VStarLabel* fStarLabels[MAX_STARS];
  VStarArc* fArcs[MAX_STARS];
  int fNumStars;
  double fRA;
  double fDec;
  double fElevation;
  double fAzimuth;
  double fHourAngle;
  double fSiderealTime;
  double fFOV;
  double fScale;
  double fMaxMagnitude;
  int fCentreX;
  int fCentreY;
  double fArcLength;
  bool fArcsOn;
  bool fStarsOn;

};

/**********************************************/

#endif
