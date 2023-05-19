// client.cpp

#include <iostream>
#include "exception.h"
#include "Vcorba.h"

#include "VHVArrayServer.hh"
#include "VHV.h"
#include "VHVSystem.h"
#include "VHVData.h"
// #define THIS_DOES_NOT_WORK

int
main(int argc, char *argv[])
{
    Command_var server_ref;

    try {
	std::string hostname;
	if (argc == 1) {
	    hostname = "imon.t1.vts";
	    std::cerr << "Using local host as corba nameserver" << "\n";
	} else if (argc == 2) {
	    hostname = argv[1];
	    std::cerr << "Using " << hostname << " as corba nameserver" << "\n";
	} else {
	    std::cerr << "usage: " << argv[0] << " [hostname]" << "\n";
	    exit(1);
	}

	std::cout << "creating corba_client"<<std::endl;
	Vcorba::corba_client c(hostname);
	//Vcorba::corba_client c(argc, argv);
	try {
	  std::cout<<"creating obj_var"<<std::endl;
	    // This is what I do...
	    CORBA::Object_var obj = c.getObjectRef("Server", "Command", 
	    	Vcorba::NEVER_TIME_OUT);
	    std::cout<<"narrowing"<<std::endl;
	    server_ref = Command::_narrow(obj);

#ifdef THIS_DOES_NOT_WORK
	    // But this is what I would like to do. Unfortunately, Command
	    // and Command_var are some kind of hairy typedefs, and it just
	    // doesn't seem to work with templates.
	    std::cout<<"this shouldn't print"<<std::endl;
	    server_ref =
	    	c.get_obj_ref< Command, Command_var >("Server", "Command");
#endif // THIS_DOES_NOT_WORK

	    std::cout<<"prepare for input"<<std::endl;
	    std::string input;
	    std::cout << "Ready for input." << std::endl;
	    while (1) {
		std::cout << "Nu? " << std::flush;
		std::getline(std::cin, input);
		if (input == "X") {
		    std::cout << "Exiting" << std::endl;
		    break;
		} else if (input == "q") {
		    server_ref->quitVHV();
		    break;
		}
	    }

        } catch (...) {
	    throw;
        }
    } catch (CORBA::Exception &e) {
	std::cerr << Vcorba::string_from_corba_exception(e) << std::endl;
	return 3;
    } catch (VException &e) {
	std::cerr << e << std::endl;
	return 2;
    } catch (...) {
	std::cerr << "Unknown exception." << "\n";
	return 2;
    }

    return 0;
}
