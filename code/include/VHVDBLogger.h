#ifndef VHVDBLOGGER_H
#define VHVDBLOGGER_H

/** \file VHVDBLogger.h
 * \brief Header file for classes in VHVDBLogger.cpp (all associated with camera display widget).
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <qobject.h>
/* #include <qthread.h> */

#include "VHV.h"
#include "VHVData.h"
#include "VDBCamera.h"
#include "VDBCamStruct.h"

/** \bool kDBLogging
 *  Flag for logging to Database.  Default is off (false).
 */

//---------------------------------------------------------

/* class VHVDBLogger : public QObject, public QThread { */
class VHVDBLogger : public QObject {

Q_OBJECT    

 public:
  VHVDBLogger( VHVData *data, bool freeRunning=true );
  virtual ~VHVDBLogger();
 void logOneStatusToDB( int chan );

/*  protected: */
/*   void run(); */

 public slots:
   void logStatusToDB2();

 private:
/*   void logStatusToDB2(); */
  
  VHVData *fData;
  bool     fFreeRunning;
  QTimer  *fTimer;
  char     fLocalTextLog[200];

};

#endif
