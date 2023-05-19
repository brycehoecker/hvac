/*!
 \class VDCMonComms.h
 \brief demonstration server class for vdcmon communications via CORBA

 \todo add exceptions
       test memory management
       add vdcmon comms

 \author mkd (michael.daniel@ucd.ie) 

 usage: ./server [hostname]

 This program assumes the vhv program is the server.
 When vdcmon has data it sends it to vhv via the sendVDCMonData
 command.

*/

#ifndef VDCMONCOMMS_H
#define VDCMONCOMMS_H

#include <iostream>
#include <vector>
#include <exception.h>
#include <Vcorba.h>
#include "VHVData.h"
#include "VHVControl.h"
#include "VHVSystem.h"

#include "VDCMonComms.hh"

class VDCMonComms_i : public POA_VDCMonComms,
                      public PortableServer::RefCountServantBase
{
  private:
      float fCurrents[499];
      float fT[5];
      float fHumidity;
      int short fPixelStatus[499];

      int short fHVStatus;
      VHVData *fData;
      VHVControl *fControl;

  public:
      VDCMonComms_i( VHVControl *control,
		     VHVData *data ); 
      virtual ~VDCMonComms_i() {}

      virtual void setHVStatus(int short iStatus);
      virtual int  short getHVStatus();
      virtual void resetSerCom();
      virtual void measureOffsets();

      virtual void getPixelStatii(PixelStateArray_out iPixelStates);
      virtual int  short getPixelStatus(int short iPixel);
      virtual void setPixelStatus(int short iPixel, int short iStatus);

      virtual void sendVDCMonData(const VDCMonSummary& iDetails);

      virtual void suppressPixel(int short iPixel);
      virtual void killPixel(int short iPixel);
      virtual void killAll();

      virtual void endNight();

      void quit();
};

#endif
