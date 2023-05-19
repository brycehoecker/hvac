///////////////////////////////////////////////////////////////////////////////
// $Id: tdevicedata.h,v 1.1.1.1 2006/01/18 20:18:51 ergin Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#ifndef TDEVICEDATA_H
#define TDEVICEDATA_H

#include <qstring.h>
#include <qwidget.h>
#include "VHVData.h"


class TDeviceData : public QWidget
{
Q_OBJECT 

 public:
  TDeviceData(  VHVData *data );
  ~TDeviceData();

  void setTimeString( const QString & );
  const QString& getTime(); 
  void setPMT( int channel, float current, int status );
  void setTemp( int temp );
  void setHum( int hum );
  void setSensor( int sensor, int value );

 private:
  QString fTime;
  VHVData *fData;
  
};

#endif 
