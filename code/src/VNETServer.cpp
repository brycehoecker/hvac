// VNETServer.cpp 
//=======================================================================||
//                                                                       ||
//	Example CORBA server						 ||
//                                                                       ||
//      J. Kildea 26 JAN 04                                              ||
// modified by MKD to send/receive imon specific data 7/9/04
//=======================================================================||
// project specific includes
#include "VNETFunctionsServerThread.h"

// standard includes
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	//================================================================
	// Instantiate a VNETFunctionsServerThread Thread object and set it 
	// free
	//================================================================
	cout << "Starting separate thread..." << endl;
	VNETFunctionsServerThread NETFunctionsServerThread(argc, argv);
	NETFunctionsServerThread.run();

	cout << "Back to main thread..."<< endl;

	//================================================================
	// That's all folks!!
	//================================================================
	return 0;
	
}
