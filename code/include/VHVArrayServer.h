// VNETfunctionsServerThread.h 
//=======================================================================||
//                                                                       ||
//	Example CORBA server stuff - thread for server to run in	 ||
//									 ||
//                                                                       ||
//      J. Kildea 26 JAN 04                                              ||
//=======================================================================||
/* #include <zthread/Thread.h> */
#ifndef VHVARRAYSERVER_H
#define VHVARRAYSERVER_H

#include "omniORB4/CORBA.h"
#include <zthread/Thread.h>
#include <qobject.h>
#include <qthread.h>

#include "VHVSystem.h"
#include "VHVData.h"
#include "VHVControl.h"
#include "VHV.h"
#include "VHVArrayFunctions.h"
#include "VHVArrayServer.hh"

class VHVArrayServer : public QObject, public QThread
{
  Q_OBJECT
  
    public:
  VHVArrayServer( VHVData *data, VHVControl *control );
  /* 	  throw() {}; */
  /*   	~VNETFunctionsServerThread() throw() {}; */
  ~VHVArrayServer(){};
  
 protected:
  /*   	virtual void run() throw(); */
  virtual void run();
  
 private:
  int argc;
  char **argv;
  CORBA::ORB_var orb;
  VHVData *fData;
  VHVControl *fControl;
};

#endif
