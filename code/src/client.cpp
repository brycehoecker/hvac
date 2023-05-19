/*!
 \class client.cpp
 \brief demonstration client class for vdcmon communications via CORBA

 \author mkd 29/11/05 (michael.daniel@ucd.ie)
  based on Vcorba 1.5 by Marty Olevitch (marty@cosray.wustl.edu)

 usage: ./client [hostname]

 This assumes that vdcmon is the client program that sends imon data
 to a server vhv program. 

 if no response is returned from the server program this will either 
 wait forever or time out after a specified amount of time. 
 At present no time out period is set so the former is true.

 \todo add further exception handling
       test for memory leaks

*/
#include <iostream>
#include "exception.h"
#include "Vcorba.h"

#include "VDCMonComms.hh"

// using namespace std;

int kTelescopeID = 0;

int main(int argc, char *argv[]){
  VDCMonComms_var server_ref; // this stores the reference to the server program

  try{
      std::string hostname;
      // defaults to local host as omniNames server
      if(argc == 1){
          hostname = "";
          std::cerr << "Using local host as corba nameserver" << "\n";
       }else if (argc == 2){
         hostname = argv[1];
         std::cerr << "Using " << hostname << " as corba nameserver" << "\n";
       }else{
         std::cerr << "usage: " << argv[0] << " [hostname]" << "\n";
         exit(1);
       }//if CORBA hostname setup

       Vcorba::corba_client c(hostname);
       std::cout << "made client" << std::endl;
       //Vcorba::corba_client c(argc, argv);
       try{
	 char servStr[20], commStr[20];
	 sprintf( servStr, "vhvServer_T%d", kTelescopeID+1 );
	 sprintf( commStr, "CurrMon_T%d", kTelescopeID+1 );
	 std::cout << servStr << "     " << commStr << std::endl;
// since NEVER_TIME_OUT is set if client does not receive reply from 
// the server it will sit there and wait forever for a reply...
           CORBA::Object_var obj = c.getObjectRef( servStr, commStr, 
						   Vcorba::NEVER_TIME_OUT );
	   std::cout << "objd" << std::endl;
           server_ref = VDCMonComms::_narrow(obj);
	   std::cout << "narrowed" << std::endl;
//
// mkd removed Marty clutter from here
//
// mock data comms with current monitor
          VDCMonSummary  iDetails;
	  double randmax = 1.0 * RAND_MAX;

	  for ( int j=0; j<100; ++j ) {
	    iDetails.fTelID = 0; // ==T1
	    for(unsigned i=0; i<499; ++i){
	      iDetails.fPixels.fCurrents[i] = 60.*rand()/randmax;
	    }
	    for(unsigned i=0; i<5; ++i){
	      iDetails.fSensors.fT[i] = 20.*rand()/randmax;
	    }
	    iDetails.fSensors.fHumidity = 26;
	    server_ref->sendVDCMonData(iDetails);
	    for ( int k=0; k<40000000; ++k ) {
	      int q = sqrt( k );
	    }
	    std::cout << j << std::endl;
	  }
         //this would exit both the client and server programs...
         server_ref->quit();

       }catch(...){
         throw;//re-throw exceptions to next level
       }//try menu
  }catch(CORBA::Exception &e){
         std::cerr << Vcorba::string_from_corba_exception(e) << std::endl;
         return 3;
  }catch(VException &e){
         std::cerr << e << std::endl;
         return 2;
  }catch(...){
         std::cerr << "Unknown exception." << "\n";
         return 2;
  }//catch exceptions from main try

 return 0;
}//main end
