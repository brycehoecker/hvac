///////////////////////////////////////////////////////////////////////////////
// $Id: txmlclient.h,v 1.1.1.1 2006/01/18 20:18:51 ergin Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#ifndef TXMLCLIENT_H
#define TXMLCLIENT_H

#include <qxml.h>
#include <qsocket.h>
#include "tclienthandler.h"
#include "VHV.h"

class TXMLClient : public QSocket
{
  Q_OBJECT
    public:
  TXMLClient( const char* host=0, Q_UINT16 port=0,QWidget* parent=0, const char* name=0 );
  ~TXMLClient();
  
  void setHandler( TClientHandler* );

 signals:
  void statusInfo( int status, const char* );
  
  public slots:
  void setXmlString( QString );
  void connectHost( void );
  void request( void );
  void request( QString& );
  void receive( void );
  void error( int );

 private:
  //! The XML parser
  QXmlSimpleReader* fParser; 
  //! Data communicated thru this socket
  QSocket* fSocket;  
  //! SAX handler for the parser. Dictates Parser behaviour.
  TClientHandler* fHandler;  
  //! Server Hostname 
  const char* fHost;      
  //! Server Port number
  Q_UINT16 fPort; 
  //! The Xml request string sent by request()
  QString fXmlString;  
};

#endif //TXMLCLIENT_H
