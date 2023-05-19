// VNETClient.cpp 
//=======================================================================||
//                                                                       ||
//	Example CORBA client to do very simple message passing		 ||
//                                                                       ||
//      J. Kildea 26 JAN 04                                              ||
//
// adapted by mkd 8/9/04 for vdcmon/HV comms
//=======================================================================||

// standard includes

// project specific includes
#include "VNETClientThread.h"
//#include "VReadConfig.h"      //for reading the vdcmon config file

// CORBA includes
#include "omniORB4/CORBA.h"

// using namespace std;

extern bool kActiveTelescope[NUMTEL];
extern bool kAutoResponse[NUMTEL];
extern bool kStarRestore[NUMTEL];
extern bool kDBLogging[NUMTEL];

VNETClientThread::VNETClientThread( int telID,
				    VHVData *data ) throw()
{ 
  fTelescopeID = telID;
  fData = data;
  fTelMessColor[0] = string( "brown" );
  fTelMessColor[1] = string( "blue" );
  fTelMessColor[2] = string( "green" );
  fTelMessColor[3] = string( "magenta" );
  begin(); 
}


void VNETClientThread::begin() throw()
{
//   cout << "In VNETClientThread::begin()" << endl;
  //read in the VDCMon config file

  //   VReadConfig VDCMonConfig;
  //   //I shall assume the camdef.txt file is already in this directory
  //   //could add some code to allow it be entered from the command line instead
  //   cout << "Reading config file" << endl;
  //   if((VDCMonConfig.ReadVDCMon("camdef.txt"))!=0) {cerr << "Failed to read camdef.txt" << endl;}

  //allow things to catch up
//   sleep(10);

  //================================================================
  // CORBA INITIALISATION STUFF
  // Initialise the ORB, obtain the name service reference  
  // - timeout after 10 seconds 
  //================================================================
//   cout << "Trying to initialise CORBA..." << endl;
  int argcFake=3;
//   char* argvFake[3]={"test2","-ORBInitRef","NameService=corbaname::128.135.52.19"};  // localhost
  char* argvFake[3]={"test2","-ORBInitRef","NameService=corbaname::db.vts"};
//   cerr << "Faked" << endl;

  CORBA::ORB_var fOrb = CORBA::ORB_init(argcFake, argvFake );
//   cerr << "orb'd" << endl;

  CORBA::Object_var obj = getObjectReference(fOrb);
//   cerr << "obj'd" << endl;
  fNETTransfer_ref = VHVCommunications::VNETTransfer::_narrow(obj);
//   cerr << "narrowed" << endl;
  if( CORBA::is_nil( fNETTransfer_ref ) ) {
    cout << "hvac:  VNETClientThread(): The object reference is " << endl
	 << "\tnil, so this" 
	 << " is not an active telescope." << endl;
  } else {
    try {
      fNETTransfer_ref->alive();
      kActiveTelescope[fTelescopeID] = true;
      cout << "hvac:  VNETClientThread():  Object connected!" << endl;
    }
//     catch(CosNaming::NamingContext::NotFound& ex) {
//       // This exception is thrown if any of the components of the
//       // path [contexts or the object] aren't found:
//       cerr << "Context not found.  Not an active telescope." << endl;
//     }
//     catch(CORBA::COMM_FAILURE& ex) {
//       cerr << "Caught system exception COMM_FAILURE -- unable to contact the "
// 	   << "naming service." << endl;
//     }
    catch(CORBA::SystemException &ex) {
      cerr << "\tCaught a CORBA::SystemException while using the naming" 
	   << "\n\tservice" << ":    " << ex.NP_minorString() 
	   << "\n\tMust not be an active telescope."
	   << endl;
    }
  }
}


