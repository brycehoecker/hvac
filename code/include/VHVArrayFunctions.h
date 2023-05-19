#ifndef VHVARRAYFUNCTIONS_H
#define VHVARRAYFUNCTIONS_H

#include "CORBA.h"
#include <zthread/Thread.h>
#include <qobject.h>
#include <qthread.h>

#include "VHVSystem.h"
#include "VHVData.h"
#include "VHV.h"
#include "VHVArrayServer.hh"
#include "Vcorba.h"
#include "VHVControl.h"

class VHVArrayFunctions_i : public POA_VHVArrayFunctions, 
			    public PortableServer::RefCountServantBase
{
 public:
  VHVArrayFunctions_i( VHVData *data, VHVControl *control, 
	     Vcorba::corba_server *corbaServer );
  virtual ~VHVArrayFunctions_i();
  virtual void quitVHV();
  virtual void loadProductionHV();
  virtual void saveProductionHV();
/*   virtual void loadHVFile( CORBA::String_var hv_filename ); */
/*   virtual void saveHVFile( CORBA::String_var hv_filename ); */
  virtual void allON();
  virtual void allOFF();
  virtual void pixelON( int short pixel );
  virtual void pixelOFF( int short pixel );
  virtual void pixelSuppress( int short pixel );
  virtual void pixelRestore( int short pixel );
  virtual void pixelSetHV( int short pixel, float voltage );
  virtual void loggingON();
  virtual void loggingOFF();
  virtual void alive();

  VHVData* fData;
  VHVControl* fControl;
  Vcorba::corba_server* fCorbaServer;
};

#endif
