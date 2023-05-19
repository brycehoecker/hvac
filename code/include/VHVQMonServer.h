#ifndef VHVQMonServer_H
#define VHVQMonServer_H

/** \file VHVQMonServer.h
 *  \brief Header file for the VHVQMonServer class (defined in VHVQMonServer.cpp) 
 
 * This class runs as a thread - receiving data from the current monitor 
 * process and placing information in a VHVData object.
 */

#include <qobject.h>
#include <qthread.h>

/* #include "VHVSystem.h" */
#include "VHVData.h"


class VHVQMonServer : public QObject, public QThread  {

Q_OBJECT
  
 public:
  VHVQMonServer( VHVData *data );
  virtual ~VHVQMonServer(); 
 
 protected:
  virtual void run();
  
/*  signals: */
/*   void newData(); /\**< signal the arrival of new data *\/ */

/*  public slots: */
/*   void setMode(int mode); */
/*   void currentsStatus(int status, const char *message); */

 private:
  
/*   void updateHV();  */
  
  VHVData      *fData;
/*   VHVSystem    *fHV[NUM_CRATES]; */
/*   TXMLClient     *fXMLClient; */
/*   TClientHandler *fXMLHandler; */
/*   TDeviceData    *fXMLData; */
  
};

#endif
