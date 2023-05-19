/**
 * \class VHVPST
 * \ingroup VHV
 * \brief Class to model the Pattern Selection Trigger.
 *
 * Here is a tedious verbose multi-line description of all
 * the details of the code, more than you would
 * ever want to read. Generally, all the important documentation
 * goes in the .cpp files.
 *
 * Original Author: B. Humensky
 * $Author: humensky $
 * $Date: 2005/12/20 22:27:38 $
 * $Revision: 1.1 $
 * $Tag$
 *
 **/

#include "VHVPST.h"


/// Default Constructor
VHVPST::VHVPST( VHVData* data )
  : fData( data )
{
//   cout << "entering VHVPST constructor" << endl;
  fMultiplicity = fData->getPatternMultiplicity();
  getPST();
  setCFDMask();
}

/// Default Destructor.
VHVPST::~VHVPST()
{
}


/** Method to search CFD triggers for a pattern trigger.  Simple algorithm to
    start with.  Requires CFD triggers to be present both now and fMinLength 
    (eg, 5 ns) ago.  Breaks out of search loop after 1st valid pattern is found
    and uses fLocalTime as the trigger time -- this means trigger time is good
    only to 2 ns!  Will update later with a more sophisticated routine that looks
    for all valid patterns and determines which triggered first. (Sounds like
    that will run slower, however) */
// Requires pixel numbers -- counting starting at 1, not 0!
vector< int > VHVPST::evaluateTrigger( vector< int > triggers )
{
//   cout << "entering VHVPST::evaluateTrigger() triggers.size()=" 
//        << triggers.size() << endl;
  fMultiplicity = fData->getPatternMultiplicity();
  bool newTrigger = false;
  vector< int > triggeredPixels;
  int patchSum[PATCHESINCAMERA];
  int grandSum = 0;
  bitset<19> patchPattern[PATCHESINCAMERA];
  for ( int i=0; i<PATCHESINCAMERA; ++i ) {
    patchSum[i] = 0;
  }

  // Loop over pixels.
  for ( int pix=0; pix<triggers.size(); ++pix ) {
    // skip pixel if no trigger or if pixel is masked off.
    if ( !fCFDMask[triggers[pix]] ) continue;
    // Figure out which patch this pixel belongs to, and if it is active then 
    // increment that patch's sum and bitset.
    for ( int pat=0; pat<PATCHESINCAMERA; ++pat ) {
      if ( !fPatchMask[pat] ) continue;
      for ( int ippix=0; ippix<PIXELSINPATCH; ++ippix ) {
	if ( triggers[pix] == fPatchContents[pat][ippix] ) {
	  patchSum[pat]++;
	  grandSum++;
	  patchPattern[pat].set( ippix );
	}
      }
    }
  }

  // First check whether there's enough pixels in the full camera to possibly
  // trigger.  If not, return.
  if ( grandSum >= fMultiplicity ) {
//     cout << "grandSum=" << grandSum << " so looking for trigger." << endl;

    // Now we look for triggers.
    for ( int pat=0; pat<PATCHESINCAMERA; ++pat ) {
      // skip patch if it doesn't have the right multiplicity or if 
      // it is masked off.
      if ( patchPattern[pat].count() < fMultiplicity || !fPatchMask[pat] ) 
	continue;
      // Loop over patterns, looking for a match.
      for ( unsigned int ipt=0; ipt<fNumValidPatterns; ++ipt ) {
	if ( (patchPattern[pat] & fValidPatterns[ipt]) == fValidPatterns[ipt] ) {
	  // It's a trigger.  Set trigger flag and time, and record which pixels
	  // participated in the trigger.  Break out of loops.
	  newTrigger = true;
	  for ( int i=0; i<PIXELSINPATCH; ++i ) {
	    if ( fValidPatterns[ipt][i] ) {
	      triggeredPixels.push_back( fPatchContents[pat][i] );
	    }
	  }
	  break;
	}
      }
      if ( newTrigger ) break;
    }
  }
//   cout << "triggeredPixels.size()=" << triggeredPixels.size() << endl;
  return triggeredPixels;
  
}

/** Method to set the CFD trigger mask.  This mask determines which CFDs are excluded
    from the L2 trigger (in addition to those that are already not in patches) */
void VHVPST::setCFDMask()
{
//   cout << "entering VHVPST::setCFDMask()" << endl;
  for ( int pat=0; pat<PATCHESINCAMERA; ++pat ) {
    for ( int pix=0; pix<PIXELSINPATCH; ++pix ) {
      //cout << pat << "\t" << pix << "\t" << fPatchContents[pat][pix] << endl;
      fCFDMask.set( fPatchContents[pat][pix] );
    }
  }
}


/* ----------------------------------------------------------- */

// Definitions of private methods.


