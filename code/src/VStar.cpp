///////////////////////////////////////////////////////////////////////////////
// $Id: VStar.cpp,v 1.2 2006/03/28 04:50:53 humensky Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
/** \file VStar.cpp
 * \brief Definition of VStar and related classes.
 */

#include <stdlib.h> 
#include <iostream>

#include "VStar.h"
#include "VHVControl.h"

// double kLatitude   = 31.68; // ridge
// double kLongitude  = -110.88; //ridge
// double kLatitude   = 31.66747; // basecamp
// double kLongitude  = -110.9528; // basecamp
double kLatitude   = 31.6747; // basecamp from VAArrayInfoFactoryLite.cpp
double kLongitude  = -110.9528; // basecamp from VAArrayInfoFactoryLite.cpp


using namespace std;

/**********************************************************************/

/** \class VStar  VStar.h
 * \brief This class defines the icon and behaviour of a star in the HV GUI  
 * \sa VStarField and VStarArc
 */

/** Constructor
 * \arg \c RA   - Right Ascension of target (degrees)
 * \arg \c Dec  - Declination of target (degrees)
 * \arg \c bmag - B band magnitude
 * \arg \c vmag - V band magnitude
 * \arg \c canvas - Pointer to the display canvas
 */
VStar::VStar(double RA, double Dec, double bmag, double vmag,
	   QCanvas* canvas) : QCanvasEllipse(canvas)
{
  setPars(RA, Dec,  bmag, vmag, 0, 0, 0, 0);
 
}

/** Alternative constructor - define all coordinates
 * \arg \c RA   - Right Ascension of target (degrees)
 * \arg \c Dec  - Declination of target (degrees)
 * \arg \c bmag - B band magnitude
 * \arg \c vmag - V band magnitude
 * \arg \c canvas - Pointer to the display canvas
 * \arg \c el    - Elevation (degrees)
 * \arg \c az    - Azimuth (degrees)
 * \arg \c xpos  - Camera position East (pixels)
 * \arg \c ypos  - Camera position North (pixels)
 */
VStar::VStar(double RA, double Dec, double bmag, double vmag, 
	   QCanvas* canvas,
	   double el, double az, double xpos, double ypos) 
  : QCanvasEllipse(canvas)
{

  setPars(RA, Dec,  bmag, vmag, el, az, xpos, ypos);

}

/** Destructor
 */
VStar::~VStar() {}

/** Define parameters/coordinates of the star.
 * \arg \c RA   - Right Ascension of star (degrees)
 * \arg \c Dec  - Declination of star (degrees)
 * \arg \c bmag - B band magnitude
 * \arg \c vmag - V band magnitude
 * \arg \c el    - Elevation (degrees)
 * \arg \c az    - Azimuth (degrees)
 * \arg \c xpos  - Camera position East (pixels)
 * \arg \c ypos  - Camera position North (pixels)
 */
void VStar::setPars( double RA, double Dec, double bmag, double vmag,
		    double el, double az, double xpos, double ypos )
{
  
  fRA = RA;
  fDec = Dec;
  fBMag = bmag;
  fVMag = vmag;
  fAzimuth = az;
  fElevation = el;
  
  fSize = (int)((10.0 - fBMag) * 2.5);
  fColour = getColor(fVMag-fBMag, -1.0, 1.0, CS_STAR);

  setSize(fSize,fSize);

  move(xpos,ypos);
  fX = xpos;
  fY = ypos;
  setZ(4); //  Put stars in front of pixel numbers (z=3) and icons (z=1)
  
}

/** Set the star position in camera coordinates.
 * \arg \c xpos  - Camera position East (pixels)
 * \arg \c ypos  - Camera position North (pixels)
 */
void VStar::setPos(double X, double Y)
{
  
  move(X,Y);
  
}

/** Return the stars Right Ascension in degrees.
 */
double VStar::getRA() { return(fRA); }

