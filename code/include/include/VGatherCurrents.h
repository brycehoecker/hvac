///////////////////////////////////////////////////////////////////////////////
// $Id: VGatherCurrents.h,v 1.1 2006/03/28 02:56:19 humensky Exp $
//
// Filename : 
// Created  : Mon Mar 27 2006 by Brian Humensky
///////////////////////////////////////////////////////////////////////////////
#ifndef VGATHERCURRENTS_H
#define VGATHERCURRENTS_H

/** \file VGatherCurrents.h
 *  \brief Header file for the VGatherCurrents class (defined in VGatherCurrents.cpp) 
 
 * This class runs as a thread - gathering data from the HV system and also
 * from the current monitor process and placing information in a VHVData 
 * object.
*/

#include <cmath>

#include <qobject.h>
#include <qthread.h>
#include <qwidget.h>

#include <VDBHighVoltage.h>
#include <VDBArrayControl.h>
#include <VDBPositioner.h>

#include "VHV.h"
#include "VHVSystem.h"
#include "VHVData.h"
#include "VHVPST.h"
#include "txmlclient.h"
#include "tclienthandler.h"
#include "tdevicedata.h"
#include "astrometry.h"
#include "VeritasCam.h"

static const int kVERITASPort = 10001;

class VGatherCurrents : public QObject  {

Q_OBJECT
  
 public:
  VGatherCurrents( VHVData *data );
  virtual ~VGatherCurrents(); 

  void updateCurrents();
  
 signals:
  void printMessage( const QString & string );

 public slots:
  void currentsStatus(int status, const char *message);

 private: 
  VHVData        *fData;
  TXMLClient     *fXMLClient;
  TClientHandler *fXMLHandler;
  TDeviceData    *fXMLData;

};


#endif