//================================================================
// CORBA get object reference fuction - taken from the omniORB manual
//================================================================
CORBA::Object_ptr VNETClientThread::getObjectReference( CORBA::ORB_ptr orb )
{
  char telID[13];
  sprintf( telID, "HighVoltageT%d", fTelescopeID+1 );
  cout << "hvac:  VNETClientThread::getObjectReference(): " << telID << endl;
  CosNaming::NamingContext_var rootContext;
  
  try {
    try {
      // Obtain a reference to the root context of the Name service:
      CORBA::Object_var obj;
      obj = orb->resolve_initial_references("NameService");

      // Narrow the reference returned.
      rootContext = CosNaming::NamingContext::_narrow(obj);
      if( CORBA::is_nil(rootContext) ) {
	cerr << "Failed to narrow the root naming context." << endl;
	return CORBA::Object::_nil();
      }
    }
    catch(CORBA::ORB::InvalidName& ex) {
      // This should not happen!
      cerr << "Service required is invalid [does not exist]." << endl;
      return CORBA::Object::_nil();
    }
    catch(CosNaming::NamingContext::NotFound& ex) {
      // This exception is thrown if any of the components of the
      // path [contexts or the object] aren't found:
      cerr << "\tContext not found. Not an active telescope." << endl;
    }
    catch(CORBA::COMM_FAILURE& ex) {
      cerr << "Caught system exception COMM_FAILURE -- unable to contact the "
	   << "naming service." << endl;
    }
  }
  catch(CORBA::SystemException &ex) {
    cerr << "Caught a CORBA::SystemException while using the naming service."
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
  catch( exception &any ) {
    cout << "Exception caught:  " << any.what() << endl;
  }

  // Create a name object, containing the name test/context:
  CosNaming::Name name;
  name.length(2);

  name[0].id   = (const char*) telID;       // string copied
  name[0].kind = (const char*) ""; // string copied
  name[1].id   = (const char*) "VNETTransfer";
  name[1].kind = (const char*) "Object";
  // Note on kind: The kind field is used to indicate the type
  // of the object. This is to avoid conventions such as that used
  // by files (name.type -- e.g. test.ps = postscript etc.)

  try {
    // Resolve the name to an object reference.
    return rootContext->resolve(name);
  }
  catch(CosNaming::NamingContext::NotFound& ex) {
    // This exception is thrown if any of the components of the
    // path [contexts or the object] aren't found:
    cerr << "\tContext not found." << endl;
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "Caught system exception COMM_FAILURE -- unable to contact the "
         << "naming service." << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "Caught a CORBA::SystemException while using the naming service."
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }

  return CORBA::Object::_nil();
}


void VNETClientThread::done()
{
  //================================================================
  // finished with CORBA
  //================================================================
  fOrb->shutdown( true );
  fOrb->destroy();

}


void VNETClientThread::pixelSetVReq( short int pixel, float voltage )
{
//   cout << "VNETClientThread::pixelSetVReq()" << endl;
  try {
    fNETTransfer_ref->pixelSetVReq( pixel, voltage );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelSetVReq() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelSetVReq() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelGetVSet( short int pixel )
{
//   cout << "VNETClientThread::pixelGetVSet()" << endl;
  try {
    fData->setVSet( pixel, (int)fNETTransfer_ref->pixelGetVSet( pixel ) );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelGetVSet() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelGetVSet() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelGetVReq( short int pixel )
{
//   cout << "VNETClientThread::pixelGetVReq()" << endl;
  try {
    fData->setVReq( pixel, (int)fNETTransfer_ref->pixelGetVReq( pixel ) );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelGetVReq() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelGetVReq() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelGetVTrue( short int pixel )
{
//   cout << "VNETClientThread::pixelGetVTrue()" << endl;
  try {
    fData->setVTrue( pixel, (int)fNETTransfer_ref->pixelGetVTrue( pixel ) );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelGetVTrue() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelGetVTrue() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allSetHV( float voltage )
{
//   cout << "VNETClientThread::allSetHV()" << endl;
  try {
    fNETTransfer_ref->allSetHV( voltage );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allSetHV() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allSetHV() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allGetVSet()
{
//   cout << "VNETClientThread::allGetVSet()" << endl;
  try {
    VHVCommunications::VHVFloatSeq_var vset = 
      new VHVCommunications::VHVFloatSeq( MAX_PMTS );
    vset->length( MAX_PMTS );
    vset = (VHVCommunications::VHVFloatSeq_var)fNETTransfer_ref->allGetVSet();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      //       cout << "Pix " << i+1 << ":  " << vset[i] << " V" << endl;
      fData->setVSet( i, (int)vset[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allGetVSet() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allGetVSet() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allGetVReq()
{
//   cout << "VNETClientThread::allGetVReq()" << endl;
  try {
    VHVCommunications::VHVFloatSeq_var vset = 
      new VHVCommunications::VHVFloatSeq( MAX_PMTS );
    vset->length( MAX_PMTS );
    vset = (VHVCommunications::VHVFloatSeq_var)fNETTransfer_ref->allGetVReq();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      //       cout << "Pix " << i+1 << ":  " << vset[i] << " V" << endl;
      fData->setVReq( i, (int)vset[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allGetVReq() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allGetVReq() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allGetVTrue()
{
//   cout << "VNETClientThread::allGetVTrue()" << endl;
  try {
    VHVCommunications::VHVFloatSeq_var vtrue = 
      new VHVCommunications::VHVFloatSeq( MAX_PMTS );
    vtrue->length( MAX_PMTS );
    vtrue = (VHVCommunications::VHVFloatSeq_var)fNETTransfer_ref->allGetVTrue();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      //       cout << "Pix " << i+1 << ":  " << vtrue[i] << " V" << endl;
      fData->setVTrue( i, (int)vtrue[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allGetVTrue() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allGetVTrue() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allOffsetHV( float voltage )
{
//   cout << "VNETClientThread::allOffsetHV()" << endl;
  try {
    fNETTransfer_ref->allOffsetHV( voltage );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allOffsetHV() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allOffsetHV() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allScaleHV( float percent )
{
//   cout << "VNETClientThread::allScaleHV()" << endl;
  try {
    fNETTransfer_ref->allScaleHV( percent );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allScaleHV() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allScaleHV() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::writeVoltagesToDB()
{
//   cout << "VNETClientThread::writeVoltagesToDB()" << endl;
  try {
    fNETTransfer_ref->writeVoltagesToDB();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::writeVoltagesToDB() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::writeVoltagesToDB() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::readVoltagesFromDB()
{
//   cout << "VNETClientThread::readVoltagesFromDB()" << endl;
  try {
    fNETTransfer_ref->readVoltagesFromDB();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::readVoltagesFromDB() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::readVoltagesFromDB() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


//   void writeHVFile( in string filename ) // syntax!
//   void readHVFile( in string filename )  // syntax!

void VNETClientThread::allOn()
{  
  try {
    fNETTransfer_ref->allOn();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allOn() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allOn() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allOff()
{  
  try {
    fNETTransfer_ref->allOff();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allOff() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allOff() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allGetPowerStatus()
{
  try {
    VHVCommunications::VHVBoolSeq_var power =
      new VHVCommunications::VHVBoolSeq( MAX_PMTS );
    power->length( MAX_PMTS );
    power = (VHVCommunications::VHVBoolSeq_var)
      fNETTransfer_ref->allGetPowerStatus();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->setPower( i, power[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allGetPowerStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allGetPowerStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelGetPowerStatus( short int pixel )
{
  try {
    fData->setPower( pixel,
		     fNETTransfer_ref->pixelGetPowerStatus( pixel ) );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelGetPowerStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelGetPowerStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelOn( short int pixel )
{
//   cout << "VNETClientThread::pixelOn()" << endl;
  try {
    fNETTransfer_ref->pixelOn( pixel );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelOn() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelOn() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelOff( short int pixel )
{
//   cout << "VNETClientThread::pixelOff()" << endl;
  try {
    fNETTransfer_ref->pixelOff( pixel );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelOff() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelOff() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelSuppress( short int pixel )
{
//   cout << "VNETClientThread::pixelSuppress()" << endl;
  try {
    fNETTransfer_ref->pixelSuppress( pixel );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelSuppress() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelSuppress() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelRestore( short int pixel )
{
//   cout << "VNETClientThread::pixelRestore()" << endl;
  try {
    fNETTransfer_ref->pixelRestore( pixel );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelRestore() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelRestore() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allSuppress()
{
//   cout << "VNETClientThread::allSuppress()" << endl;
  try {
    fNETTransfer_ref->allSuppress();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allSuppress() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allSuppress() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allRestore()
{
//   cout << "VNETClientThread::allRestore()" << endl;
  try {
    fNETTransfer_ref->allRestore();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allRestore() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allRestore() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::restoreRecent()
{
//   cout << "VNETClientThread::restoreRecent()" << endl;
  try {
    fNETTransfer_ref->restoreRecent();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::restoreRecent() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::restoreRecent() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelGetSuppressionStatus( short int pixel )
{
  try {
    fData->initSuppressed( pixel,
			   fNETTransfer_ref->
			   pixelGetSuppressionStatus( pixel ) );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelGetSuppressionStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelGetSuppressionStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allGetSuppressionStatus()
{
  try {
    VHVCommunications::VHVBoolSeq_var status =
      new VHVCommunications::VHVBoolSeq( MAX_PMTS );
    status->length( MAX_PMTS );
    status = (VHVCommunications::VHVBoolSeq_var)
      fNETTransfer_ref->allGetSuppressionStatus();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->initSuppressed( i, status[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allGetSuppressionStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allGetSuppressionStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::loggingOn()
{
//   cout << "VNETClientThread::loggingOn()" << endl;
  try {
    fNETTransfer_ref->loggingOn();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::loggingOn() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::loggingOn() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::loggingOff()
{
//   cout << "VNETClientThread::loggingOff()" << endl;
  try {
    fNETTransfer_ref->loggingOff();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::loggingOff() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::loggingOff() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}

/// Method to check the state of logging to the database.
void VNETClientThread::getLoggingState()
{
  try {
    kDBLogging[fTelescopeID] = fNETTransfer_ref->getLoggingState();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getLoggingState() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getLoggingState() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}
/// Method to check whether connection is alive.  Need to use exceptions more
/// intelligently!
void VNETClientThread::alive()
{
//   cout << "VNETClientThread::alive()" << endl;
  try {
    fNETTransfer_ref->alive();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::alive() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
//   catch(CORBA::SystemException &ex) {
//     cerr << "VNETClientThread::alive() caught a CORBA::SystemException while "
// 	 << " attempting to contact Tel " << fTelescopeID
// 	 << ":  " << ex.NP_minorString() << "  "
// 	 << endl;
//   }
}


void VNETClientThread::quitVHV()
{
  try {
    fNETTransfer_ref->quitVHV();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::quitVHV() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::quitVHV() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::connectToHVAC()
{
//   cout << "VNETClientThread::connectToHVAC()" << endl;
  try {
    fNETTransfer_ref->connectToHVAC();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::connectToHVAC() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::connectToHVAC() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::disconnectFromHVAC()
{
//   cout << "VNETClientThread::disconnectFromHVAC()" << endl;
  try {
    fNETTransfer_ref->disconnectFromHVAC();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::disconnectFromHVAC() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::disconnectFromHVAC() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::markPixelDisabled( short int pixel )
{
//   cout << "VNETClientThread::markPixelDisabled()" << endl;
  try {
    fNETTransfer_ref->markPixelDisabled( pixel );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::markPixelDisabled() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::markPixelDisabled() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::unmarkPixelDisabled( short int pixel )
{
//   cout << "VNETClientThread::unmarkPixelDisabled()" << endl;
  try {
    fNETTransfer_ref->unmarkPixelDisabled( pixel );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::unmarkPixelDisabled() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::unmarkPixelDisabled() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelGetDisabledStatus( short int pixel )
{
  try {
    fData->setDisabled( pixel,
		      fNETTransfer_ref->pixelGetDisabledStatus( pixel ) );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelGetDisabledStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelGetDisabledStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allGetDisabledStatus()
{
  try {
    VHVCommunications::VHVBoolSeq_var disabled = 
      new VHVCommunications::VHVBoolSeq( MAX_PMTS );
    disabled->length( MAX_PMTS );
    disabled = (VHVCommunications::VHVBoolSeq_var)
      fNETTransfer_ref->allGetDisabledStatus();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->setDisabled( i, (int)disabled[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allGetDisabledStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allGetDisabledStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::markPixelNoAuto( short int pixel )
{
// //   cout << "VNETClientThread::markPixelNoAuto()" << endl;
  try {
    fNETTransfer_ref->markPixelNoAuto( pixel );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::markPixelNoAuto() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::markPixelNoAuto() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::unmarkPixelNoAuto( short int pixel )
{
//   cout << "VNETClientThread::unmarkPixelNoAuto()" << endl;
  try {
    fNETTransfer_ref->unmarkPixelNoAuto( pixel );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::unmarkPixelNoAuto() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::unmarkPixelNoAuto() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelGetAutoStatus( short int pixel )
{
  try {
    fNETTransfer_ref->pixelGetAutoStatus( pixel );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelGetAutoStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelGetAutoStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::allGetAutoStatus()
{
  try {
    VHVCommunications::VHVBoolSeq_var status = 
      new VHVCommunications::VHVBoolSeq( MAX_PMTS );
    status->length( MAX_PMTS );
    status = (VHVCommunications::VHVBoolSeq_var)
      fNETTransfer_ref->allGetAutoStatus();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->setAutoStatus( i, (int)status[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::allGetAutoStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::allGetAutoStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setAutoResponseParams()
{
  try {
//     VHVCommunications::autoParams_var params =
//       new VHVCommunications::autoParams();
//     params->autoEnable = kAutoResponse;
//     params->autoRestoreEnable = kStarRestore;
//     params->threshold = fData->getThreshold();
//     params->hiThreshold = fData->getHiThreshold();
//     params->patternThreshold = fData->getPatternThreshold();
//     params->patternMultiplicity = fData->getPatternMultiplicity();
//     params->restoreMargin = fData->getRestoreMargin();
//     params->patternRestoreMargin = fData->getPatternRestoreMargin();
//     params->checkTime = fData->getCheckTime();
//     params->recentTime = fData->getRecentTime();
    VHVCommunications::autoParams params;
    params.autoEnable = kAutoResponse[fTelescopeID];
    params.autoRestoreEnable = kStarRestore[fTelescopeID];
    params.threshold = fData->getThreshold();
    params.hiThreshold = fData->getHiThreshold();
    params.patternThreshold = fData->getPatternThreshold();
    params.patternMultiplicity = fData->getPatternMultiplicity();
    params.restoreMargin = fData->getRestoreMargin();
    params.patternRestoreMargin = fData->getPatternRestoreMargin();
    params.checkTime = fData->getCheckTime();
    params.recentTime = fData->getRecentTime();
    fNETTransfer_ref->setAutoResponseParams( params );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setAutoResponseParams() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setAutoResponseParams() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getAutoResponseParams()
{
  try {
    VHVCommunications::autoParams params = 
      fNETTransfer_ref->getAutoResponseParams();
    kAutoResponse[fTelescopeID] = params.autoEnable;
    kStarRestore[fTelescopeID] = params.autoRestoreEnable;
    fData->setThreshold( params.threshold );
    fData->setHiThreshold( params.hiThreshold );
    fData->setPatternThreshold( params.patternThreshold );
    fData->setPatternMultiplicity( params.patternMultiplicity );
    fData->setRestoreMargin( params.restoreMargin );
    fData->setPatternRestoreMargin( params.patternRestoreMargin );
    fData->setCheckTime( params.checkTime );
    fData->setRecentTime( params.recentTime );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getAutoResponseParams() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getAutoResponseParams() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


  void VNETClientThread::setFeedbackParams()
{
  try {
    VHVCommunications::fdbkParams params;
    params.fThreshold = fData->getThreshold();
    params.fHiThreshold = fData->getHiThreshold();
    params.fPatternThreshold = fData->getPatternThreshold();
    params.fPatternMultiplicity = fData->getPatternMultiplicity();
    params.fRestoreMargin = fData->getRestoreMargin();
    params.fMeasurementsPerCycle = fData->getMeasurementsPerCycle();
    params.fMinTimeBeforeRestore = fData->getMinTimeBeforeRestore();
    params.fNumberOfFeedbackLevels = fData->getNumberOfFeedbackLevels();
    params.fGainScaling = fData->getGainScaling();
    params.fSafetyFactor = fData->getSafetyFactor();
    params.fRestoreAttemptTime = fData->getRestoreAttemptTime();
    params.fMinAbsPositionChange = fData->getMinAbsPositionChange();
    params.fPatternDistanceDrop = fData->getPatternDistanceDrop();
    params.fPSF = fData->getPSF();
    params.fCyclesPerFeedbackTest = fData->getCyclesPerFeedbackTest();
    fNETTransfer_ref->setFeedbackParams( params );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setFeedbackParams() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setFeedbackParams() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getFeedbackParams()
{
  try {
    VHVCommunications::fdbkParams params = 
      fNETTransfer_ref->getFeedbackParams();
    fData->setThreshold( params.fThreshold );
    fData->setHiThreshold( params.fHiThreshold );
    fData->setPatternThreshold( params.fPatternThreshold );
    fData->setPatternMultiplicity( params.fPatternMultiplicity );
    fData->setRestoreMargin( params.fRestoreMargin );
    fData->setMeasurementsPerCycle( params.fMeasurementsPerCycle );
    fData->setMinTimeBeforeRestore( params.fMinTimeBeforeRestore );
    fData->setNumberOfFeedbackLevels( params.fNumberOfFeedbackLevels );
    fData->setGainScaling( params.fGainScaling );
    fData->setSafetyFactor( params.fSafetyFactor );
    fData->setRestoreAttemptTime( params.fRestoreAttemptTime );
    fData->setMinAbsPositionChange( params.fMinAbsPositionChange );
    fData->setPatternDistanceDrop( params.fPatternDistanceDrop );
    fData->setPSF( params.fPSF );
    fData->setCyclesPerFeedbackTest( params.fCyclesPerFeedbackTest );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getFeedbackParams() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getFeedbackParams() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setTelescopeTarget( short int targetIndex )
{
//   cout << "VNETClientThread::setTelescopeTarget()" << endl;
  try {
    fNETTransfer_ref->setTelescopeTarget( targetIndex );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setTelescopeTarget() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setTelescopeTarget() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setRA( float ra )
{
//   cout << "VNETClientThread::setRA()" << endl;
  try {
    fNETTransfer_ref->setRA( ra );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setRA() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setRA() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setDec( float dec )
{
//   cout << "VNETClientThread::setDec()" << endl;
  try {
    fNETTransfer_ref->setDec( dec );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setDec() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setDec() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::takeStarControl()
{
  try{
    fNETTransfer_ref->takeStarControl();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::takeStarControl() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::takeStarControl() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::releaseStarControl()
{
  try{
    fNETTransfer_ref->releaseStarControl();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::releaseStarControl() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::releaseStarControl() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getNumberOfPMTs()
{
  try {
    fData->setNumberOfPMTs( fNETTransfer_ref->getNumberOfPMTs() );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getNumberOfPMTs() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getNumberOfPMTs() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getNumberOfBoards()
{
  try {
    fData->setNumberOfBoards( fNETTransfer_ref->getNumberOfBoards() );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getNumberOfBoards() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getNumberOfBoards() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getNumberOfCrates()
{
  try {
    fData->setNumberOfCrates( fNETTransfer_ref->getNumberOfCrates() );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getNumberOfCrates() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getNumberOfCrates() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getCrateStatus( short int crate )
{
  try {
    fData->setCrateStatus( crate, fNETTransfer_ref->getCrateStatus( crate ) );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getCrateStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getCrateStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getHVCrate()
{
  try {
    VHVCommunications::VHVShortSeq_var crate = 
      new VHVCommunications::VHVShortSeq( MAX_PMTS );
    crate->length( MAX_PMTS );
    crate = (VHVCommunications::VHVShortSeq_var)
      fNETTransfer_ref->getHVCrate();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->setCrate( i, crate[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getHVCrate() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getHVCrate() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getHVSlot()
{
  try {
    VHVCommunications::VHVShortSeq_var slot = 
      new VHVCommunications::VHVShortSeq( MAX_PMTS );
    slot->length( MAX_PMTS );
    slot = (VHVCommunications::VHVShortSeq_var)
      fNETTransfer_ref->getHVSlot();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->setSlot( i, slot[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getHVSlot() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getHVSlot() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getHVChannel()
{
  try {
    VHVCommunications::VHVShortSeq_var channel = 
      new VHVCommunications::VHVShortSeq( MAX_PMTS );
    channel->length( MAX_PMTS );
    channel = (VHVCommunications::VHVShortSeq_var)
      fNETTransfer_ref->getHVChannel();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->setChannel( i, channel[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getHVChannel() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getHVChannel() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getStatus()
{
  try {
    VHVCommunications::VHVShortSeq_var status = 
      new VHVCommunications::VHVShortSeq( MAX_PMTS );
    status->length( MAX_PMTS );
    status = (VHVCommunications::VHVShortSeq_var)
      fNETTransfer_ref->getStatus();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->setStatus( i, status[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getCurrent()
{
  try {
    VHVCommunications::VHVFloatSeq_var current = 
      new VHVCommunications::VHVFloatSeq( MAX_PMTS );
    current->length( MAX_PMTS );
    current = (VHVCommunications::VHVFloatSeq_var)
      fNETTransfer_ref->getCurrent();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->setCurrent( i, current[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getCurrent() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getCurrent() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardCrate()
{
  try {
    VHVCommunications::VHVShortSeq_var crate = 
      new VHVCommunications::VHVShortSeq( MAX_BOARDS );
    crate->length( MAX_BOARDS );
    crate = (VHVCommunications::VHVShortSeq_var)
      fNETTransfer_ref->getBoardCrate();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardCrate( i, crate[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardCrate() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardCrate() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardSlot()
{
  try {
    VHVCommunications::VHVShortSeq_var slot = 
      new VHVCommunications::VHVShortSeq( MAX_BOARDS );
    slot->length( MAX_BOARDS );
    slot = (VHVCommunications::VHVShortSeq_var)
      fNETTransfer_ref->getBoardSlot();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardSlot( i, slot[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardSlot() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardSlot() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardVSet()
{
  try {
    VHVCommunications::VHVFloatSeq_var vset = 
      new VHVCommunications::VHVFloatSeq( MAX_BOARDS );
    vset->length( MAX_BOARDS );
    vset = (VHVCommunications::VHVFloatSeq_var)
      fNETTransfer_ref->getBoardVSet();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardVSet( i, vset[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardVSet() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardVSet() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardVReq()
{
  try {
    VHVCommunications::VHVFloatSeq_var vset = 
      new VHVCommunications::VHVFloatSeq( MAX_BOARDS );
    vset->length( MAX_BOARDS );
    vset = (VHVCommunications::VHVFloatSeq_var)
      fNETTransfer_ref->getBoardVReq();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardVReq( i, vset[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardVReq() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardVReq() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setBoardVReq( short int board, float voltage )
{
  try {
    fNETTransfer_ref->setBoardVReq( board, voltage );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setBoardVReq() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setBoardVReq() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardVTrue()
{
  try {
    VHVCommunications::VHVFloatSeq_var vtrue = 
      new VHVCommunications::VHVFloatSeq( MAX_BOARDS );
    vtrue->length( MAX_BOARDS );
    vtrue = (VHVCommunications::VHVFloatSeq_var)
      fNETTransfer_ref->getBoardVTrue();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardVTrue( i, vtrue[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardVTrue() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardVTrue() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardVMax()
{
  try {
    VHVCommunications::VHVFloatSeq_var vmax = 
      new VHVCommunications::VHVFloatSeq( MAX_BOARDS );
    vmax->length( MAX_BOARDS );
    vmax = (VHVCommunications::VHVFloatSeq_var)
      fNETTransfer_ref->getBoardVMax();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardVMax( i, vmax[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardVMax() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardVMax() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardPower()
{
  try {
    VHVCommunications::VHVBoolSeq_var power = 
      new VHVCommunications::VHVBoolSeq( MAX_BOARDS );
    power->length( MAX_BOARDS );
    power = (VHVCommunications::VHVBoolSeq_var)
      fNETTransfer_ref->getBoardPower();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardPower( i, power[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardPower() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardPower() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setBoardPower( short int board, bool powerState )
{
  try {
    fNETTransfer_ref->setBoardPower( board, powerState );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setBoardPower() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setBoardPower() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setAllBoardPower( bool powerState )
{
  try {
    fNETTransfer_ref->setAllBoardPower( powerState );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setAllBoardPower() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setAllBoardPower() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getHWStatus()
{
  try {
    VHVCommunications::VHVUShortSeq_var ramp = 
      new VHVCommunications::VHVUShortSeq( MAX_PMTS );
    ramp->length( MAX_PMTS );
    ramp = (VHVCommunications::VHVUShortSeq_var)
      fNETTransfer_ref->getHWStatus();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->setHWStatus( i, ramp[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getHWStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getHWStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardRampUp()
{
  try {
    VHVCommunications::VHVShortSeq_var ramp = 
      new VHVCommunications::VHVShortSeq( MAX_BOARDS );
    ramp->length( MAX_BOARDS );
    ramp = (VHVCommunications::VHVShortSeq_var)
      fNETTransfer_ref->getBoardRampUp();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardRampUp( i, ramp[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardRampUp() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardRampUp() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardRampDown()
{
  try {
    VHVCommunications::VHVShortSeq_var ramp = 
      new VHVCommunications::VHVShortSeq( MAX_BOARDS );
    ramp->length( MAX_BOARDS );
    ramp = (VHVCommunications::VHVShortSeq_var)
      fNETTransfer_ref->getBoardRampDown();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardRampDown( i, ramp[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardRampDown() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardRampDown() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setBoardRampUp( short int board, short int ramp )
{
  try {
    fNETTransfer_ref->setBoardRampUp( board, ramp );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setBoardRampUp() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setBoardRampUp() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setAllBoardRampUp( short int ramp )
{
  try {
    fNETTransfer_ref->setAllBoardRampUp( ramp );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setAllBoardRampUp() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setAllBoardRampUp() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setBoardRampDown( short int board, short int ramp )
{
  try {
    fNETTransfer_ref->setBoardRampDown( board, ramp );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setBoardRampDown() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setBoardRampDown() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setAllBoardRampDown( short int ramp )
{
  try {
    fNETTransfer_ref->setAllBoardRampDown( ramp );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setAllBoardRampDown() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setAllBoardRampDown() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardIMax()
{
  try {
    VHVCommunications::VHVFloatSeq_var iMax = 
      new VHVCommunications::VHVFloatSeq( MAX_BOARDS );
    iMax->length( MAX_BOARDS );
    iMax = (VHVCommunications::VHVFloatSeq_var)
      fNETTransfer_ref->getBoardIMax();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardIMax( i, iMax[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardIMax() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardIMax() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setBoardIMax( short int board, float iMax )
{
  try {
    fNETTransfer_ref->setBoardIMax( board, iMax );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setBoardIMax() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setBoardIMax() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setAllBoardIMax( float iMax )
{
  try {
    fNETTransfer_ref->setAllBoardIMax( iMax );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setAllBoardIMax() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setAllBoardIMax() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardCurrent()
{
  try {
    VHVCommunications::VHVFloatSeq_var current = 
      new VHVCommunications::VHVFloatSeq( MAX_BOARDS );
    current->length( MAX_BOARDS );
    current = (VHVCommunications::VHVFloatSeq_var)
      fNETTransfer_ref->getBoardCurrent();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardCurrent( i, current[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardCurrent() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardCurrent() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardTemp()
{
  try {
    VHVCommunications::VHVFloatSeq_var temp = 
      new VHVCommunications::VHVFloatSeq( MAX_BOARDS );
    temp->length( MAX_BOARDS );
    temp = (VHVCommunications::VHVFloatSeq_var)
      fNETTransfer_ref->getBoardTemp();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardTemp( i, temp[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardTemp() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardTemp() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getBoardActive()
{
  try {
    VHVCommunications::VHVBoolSeq_var active = 
      new VHVCommunications::VHVBoolSeq( MAX_BOARDS );
    active->length( MAX_BOARDS );
    active = (VHVCommunications::VHVBoolSeq_var)
      fNETTransfer_ref->getBoardActive();
    for ( int i=0; i<MAX_BOARDS; ++i ) {
      fData->setBoardActive( i, active[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getBoardActive() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getBoardActive() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setBoardActive( short int board, bool active )
{
  try {
    fNETTransfer_ref->setBoardActive( board, active );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setBoardActive() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setBoardActive() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::setAllBoardActive( bool active )
{
  try {
    fNETTransfer_ref->setAllBoardActive( active );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::setAllBoardActive() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::setAllBoardActive() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getCurrentsStatus()
{
  try {
    fData->setCurrentsStatus( fNETTransfer_ref->getCurrentsStatus() );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getCurrentsStatus() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getCurrentsStatus() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getCameraTemp()
{
  try {
    fData->setCameraTemp( fNETTransfer_ref->getCameraTemp() );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getCameraTemp() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getCameraTemp() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getCameraHum()
{
  try {
    fData->setCameraHum( fNETTransfer_ref->getCameraHum() );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getCameraHum() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getCameraHum() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getSensor( short int id )
{
  try {
    fData->setSensor( id, fNETTransfer_ref->getSensor( id ) );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getSensor() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getSensor() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getMessages()
{
  try {
    VHVCommunications::VHVStringSeq_var messages = 
      fNETTransfer_ref->getMessages();
    int length = messages->length();
    char tmp[80];
    sprintf( tmp, "T%d:  ", fTelescopeID+1 );
    string front = string( "<font color=\"" ) + fTelMessColor[fTelescopeID] + string( "\">" );
//     cout << "getMessages(): received " << length << " messages." << endl;
    for ( int i=0; i<length; ++i ) {
      emit printMessage( QString( front + string( tmp ) + 
				  messages[i] +
				  string( "</font><br>" ) ) );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getMessages() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getMessages() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::globalFeedbackReset()
{
  try {
    fNETTransfer_ref->globalFeedbackReset();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::globalFeedbackReset() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::globalFeedbackReset() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelFeedbackReset( short int ch )
{
  try {
    fNETTransfer_ref->pixelFeedbackReset( ch );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelFeedbackReset() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelFeedbackReset() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::globalFeedbackOn()
{
  try {
    fNETTransfer_ref->globalFeedbackOn();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::globalFeedbackOn() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelFeedbackOn( short int ch )
{
  try {
    fNETTransfer_ref->pixelFeedbackOn( ch );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::globalFeedbackOn() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::globalFeedbackOff()
{
  try {
    fNETTransfer_ref->globalFeedbackOff();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::globalFeedbackOff() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::globalFeedbackOff() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::pixelFeedbackOff( short int ch )
{
  try {
    fNETTransfer_ref->pixelFeedbackOff( ch );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::pixelFeedbackOff() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::pixelFeedbackOff() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::globalRefreshVoltageLevels()
{
  try {
    fNETTransfer_ref->globalRefreshVoltageLevels();
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::globalRefreshVoltageLevels() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::globalRefreshVoltageLevels() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::isGlobalFeedbackEnabled()
{
  try {
    fData->setFeedbackEnabled( fNETTransfer_ref->isGlobalFeedbackEnabled() );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::isGlobalFeedbackEnabled() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::isGlobalFeedbackEnabled() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::isPixelFeedbackEnabled( short int ch )
{
  try {
    fData->setPixelFeedbackEnabled( ch, fNETTransfer_ref->
				    isPixelFeedbackEnabled( ch ) );
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::isPixelFeedbackEnabled() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::isPixelFeedbackEnabled() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::isAllPixelFeedbackEnabled()
{
  try {
    VHVCommunications::VHVBoolSeq_var en = 
      new VHVCommunications::VHVBoolSeq( MAX_PMTS );
    en->length( MAX_PMTS );
    en = (VHVCommunications::VHVBoolSeq_var)
      fNETTransfer_ref->isAllPixelFeedbackEnabled();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->setPixelFeedbackEnabled( i, en[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::isAllPixelFeedbackEnabled() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::isAllPixelFeedbackEnabled() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getFeedbackLevel()
{
  try {
    VHVCommunications::VHVShortSeq_var level = 
      new VHVCommunications::VHVShortSeq( MAX_PMTS );
    level->length( MAX_PMTS );
    level = (VHVCommunications::VHVShortSeq_var)
      fNETTransfer_ref->getFeedbackLevel();
    for ( int i=0; i<MAX_PMTS; ++i ) {
      fData->setFeedbackLevel( i, level[i] );
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    cerr << "VNETClientThread::getFeedbackLevel() "
	 << "caught system exception COMM_FAILURE -- unable to contact "
         << "Tel " << fTelescopeID+1 << endl;
  }
  catch(CORBA::SystemException &ex) {
    cerr << "VNETClientThread::getFeedbackLevel() caught a CORBA::SystemException while "
	 << " attempting to contact Tel " << fTelescopeID+1
	 << ":  " << ex.NP_minorString() << "  "
	 << endl;
  }
}


void VNETClientThread::getStatusPeriod()
{
  try
    {
      fData->setStatusPeriod( fNETTransfer_ref->getStatusPeriod() );
    }
  catch(CORBA::COMM_FAILURE& ex)
    {
      cerr << "VNETClientThread::getFeedbackLevel() "
           << "caught system exception COMM_FAILURE -- unable to contact "
           << "Tel " << fTelescopeID+1 << endl;
    }
  catch(CORBA::SystemException &ex)
    {
      cerr << "VNETClientThread::getFeedbackLevel() caught a CORBA::SystemException while "
           << " attempting to contact Tel " << fTelescopeID+1
           << ":  " << ex.NP_minorString() << "  "
           << endl;
    }
}


void VNETClientThread::setStatusPeriod()
{
  try
    {
      fNETTransfer_ref->setStatusPeriod( fData->getStatusPeriod() );
    }
  catch(CORBA::COMM_FAILURE& ex)
    {
      cerr << "VNETClientThread::setStatusPeriod() "
           << "caught system exception COMM_FAILURE -- unable to contact "
           << "Tel " << fTelescopeID+1 << endl;
    }
  catch(CORBA::SystemException &ex)
    {
      cerr << "VNETClientThread::setStatusPeriod() caught a CORBA::SystemException while "
           << " attempting to contact Tel " << fTelescopeID+1
           << ":  " << ex.NP_minorString() << "  "
           << endl;
    }
}


bool VNETClientThread::getStateLimitBoardVoltages()
{  
  try
    {
      bool state = fNETTransfer_ref->getStateLimitBoardVoltages();
      if ( state ) fData->enableLimitBoardVoltages();
      else fData->disableLimitBoardVoltages();
    }
  catch(CORBA::COMM_FAILURE& ex)
    {
      cerr << "VNETClientThread::getStateLimitBoardVoltages() "
           << "caught system exception COMM_FAILURE -- unable to contact "
           << "Tel " << fTelescopeID+1 << endl;
    }
  catch(CORBA::SystemException &ex)
    {
      cerr << "VNETClientThread::getStateLimitBoardVoltages() caught a CORBA::SystemException while "
           << " attempting to contact Tel " << fTelescopeID+1
           << ":  " << ex.NP_minorString() << "  "
           << endl;
    }
}


void VNETClientThread::enableLimitBoardVoltages()
{
  try
    {
      fNETTransfer_ref->enableLimitBoardVoltages();
    }
  catch(CORBA::COMM_FAILURE& ex)
    {
      cerr << "VNETClientThread::enableLimitBoardVoltages() "
           << "caught system exception COMM_FAILURE -- unable to contact "
           << "Tel " << fTelescopeID+1 << endl;
    }
  catch(CORBA::SystemException &ex)
    {
      cerr << "VNETClientThread::enableLimitBoardVoltages() caught a CORBA::SystemException while "
           << " attempting to contact Tel " << fTelescopeID+1
           << ":  " << ex.NP_minorString() << "  "
           << endl;
    }
}


void VNETClientThread::disableLimitBoardVoltages()
{
  try
    {
      fNETTransfer_ref->disableLimitBoardVoltages();
    }
  catch(CORBA::COMM_FAILURE& ex)
    {
      cerr << "VNETClientThread::disableLimitBoardVoltages() "
           << "caught system exception COMM_FAILURE -- unable to contact "
           << "Tel " << fTelescopeID+1 << endl;
    }
  catch(CORBA::SystemException &ex)
    {
      cerr << "VNETClientThread::disableLimitBoardVoltages() caught a CORBA::SystemException while "
           << " attempting to contact Tel " << fTelescopeID+1
           << ":  " << ex.NP_minorString() << "  "
           << endl;
    }
}




//   void showStars()
//   void hideStars()
//   void showArcs()
//   void hideArcs()
//   vector< any > getStarList() // syntax!

void VNETClientThread::getHVStatus()//determine if HV is off before reading ADC offsets
{
}


void VNETClientThread::setPixelStatus(short int pixel, short int status)//send pixel status according
{
}


//0=off
//1=on
//2=bad
void VNETClientThread::sendCurrent(short int pixel, float value)//send values across 1 by 1
{
}


void VNETClientThread::updateHVDisplay()//once the pixel value array has been filled
{
}


//send a command to introduce the display update into the
//QT event loop. (not currently used)
void VNETClientThread::setPixelLow(short int pixel)//turn down a pixel in a bright field
{
}


void VNETClientThread::restorePixel(short int pixel)//restore a low pixel (not currently used)
{
}


void VNETClientThread::killPixel(short int pixel)//turn off a pixel
{
}


void VNETClientThread::killAll()//turn them all off and hope for the best
{
}


//    void setHVStatus(short int value)//could add this to ensure HV not switched on whilst imon not reading?