/** Return the stars Declination in degrees.
 */
double VStar::getDec() { return(fDec); }

/** Draw the star on the canvas
 */
void VStar::draw()
{
  
  setBrush( fColour );
  show();
  
}



/**********************************************************************/

/** \class VStarLabel  VStar.h
 * \brief This class defines the icon and behaviour of a star in the HV GUI  
 * \sa VStarField and VStarArc
 */

/** Constructor
 * \arg \c RA   - Right Ascension of target (degrees)
 * \arg \c Dec  - Declination of target (degrees)
 * \arg \c bmag - B band magnitude
 * \arg \c vmag - V band magnitude
 * \arg \c canvas - Pointer to the display canvas
 */
VStarLabel::VStarLabel(double RA, double Dec, double bmag, double vmag,
	   QCanvas* canvas) : QCanvasText(canvas)
{

  setPars(RA, Dec,  bmag, vmag, 0, 0, 0, 0);
 
}

/** Alternative constructor - define all coordinates
 * \arg \c RA   - Right Ascension of target (degrees)
 * \arg \c Dec  - Declination of target (degrees)
 * \arg \c bmag - B band magnitude
 * \arg \c vmag - V band magnitude
 * \arg \c canvas - Pointer to the display canvas
 * \arg \c el    - Elevation (degrees)
 * \arg \c az    - Azimuth (degrees)
 * \arg \c xpos  - Camera position East (pixels)
 * \arg \c ypos  - Camera position North (pixels)
 */
VStarLabel::VStarLabel(double RA, double Dec, double bmag, double vmag, 
	   QCanvas* canvas,
	   double el, double az, double xpos, double ypos) 
  : QCanvasText(canvas)
{

  setPars(RA, Dec,  bmag, vmag, el, az, xpos, ypos);

}

/** Destructor
 */
VStarLabel::~VStarLabel() {};

/** Define parameters/coordinates of the star.
 * \arg \c RA   - Right Ascension of star (degrees)
 * \arg \c Dec  - Declination of star (degrees)
 * \arg \c bmag - B band magnitude
 * \arg \c vmag - V band magnitude
 * \arg \c el    - Elevation (degrees)
 * \arg \c az    - Azimuth (degrees)
 * \arg \c xpos  - Camera position East (pixels)
 * \arg \c ypos  - Camera position North (pixels)
 */
void VStarLabel::setPars( double RA, double Dec, double bmag, double vmag,
		    double el, double az, double xpos, double ypos )
{
  
  fRA = RA;
  fDec = Dec;
  fBMag = bmag;
  fVMag = vmag;
  fAzimuth = az;
  fElevation = el;
  QFont aFont( "Helvetica", 14, QFont::Bold );
  setFont( aFont );
  stringstream textStream("");
  textStream << fixed << setprecision( 3 ) << fRA << ", " << fDec
	     << ": " << fBMag;
  setText( textStream.str().c_str() );
  //fSize = (int)((10.0 - fBMag) * 2.5);
  fColour = getColor(fVMag-fBMag, -1.0, 1.0, CS_STAR);

  //setSize(fSize,fSize);

  move(xpos+15,ypos-15);
  setZ(4); //  Put stars in front of pixel numbers (z=3) and icons (z=1)

}

/** Set the star position in camera coordinates.
 * \arg \c xpos  - Camera position East (pixels)
 * \arg \c ypos  - Camera position North (pixels)
 */
void VStarLabel::setPos(double X, double Y)
{

  move(X,Y);

}

/** Return the stars Right Ascension in degrees.
 */
double VStarLabel::getRA() { return(fRA); }

/** Return the stars Declination in degrees.
 */
double VStarLabel::getDec() { return(fDec); }

/** Draw the star on the canvas
 */
void VStarLabel::draw()
{

  setColor( fColour );
  show();

}

/**********************************************************************/

