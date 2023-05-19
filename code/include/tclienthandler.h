///////////////////////////////////////////////////////////////////////////////
// $Id: tclienthandler.h,v 1.1.1.1 2006/01/18 20:18:51 ergin Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#ifndef TCLIENTHANDLER_H
#define TCLIENTHANDLER_H

#include <qxml.h>
#include <qstring.h>
#include "tdevicedata.h"
#include "VHV.h"

class TClientHandler : public QXmlDefaultHandler
{
public:
  TClientHandler();
  bool startElement( const QString&, const QString&, const QString& name, const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& name );
  bool endDocument();
  bool characters( const QString& );

  bool error( const QXmlParseException& );
  bool fatalError( const QXmlParseException& );
  bool warning( const QXmlParseException& );

  void setDeviceData(TDeviceData* );
 private:
  bool fInTime;
  TDeviceData* fDeviceData;
};

#endif
