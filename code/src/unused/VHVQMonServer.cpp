///////////////////////////////////////////////////////////////////////////////
// $Id: VHVQMonServer.cpp,v 1.1.1.1 2006/01/18 20:18:51 ergin Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#include <cstdio>
#include <cstdlib>

#include "VHVQMonServer.h"

/** \file VHVQMonServer.cpp
 * \brief Definition of class VHVQMonServer
 */

/** \class VHVQMonServer VHVQMonServer.h
 *  \brief Class to get/set information to/from the HV supply
 and current monitor.
*/

/** Constuctor. Initialises HV and current monitor communications 
 * \arg \c data - pointer to a VHVData object
 */
VHVQMonServer::VHVQMonServer(VHVData *data) : QObject(0,0)
{

  fData = data;
  cout << "Whoopee!" << endl;
}

/** Destructor
 */
VHVQMonServer::~VHVQMonServer()
{}

/** Continous loop: read current monitor and HV data and update
 *  VHVData object then pause.
 */
void VHVQMonServer::run() 
{
 
  
}