/** \class VStarField  VStar.h
 * \brief This class contains an array of VStar objects...
 */

/** Constructor 
 * \arg \c c    - Pointer to the display canvas
 * \arg \c RA   - Right Ascension of target (degrees)
 * \arg \c Dec  - Declination of target (degrees)
 * \arg \c fov  - Field of view of the telescope
 * \arg \c degs_per_pixel - Camera scale
 * \arg \c x    - Centre of Camera - x-pixel 
 * \arg \c y    - Centre of Camera - y-pixel
 * \arg \c maxmag - Maximum B-band magnitude of stars displayed 
 */

VStarField::VStarField(QCanvas *c, VHVData *data, double RA, double Dec, 
		       double fov, 
		       double degs_per_pixel, int x, int y, double maxmag)
{
//   cout << "x=" << x << endl;
//   cout << "y=" << y << endl;
  
  fCanvas = c;
  fData = data;

  fRA = RA;
  fDec = Dec;
  fFOV = fov;
  fScale = degs_per_pixel;
  fCentreX = x;
  fCentreY = y;
  fMaxMagnitude = maxmag;  
  fNumStars = 0;
  
  calcSourcePos();

  for(int i=0; i<MAX_STARS; i++)
    { 
      fStars[i] = 0;
      fStarLabels[i] = 0;
    }
  fArcsOn = TRUE;
  fStarsOn = TRUE;

}

/** Destructor
 */
VStarField::~VStarField()
{
  
  for(int i=0; i<MAX_STARS; i++) 
    delete fStars[i];
  for(int i=0; i<MAX_STARS; i++) 
    delete fStarLabels[i];
  for(int i=0; i<MAX_STARS; i++) 
    delete fArcs[i];
  
}

/** Reset the target position
 * \arg \c RA   - Right Ascension of target (degrees)
 * \arg \c Dec  - Declination of target (degrees)
 */
void VStarField::setPointing(double RA, double Dec)
{
  
  fRA = RA;
  fDec = Dec;
  
  bool remember_stars = fStarsOn;
  bool remember_arcs = fArcsOn;
  
  if (fStarsOn) hide();
  if (fArcsOn) hideArcs();
  
  calcSourcePos();
  
  readStars();
  
  if (remember_stars) draw();
  if (remember_arcs) timeArc( 0 );
  
}

/** Get the current target position
 * \arg \c RA   - Right Ascension of target (degrees)
 * \arg \c Dec  - Declination of target (degrees)
 */
void VStarField::getPointing(double& RA, double& Dec)
{
  
  RA = fRA;
  Dec = fDec;
  
}

/** Get the current target RA.
 */
double VStarField::getRA()
{
  return fRA;
}

/** Get the current target Dec.
 */
double VStarField::getDec()
{
  return fDec;
}

/** Change the limiting B-band magnitude 
 * \arg \c maxmag - Maximum B-band magnitude of stars displayed 
 */
void VStarField::changeMagnitude(double maxmag)
{
  
  printf("New magnitude %f\n",maxmag); 
  
  bool remember_stars = fStarsOn;
  bool remember_arcs = fArcsOn;

  if (fStarsOn) hide();
  if (fArcsOn) hideArcs();

  fMaxMagnitude = maxmag;
  readStars();
  
  if (remember_stars) draw();
  if (remember_arcs) timeArc( 0 );
  
}

/** Read from a catalog file to produce a list of stars to display
 */