/// Method to load PST information from configuration files.  Modified from GrISU
/// read_array.h get_pst() written by J. Holder.
void VHVPST::getPST()
     /*.....................................................................*/
{
//   cout << "entering VHVPST::getPST()" << endl;
  int i1 = fMultiplicity;
  int i2 = PATCHESINCAMERA;
//   cout << "multiplicity is " << i1 << " and there are " << i2 
//        << " patches in the camera." << endl;

  // Read in the map of which pixels are in which patch (and whether any patches are
  // disabled).
  ifstream infi( "/usr/local/veritas/SP05/vhv/Config/VSEPSTPatchMembers.dat",
		 ios::in );
  int tmp;
  for ( int i=0; i<PATCHESINCAMERA; ++i ) {
    infi >> tmp >> fPatchMask[i];
    for ( int j=0; j<PIXELSINPATCH; ++j ) {
      infi >> fPatchContents[i][j];
    }
  }

  // Find the valid trigger patterns for each patch.
  findAcceptablePatterns();
  //printf("Multiplicity %d found %d valid patterns\n",ta->elec.pst.n_adj,ta->elec.pst.n_valid);

  if (i2>MAXNUMPATCHES || i2<1) 
    cout << "Number of patches out of range" << endl;

  if (i1<1 || i1>4)
    cout << "Multiplicity out of range for PST" << endl;
}


/// Method to load nearest-neighbor lists.  Borrowed from GrISU read_array.h,
/// find_valid_patterns() written by J. Holder.
//    Called by constructor.
void VHVPST::findAcceptablePatterns()
{
//   cout << "entering VHVPST::findAcceptablePatterns()" << endl;
  // Finds the number of valid patterns in a 19 pixel patch for a given 
  // multiplicity

  if (fMultiplicity<1 || fMultiplicity>4) printf("Can't deal with this  multiplicity (in findAcceptablePatterns)\n");

  //(u,v) pixel coordinates for 1 patch
  int u[19]={-0,-1,-2,-2,-2,+1,-0,-1,-1,-1,+2,+1,-0,-0,-0,+2,+1,+1,+2};
  int v[19]={-2,-1,-0,+1,+2,-2,-1,-0,+1,+2,-2,-1,-0,+1,+2,-1,-0,+1,-0};

  // check pixel coordinates are unique
  int i;
  int j;
  int k;

  for (i=0; i<19; i++){
    for (j=i+1; j<19; j++){
      if (u[i]==u[j]) {
	if (v[i]==v[j]) {
	  printf("found two identical pixels (in findAcceptablePatterns)\n");
	  return;
	}
      }
    }
  }
  // check distance from central pixel
  for (i=0; i<19; i++){
    if (abs(u[i])>2 || abs(v[i])>2 || abs(u[i]+v[i])>2 ){    //max dist from centre
      printf("Incorrect pixel coordinate(in findAcceptablePatterns)\n");
      return;
    }
  }

  // Find all patterns with 2 adjacent pixels out of 19
  bitset<19> px[1000];
  int n=0;
  for (i=0; i<19; i++){
    for (j=i+1; j<19; j++){
      if (abs(u[i]-u[j])>1) continue; // skip if not a neighbour
      if (abs(v[i]-v[j])>1) continue; // skip if not a neighbour
      if (abs((u[i]-u[j])-(v[i]-v[j]))<1) continue; 
      px[n].set( i );
      px[n].set( j );
      n+=1;                           // increment counter
    }
  }
  int n2=n;
  bitset<19> p2[n2];
  for ( i=0; i<n2; i++ ) {
    p2[i] = px[i];
    px[i].reset();
  }
  //printf("number of 2-folds=%d\n",n);

  // find all patterns with 3 adjacent pixels
  n=0;
  for (i=0; i<n2; i++){               //loop over all pairs
    for (j=i+1; j<n2; j++){           //of 2-folds
      bitset<19> i19;
      i19 = p2[i] | p2[j];     //take overlap
      if ( i19.count() == 3 ) {       // if 3-fold
	int identical=0;
	for (k=0;k<n;k++){
	  if (px[k]==i19) identical=1;
	}
	if (identical==0) {           // then this is a new 3-fold pattern
	  px[n]=i19;
	  n++;                       
	}      
      }
    }
  }
  
  int n3=n;
  bitset<19> p3[n3];
  for ( i=0; i<n3; i++ ) {
    p3[i] = px[i];
    px[i].reset();
  }
  //printf("number of 3-folds=%d\n",n);


  // find all patterns with 4 adjacent pixels
  n=0;
  for (i=0; i<n2; i++){                    //loop over all 2-folds
    for (j=0; j<n3; j++){                  //loop over all 3-folds
      bitset<19> i19=p2[i] | p3[j];        //take overlap
      if ( i19.count() == 4 ) {            // if 4-fold
	int identical=0;
	for ( k=0; k<n; k++ ){
	  if ( px[k] == i19 ) identical=1;
	}
	if ( identical == 0 ) {            // then this is a new 4-fold pattern	
	  px[n] = i19;
	  n++;                       
	}      
      }
    }
  }  
  int n4=n;
  bitset<19> p4[n4];
  for ( i=0; i<n4; i++ ) {
    p4[i] = px[i];
    px[i].reset();
  }
  //printf("number of 4-folds=%d\n",n);
  
  //Fill Array of valid patterns for requested multiplicity
  switch (fMultiplicity)
    {
    case 1:
	printf("All patterns valid\n");
	break;
    case 2:
      for (i=0;i<n2;i++) fValidPatterns[i]=p2[i];
      fNumValidPatterns=n2; 
      break;
    case 3:
      for (i=0;i<n3;i++) fValidPatterns[i]=p3[i];
      fNumValidPatterns=n3; 
      break;
    case 4:
      for (i=0;i<n4;i++) fValidPatterns[i]=p4[i];
      fNumValidPatterns=n4; 
      break;
    default:
      printf("Can't deal with this multiplicity (in findAcceptablePatterns)\n");
      break;
    }
}
