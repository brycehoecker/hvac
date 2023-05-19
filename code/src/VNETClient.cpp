// VNETClient.cpp 
//=======================================================================||
//                                                                       ||
//	Example CORBA client to do very simple message passing		 ||
//                                                                       ||
//      J. Kildea 26 JAN 04                                              ||
// modified by mkd for use in vdcmon 7/9/04
// Open the client orb in a seperate thread to prevent blocking <touch wood>
//=======================================================================||

//std includes
#include <iostream>           //for writing to screen, but not disk

//project specific includes
#include "VNETClientThread.h"
//#include "VReadConfig.h"      //for reading the vdcmon config file

using namespace std;

int main(int argc, char **argv){

//start a client thread and set it free
// if it comes back its broken... (or ended ;-)
cout << "starting separate thread" << endl;

VNETClientThread NETClientThread(argc,argv);
NETClientThread.run();

cout << "did it work?" << endl;

return 0;

}//main