void VStarField::readStars()
{
  
  double data[DATA_WORDS];
  
  FILE *catalog = fopen(CATALOG,"r");
  if (catalog == 0) {
    fprintf(stderr,"File %s not found\n",CATALOG);
    exit(-1);
  }
  
  fNumStars = 0;

  while( fread( data, sizeof(double), DATA_WORDS, catalog ) && 
	 (fNumStars < MAX_STARS) ) {
    
    double sra  = data[0];
    double sdec = data[1];    
    
    if (fabs(sdec-fDec) < fFOV*0.6) {
      double ha = fSiderealTime - sra;
      double el=0,az=0,dx=0,dy=0;
      calcSkyPosition(kLatitude, sdec, ha, el, az);
      calcCameraPosition(fElevation, fAzimuth, el, az, dx, dy, 1.0);

//       cout << "test " << fCentreX << " " << fCentreY << " " << dx << " " 
// 	   << dy << " " << fScale << endl;

      if (sqrt(dx*dx+dy*dy) < fFOV * 0.6) {
	double vmag = data[2];
	double bmag = data[3];
	if ( bmag < fMaxMagnitude ) {
	  if (fStars[fNumStars] == 0) {
	    fStars[fNumStars] = new VStar(sra, sdec, bmag, vmag, fCanvas);
	    fStars[fNumStars]->move(fCentreX+dx/fScale, fCentreY+dy/fScale);
	    fStarLabels[fNumStars] = 
	      new VStarLabel(sra, sdec, bmag, vmag, fCanvas);
	    fStarLabels[fNumStars]->
	      move(fCentreX+dx/fScale, fCentreY+dy/fScale);
	  } else {
	    fStars[fNumStars]->setPars( sra, sdec, bmag, vmag, el, az,
				  fCentreX+dx/fScale, fCentreY+dy/fScale );
	    fStarLabels[fNumStars]->setPars( sra, sdec, bmag, vmag, el, az,
				  fCentreX+dx/fScale, fCentreY+dy/fScale );
	  }
	  fNumStars++;	  
	}
      }
    }
//     kLogFile << fNumStars << "  ra=" << sra << "  dec=" << sdec
// 	     << "  bmag=" << data[3] << endl;
  }

  fclose(catalog);
  
}
  
/** Calculate the positions of all field stars in camera coordinates
 */
void VStarField::calculateField()
{
  
  calcSourcePos();

  (void) calcSourcePos();

  for (int i=0; i<fNumStars; i++)
    {
      cameraPos(fStars[i],fStarLabels[i],fSiderealTime);
      fStars[i]->draw();
      fStarLabels[i]->draw();
    }
  
}

/** Calculate the position of a given star in camera coordinates
 * \arg \c s - Pointer to the star object (VStar)
 * \arg \c st - Sidereal time (in ?)
 */
void VStarField::cameraPos(VStar *s, VStarLabel *l, double st) 
{
  
  double el=0;
  double az=0;
  double ha = st - s->getRA();
  if (ha < 0) ha += 360.0;
  calcSkyPosition(kLatitude, s->getDec(), ha, el, az);
  double dx=0,dy=0;
  calcCameraPosition(fElevation, fAzimuth, el, az, dx, dy, fScale);
  
  s->move(dx+fCentreX,dy+fCentreY);
  l->move(dx+fCentreX,dy+fCentreY);

}

/** Draw all of the stars in the field
 */
void VStarField::draw()
{
  
  fStarsOn = TRUE;

  for (int i=0; i<fNumStars; i++)
    {
      fStars[i]->draw();
      //fStarLabels[i]->draw();
    }
  
}

/** Hide all of the stars in the field
 */
void VStarField::hide()
{
  
  fStarsOn = FALSE;
  
  for (int i=0; i<fNumStars; i++)
    {
      fStars[i]->hide();
      //fStarLabels[i]->hide();
    }
  
}

/** Hide all of the arc objects (VStarArc) in the field
 */
void VStarField::hideArcs()
{
  
  fArcsOn = FALSE;
  
  for (int i=0; i<fNumStars; i++) {
//     fArcs[i]->hide();
    fStarLabels[i]->hide();
  }
  
}

/** Generate a VStarArc object for each VStar in the field
 * \arg \c length - length of the arc in time (minutes)
 */
