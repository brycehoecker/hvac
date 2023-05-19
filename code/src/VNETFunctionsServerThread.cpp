// VNETfunctionsServerThread.cpp 
//=======================================================================||
//                                                                       ||
//	Example CORBA server stuff - thread for server to run in	 ||
//									 ||
//                                                                       ||
//      J. Kildea 26 JAN 04                                              ||
//=======================================================================||
// include CORBA stuff
#include "VNETFunctionsServerThread.h"
// #include "VNETFunctionsServer.h"
#include "VNETFunctions.hh"

#include <iostream>

using namespace std;

extern int kTelescopeID;

static CORBA::Boolean bindObjectToName(CORBA::ORB_ptr, CORBA::Object_var);

//===================================================================
// The contents of the thread
//===================================================================

// Constructor
VNETFunctionsServerThread::VNETFunctionsServerThread( VHVControl *control, 
						      VHVData *data )  : 
  QObject(0,0)
{
  fControl = control;
  fData = data;
  fNETTransfer = new VNETTransfer_i( fGather, fData ); 
}


VNETFunctionsServerThread::~VNETFunctionsServerThread()
{
  delete fNETTransfer; 
}


// void VNETFunctionsServerThread::run() throw()
void VNETFunctionsServerThread::run()
{
//   cout<<"setting up corba stuff"<<endl;
	//===================================================================
	// Start the CORBA stuff in the thread so that it is not blocking
  	// Init the ORB,Get the RootPOA reference,narrow it, get the nameserver
  	// reference, narrow it
  	// This results in a single-thread model for the server. Each 
  	// client request waits until all previous requests are satisfied(its 
  	// blocked). Ie our server code does not need to be written as thread 
  	// safe!
	//===================================================================
	// ORB initialisation
     	int argcFake=3;
      	char* argvFake[3]={"test","-ORBInitRef","NameService=corbaname::128.135.52.19"};
	orb=CORBA::ORB_init(argcFake,argvFake,"omniORB4");
cout << "ORB_init" << endl;
	//Obtaining the Root POA
    	CORBA::Object_var obj = fOrb->resolve_initial_references("RootPOA");
// cout << "resolve" << endl;
    	PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
cout << "narrow" << endl;

	//Object initialisation - transfer object
      	VNETTransfer_i* NETTransfer = new VNETTransfer_i( fControl, fData ); 
	PortableServer::ObjectId_var NETTransfer_id = poa->activate_object(NETTransfer);
cout << "activate" << endl;

    	// Obtain a reference to the object, and register it in
    	// the naming service.
    	obj = NETTransfer->_this();
cout << "objed" << endl;

	// The error message below seems to be printed out even when things appear to be
	// working - so I'm commenting it out for the moment...
//probably because the if() is missing, have to try it at a later date mkd 8/9/04
	// CORBA::String_var x;
    	// x = orb->object_to_string(obj);
    	// cerr << x << "\n";

   	if( !bindObjectToName(orb, obj) )
   		cerr << "Object not bound" << endl;
	cout << "past bindObjectToName" << endl;
	NETTransfer->_remove_ref();
cout << "remove" << endl;

    	PortableServer::POAManager_var pman = poa->the_POAManager();
    	pman->activate();
cout << "get your motor running" << endl;

    	orb->run();
}

static CORBA::Boolean
bindObjectToName(CORBA::ORB_ptr orb, CORBA::Object_var objref)
{
  char telID[13];
  sprintf( telID, "HighVoltageT%d", kTelescopeID+1 );
//   cout << "bindObjectToName():  telID = " << telID << endl;
  CosNaming::NamingContext_var rootContext;

  try {
    // Obtain a reference to the root context of the Name service:
//     cout<<"inbind"<< endl;
    CORBA::Object_var obj;
    obj = orb->resolve_initial_references("NameService");
//     cout<<"ref obtained"<<endl;
    // Narrow the reference returned.
    rootContext = CosNaming::NamingContext::_narrow(obj);
//     cout<<"rootContext set"<<endl;
    if( CORBA::is_nil(rootContext) ) {
      cerr << "Failed to narrow the root naming context." << endl;
      return 0;
    }
//     cout<<"ref narrowed"<<endl;
  }
  catch(CORBA::ORB::InvalidName& ex) {
    // This should not happen!
    cerr << "Service required is invalid [does not exist]." << endl;
    return 0;
  }

  try {
    // Bind a context called "test" to the root context:

    CosNaming::Name contextName;
    contextName.length(1);
    contextName[0].id   = (const char*) telID;       // string copied
    contextName[0].kind = (const char*) ""; // string copied
    // Note on kind: The kind field is used to indicate the type
    // of the object. This is to avoid conventions such as that used
    // by files (name.type -- e.g. test.ps = postscript etc.)
//     cout<<"context stuff done"<< endl;
    CosNaming::NamingContext_var testContext;
    try {
      // Bind the context to root.
      testContext = rootContext->bind_new_context(contextName);
//       cout<<"bound to root"<<endl;
    }
    catch(CosNaming::NamingContext::AlreadyBound& ex) {
      // If the context already exists, this exception will be raised.
      // In this case, just resolve the name and assign testContext
      // to the object returned:
      CORBA::Object_var obj;
      obj = rootContext->resolve(contextName);
      testContext = CosNaming::NamingContext::_narrow(obj);
      if( CORBA::is_nil(testContext) ) {
        cerr << "Failed to narrow naming context." << endl;
        return 0;
      }
    }

    // Bind objref with name VfNETTransfer to the testContext:
    CosNaming::Name objectName;
    objectName.length(1);
    objectName[0].id   = (const char*) "VNETTransfer";   // string copied
    objectName[0].kind = (const char*) "Object"; // string copied
//     cout<<"context stuff done"<< endl;

    try {
      testContext->bind(objectName, objref);
//       cout<<"bound to testContext"<<endl;
    }
    catch(CosNaming::NamingContext::AlreadyBound& ex) {
      testContext->rebind(objectName, objref);
    }
    // Note: Using rebind() will overwrite any Object previously bound
    //       to /test/VNETMessage with obj.
    //       Alternatively, bind() can be used, which will raise a
    //       CosNaming::NamingContext::AlreadyBound exception if the name
    //       supplied is already bound to an object.

    // Amendment: When using OrbixNames, it is necessary to first try bind
    // and then rebind, as rebind on it's own will throw a NotFoundexception if
    // the Name has not already been bound. [This is incorrect behaviour -
    // it should just bind].
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "Caught system exception COMM_FAILURE -- unable to contact the "
         << "naming service." << endl;
    return 0;
  }
  catch(CORBA::SystemException&) {
    cerr << "Caught a CORBA::SystemException while using the naming service."
  << endl;
    return 0;
  }

  return 1;
}


void VNETFunctionsServerThread::done()
{
  //================================================================
  // finished with CORBA
  //================================================================
  fOrb->shutdown( false );
  fOrb->destroy();
  fOrb = CORBA::ORB::_nil();
  //   fOrb->destroy();
  //   fOrb = CORBA::ORB::_nil();
}


VNETTransfer_i* VNETFunctionsServerThread::getNETTransfer()
{
  return fNETTransfer;
}



