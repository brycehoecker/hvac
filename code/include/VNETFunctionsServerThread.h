// VNETfunctionsServerThread.h 
//=======================================================================||
//                                                                       ||
//	Example CORBA server stuff - thread for server to run in	 ||
//									 ||
//                                                                       ||
//      J. Kildea 26 JAN 04                                              ||
//=======================================================================||
/* #include <zthread/Thread.h> */
#include "omniORB4/CORBA.h"
#include <zthread/Thread.h>
#include <qobject.h>
#include <qthread.h>

#include "VHVSystem.h"
#include "VHVData.h"
#include "VHVControl.h"

class VNETFunctionsServerThread : public QObject, public QThread
{
  Q_OBJECT
  
    public:
  VNETFunctionsServerThread( VHVControl *control, VHVData *data );
  /* 	  throw() {}; */
  /*   	~VNETFunctionsServerThread() throw() {}; */
  ~VNETFunctionsServerThread(){};
  
 protected:
  virtual void run();
  
 private:
  int argc;
  char **argv;
  CORBA::ORB_var orb;
  VHVControl *fControl;
  VHVData *fData;
};

