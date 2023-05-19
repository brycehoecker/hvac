#include <iostream>
#include <exception.h>
#include <Vcorba.h>

#include "VHVArrayServer.h"

using namespace std;

Vcorba::corba_server *corbaServer;

VHVArrayServer::VHVArrayServer(VHVData *data, VHVControl *control )
  : QObject(0,0)
{
  fData = data;
  fControl = control;
}

void VHVArrayServer::run()
{
    try {

      std::string hostname;
      hostname = "";
      std::cerr << "Using local host as corba nameserver" << "\n";

      Vcorba::corba_server cs(hostname);
      corbaServer = &cs;
      
      // create servant
      VHVArrayFunctions_i *serv = new VHVArrayFunctions_i( fData, fControl, corbaServer );

      corbaServer->activateAndRegister(serv, "Server", "VHVArrayFunctions");

      // actually fire up CORBA
      std::cerr
	<< "ORB server thread running, ready for connections."
	<< std::endl;
      
      corbaServer->run();

    } catch (VException &e) {
      std::cerr << e << std::endl;
//       return 2;
    } catch (...) {
      std::cerr << "VHVArrayServer::run():  Unknown exception!" << std::endl;
//       return 1;
    }
    
//     return 0;
}
