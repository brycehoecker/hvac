// Class to implement a Pattern Selection Trigger (Level 2)

#ifndef VHVPST_H
#define VHVPST_H

#include <string>
#include <vector>
#include <bitset>
#include <iostream>
#include <fstream>

#include "VHVData.h"

using namespace std;

// PST definitions
#define PIXELSINCAMERA   499
#define PATCHESINCAMERA  91
#define PIXELSINPATCH    19
#define MAXPATTERNS      1000
#define MAXNUMPATCHES    100

class VHVPST
{

 public:
  
  /// Default Constructor
  VHVPST( VHVData* data );

  /// Default Destructor.
  ~VHVPST();

  /// Method to search CFD triggers for a pattern trigger.
  //    Returns the time of trigger, or -1 if no trigger found.
  vector< int > evaluateTrigger( vector< int > triggers );

  /** Method to set the CFD trigger mask.  This mask determines which CFDs are excluded
      from the L2 trigger (in addition to those that are already not in patches) */
  void setCFDMask();

 private:
  VHVData *fData;
  int fMultiplicity;
  bitset<PIXELSINCAMERA> fCFDMask;
  bool   fPatchMask[PATCHESINCAMERA];
  int   fPatchContents[PATCHESINCAMERA][PIXELSINPATCH];
  int   fNumValidPatterns;
  bitset<PIXELSINPATCH>   fValidPatterns[MAXPATTERNS];

  /// Method to load nearest-neighbor lists.  Borrowed from GrISU read_array.h,
  /// get_pst() written by J. Holder.
  void getPST();

  /// Method to determine acceptable trigger patterns. Borrowed from GrISU read_array.h,
  /// find_valid_patterns() written by J. Holder.
  //    Called by constructor.
  void findAcceptablePatterns();

};

#endif // VHVPST_H
