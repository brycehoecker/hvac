/***************************************************************************/
/*                                                                         */
/*        --- CAEN Engineering Srl - Computing Systems Division ---        */
/*                                                                         */
/*    CAENHVWRAPPER.H                                         		   */
/*                                                                         */
/*                                                                         */
/*    Source code written in ANSI C                                        */
/*                                                                         */ 
/*    Created: March 2000                                                  */
/*    February 2001: Rel. 1.1 : add CAENHVCaenetComm    		         */
/*    June     2001: Rel. 1.2 : restored CAENHVSetSysProp                  */
/*                                                                         */
/***************************************************************************/

#ifndef __CAENHVWRAPPER_H
#define __CAENHVWRAPPER_H


#include "sy1527user.h"
#include "caenhvoslib.h"

#ifndef uchar 
#define uchar unsigned char
#endif
#ifndef ushort 
#define ushort unsigned short
#endif
#ifndef ulong
#define ulong unsigned long
#endif

// Error Codes
#define CAENHV_OK              0
#define CAENHV_CONNECTED	 ((SY1527_MAX_LIB_ERROR) + 1)
#define CAENHV_NOTCONNECTED	 ((SY1527_MAX_LIB_ERROR) + 2)
#define CAENHV_OS    	     ((SY1527_MAX_LIB_ERROR) + 3)
#define CAENHV_LOGINFAILED   ((SY1527_MAX_LIB_ERROR) + 4)
#define CAENHV_LOGOUTFAILED  ((SY1527_MAX_LIB_ERROR) + 5)
#define CAENHV_LINKNOTSUPPORTED  ((SY1527_MAX_LIB_ERROR) + 6)  // Rel. 1.2

// Link Types for InitSystem
#define LINKTYPE_TCPIP		  0
#define LINKTYPE_RS232		  1
#define LINKTYPE_CAENET		  2

// The Error Code type
typedef int CAENHVRESULT;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

CAENHVLIB_API char         *CAENHVGetError(const char *SystemName);

CAENHVLIB_API char         *CAENHVLibSwRel(void);

CAENHVLIB_API CAENHVRESULT  CAENHVInitSystem(const char *SystemName, 
 int LinkType, void *Arg, const char *UserName, const char *Passwd);

CAENHVLIB_API CAENHVRESULT  CAENHVDeinitSystem(const char *SystemName);

CAENHVLIB_API CAENHVRESULT  CAENHVGetChName(const char *SystemName, ushort slot, 
 ushort ChNum, const ushort *ChList, char (*ChNameList)[MAX_CH_NAME]);

CAENHVLIB_API CAENHVRESULT  CAENHVSetChName(const char *SystemName, ushort slot, 
 ushort ChNum, const ushort *ChList, const char *ChName);

CAENHVLIB_API CAENHVRESULT  CAENHVGetChParamInfo(const char *SystemName, 
 ushort slot, ushort Ch, char **ParNameList);

CAENHVLIB_API CAENHVRESULT  CAENHVGetChParamProp(const char *SystemName, 
 ushort slot, ushort Ch, const char *ParName, const char *PropName, void *retval);

CAENHVLIB_API CAENHVRESULT  CAENHVGetChParam(const char *SystemName, ushort slot, 
 const char *ParName, ushort ChNum, const ushort *ChList, void *ParValList);

CAENHVLIB_API CAENHVRESULT  CAENHVSetChParam(const char *SystemName, ushort slot, 
 const char *ParName, ushort ChNum, const ushort *ChList, void *ParValue);

CAENHVLIB_API CAENHVRESULT  CAENHVTestBdPresence(const char *SystemName, 
 ushort slot, ushort *NrofCh, char *Model, char *Description, ushort *SerNum, 
 uchar *FmwRelMin, uchar *FmwRelMax);

CAENHVLIB_API CAENHVRESULT  CAENHVGetBdParamInfo(const char *SystemName, 
 ushort slot, char **ParNameList);

CAENHVLIB_API CAENHVRESULT  CAENHVGetBdParamProp(const char *SystemName, 
 ushort slot, const char *ParName, const char *PropName, void *retval);

CAENHVLIB_API CAENHVRESULT  CAENHVGetBdParam(const char *SystemName, 
 ushort slotNum, const ushort *slotList, const char *ParName, void *ParValList);

CAENHVLIB_API CAENHVRESULT  CAENHVSetBdParam(const char *SystemName, 
 ushort slotNum, const ushort *slotList, const char *ParName, void *ParValue);

CAENHVLIB_API CAENHVRESULT  CAENHVGetGrpComp(const char *SystemName, ushort group, 
 ushort *NrOfCh, ulong **ChList);

CAENHVLIB_API CAENHVRESULT  CAENHVAddChToGrp(const char *SystemName, ushort group, 
 ushort NrOfCh, const ulong *ChList);

CAENHVLIB_API CAENHVRESULT  CAENHVRemChToGrp(const char *SystemName, ushort group, 
 ushort NrOfCh, const ulong *ChList);

CAENHVLIB_API CAENHVRESULT  CAENHVGetGrpParam(const char *SystemName, ushort Group, 
 ushort NrOfPar, const uchar **ParNameList, void *ParValList);

CAENHVLIB_API CAENHVRESULT  CAENHVSetGrpParam(const char *SystemName, ushort Group, 
 const uchar *ParName, void *ParVal);

CAENHVLIB_API CAENHVRESULT  CAENHVGetCrateMap(const char *SystemName,	
 ushort *NrOfSlot, ushort **NrofChList, char **ModelList, char **DescriptionList,
 ushort **SerNumList, uchar **FmwRelMinList, uchar **FmwRelMaxList);

CAENHVLIB_API CAENHVRESULT  CAENHVGetExecCommList(const char *SystemName,
 ushort *NumComm, char **CommNameList);

CAENHVLIB_API CAENHVRESULT  CAENHVExecComm(const char *SystemName, 
 const char *CommName);

CAENHVLIB_API CAENHVRESULT  CAENHVGetSysPropList(const char *SystemName, 
 ushort *NumProp, char **PropNameList);

CAENHVLIB_API CAENHVRESULT  CAENHVGetSysPropInfo(const char *SystemName, 
 const char *PropName, unsigned *PropMode, unsigned *PropType);

CAENHVLIB_API CAENHVRESULT  CAENHVGetSysProp(const char *SystemName, 
 const char *PropName, void *Result);

/* Rel. 1.2 */
CAENHVLIB_API CAENHVRESULT  CAENHVSetSysProp(const char *SystemName, 
 const char	*PropName, void *Set);

/* Rel. 1.1 */
CAENHVLIB_API CAENHVRESULT CAENHVCaenetComm(const char *SystemName, 
 ushort Crate, ushort Code, ushort NrWCode, ushort *WCode, short *Result,
 ushort *NrOfData, ushort **Data);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __CAENHVWRAPPER_H