void VStarField::timeArc(double length)
{
  
//   if (length <= 0) length = fArcLength;

// //   double dt = 0.4;
//   double dt = 2.0;
//   int steps = (int)(length/dt);
//   QPointArray path(steps);
//   cout << "Made QPointArray" << endl;
  fArcsOn = TRUE;

//   calcSourcePos();
//   cout << "Calculated source position" << endl;
  
//   for (int i = 0; i < fNumStars; i++) {
//     double t = fSiderealTime;
//     for (int j = 0; j < steps; j++) {
//       double el=0,az=0;
//       double ha = t - fRA;
//       calcSkyPosition(kLatitude, fDec, ha, el, az);
//       cout << "Calculated sky position 1st time" << endl;
//       double dx=0,dy=0,saz=0,sel=0;
//       double sha = t - fStars[i]->getRA();
//       calcSkyPosition( kLatitude, fStars[i]->getDec(), sha, sel, saz );
//       cout << "Calculated sky position 2nd time" << endl;
//       calcCameraPosition( el, az, sel, saz, dx, dy, fScale );
//       cout << "Calculated camera position" << endl;
//       dx += fCentreX;
//       dy += fCentreY;
//       path.setPoint(j,(int)dx,(int)dy);
//       //      printf("%d %d %f %d %d : %f %f\n",i,j,t,(int)dx,(int)dy,az,el);
//       t += dt; 
//     }
//     cout << "Entering if" << endl;
//     if (fArcs[i]) {
//       cout << "\t top branch" <<endl;
//       fArcs[i]->newTrack( path );
//       cout << "\t assigned newTrack" << endl;
//     } else {
//       cout << "\t bottom branch" <<endl;
//       fArcs[i] = new VStarArc( path, fCanvas );
//       cout << "\t assigned new VStarArc" <<endl;
//     }
//     cout << "showing arc " << i << endl;
//     fArcs[i]->show();
//   }
  
//   fArcLength = length;
   for (int i = 0; i < fNumStars; i++)
     {
       fStarLabels[i]->draw();  
     }
}

/** Calculate the position of the target in local coordinates (az,el) 
 */
void VStarField::calcSourcePos()
{
  
  double gmst = RAD2DEG*MJD2GMST(getCurrentMJD());
  fSiderealTime = gmst + kLongitude; // in degrees
  fHourAngle = fSiderealTime - fRA;

  if (fHourAngle < 0) fHourAngle += 360.0;
  
  calcSkyPosition(kLatitude, fDec, fHourAngle, fElevation, fAzimuth);
  fData->setElevation( fElevation );
  fData->setAzimuth( fAzimuth );
  
}



/******* VStarArc ****************************************************/

/** \class VStarArc VStar.h
 * \brief This class defines the arc described by a star in the camera plane
 */

/** Constructor
 * \arg \c points - An array of points describing the arc
 * \arg \c canvas - A pointer to the display canvas
 */
VStarArc::VStarArc(QPointArray points, QCanvas *canvas ) : 
  QCanvasPolygonalItem(canvas)
{

  fTrack = points.copy();

}  

/** Destructor
 */
VStarArc::~VStarArc() {};

/** Draw the arc using the given QPainter
 * \arg \c p - A Qt pen style/colour object
 */
void VStarArc::drawShape( QPainter &p )
{

  p.setPen( QPen( blue, 4, SolidLine ) );
  p.drawPolyline( fTrack );
  
}

/** Return an array defining the boundary of this graphical object
 */
QPointArray VStarArc::areaPoints() const
{

  QRect rect;

  rect = fTrack.boundingRect();
  
  QPointArray points( rect, TRUE );

  return( points );

}

/** Redefine the points defining the arc
 * \arg \c points - Array of points along the arc.
 */
void VStarArc::newTrack( QPointArray points )
{
  cout << "In newTrack" << endl;
//   fTrack = points.copy();
  fTrack = points;
  fTrack.detach();
  
}

