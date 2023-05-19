///////////////////////////////////////////////////////////////////////////////
// $Id: VHV.h,v 1.2 2006/03/21 04:59:47 humensky Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#ifndef VHV_H
#define VHV_H

/*! \file VHV.h 
  \brief Header file for miscellaneous constants of the HV code and global 
         variables.
*/

#ifndef TRIG_CONV
#define TRIG_CONV
#define DEG2RAD    0.017453 /**< Degree to Radian conversion factor */
#define RAD2DEG    57.29578 /**< Radian to Degree conversion factor */
#endif

#define NUMTEL 4              /**< Number of telescopes in array */
#define MAXVOLTS         1500 /**< Maximum allowable board voltage (V) */
#define MINVOLTS         0    /**< Minimum allowable voltage (V) */
#define MINDROP          100  /**< Minimum board to channel voltage drop (V) */
#define MAX_BOARDS       11   /**< Maximum number of HV boards */
#define CHANS_PER_BOARD  48   /**< Number of channels per HV board */
#define MAX_PMTS         MAX_BOARDS * CHANS_PER_BOARD /**< Maximum number of PMTs */
#define NUM_CRATES       2    /** Number of crates vhv is talking to */
#define NUMSENSORS       6  /** Number of camera sensors to display */

#define VOK              1  /**< Flag for success */
#define VERROR           0  /**< Flag for failure */

#define SUPPRESSED       0 /* Flag for HV channel zeroed out TBH 041119 */
#define RESTORED         1 /**< Flag for HV channel not zeroed out TBH 041119*/

#ifndef ONOFF
#define ONOFF
/** \enum EnumPower
 * Power ON/OFF enumeration
 */
enum EnumPower { OFF, ON };
#endif 

/** \enum EnumDisplay
 * User selected display option enumeration 
 */
enum EnumDisplay { IPMT, VREQ, VSET, VTRUE, DELTAV1, DELTAV2, POWER, IBOARD }; 

/** \enum EnumSetBy
 * Set By Who? enumeration 
 */
enum EnumSetBy {EThread, EUser};

/** \enum EnumStatus
 *  Channel status enumeration 
 */
enum EnumStatus {EInactive, EKnown, EUnknown};

/** \enum EnumColorScheme
 * Colour scheme enumeration for getColor()
 */
enum EnumColorScheme { CS_GREY, CS_STD, CS_STAR, CS_PMT, 
		       CS_PMT_HOT, CS_PMT_COOL };

/** \enum EnumStarOffset
 * Offset in star display -- ON source, Off before ON, OFF after ON.
 */
enum EnumStarOffset { ONSOURCE, OFFBEFORE, OFFAFTER };

enum EnumCameraSensor { SENSOR_CAMTEMP, SENSOR_AMBTEMP, SENSOR_CAMHUM,
			SENSOR_AMBHUM, SENSOR_LIGHT1, SENSOR_LIGHT2 };

#endif
