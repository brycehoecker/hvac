///////////////////////////////////////////////////////////////////////////////
// $Id: txmlclient.cpp,v 1.1.1.1 2006/01/18 20:18:51 ergin Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#include "txmlclient.h"
#include <qcstring.h>
#include <qtextstream.h>
#include <iostream> 

using namespace std;

extern ofstream kLogFile;

/*!
  \class TXMLclient txmlclient.h
  \brief The TXMLclient controls XML communications with a server
  
  \sa QSocket and QXmlSimpleReader
*/

/*!
Creates a client object that will also initialise the parser and socket.
 */
//TXMLClient::TXMLClient( const char* host=0, Q_UINT16 port=0, QWidget* parent=0, const char* name=0 )
TXMLClient::TXMLClient( const char* host, Q_UINT16 port, QWidget* parent, const char* name )
: QSocket( parent, name ), fHost( host ), fPort( port), fHandler(0)
{
  kLogFile << "txmlclient::txmlclient(): host=" << host
	   << " port=" << port
	   << " parent=" << parent
	   << " name=" << name << endl;
  fParser = new QXmlSimpleReader(); // READER - parses the xml
  connect( this, SIGNAL( connectionClosed()), SLOT(receive()) );
  connect( this, SIGNAL( connected() ), SLOT( request() ) );
  connect( this, SIGNAL( error(int)), SLOT( error(int)) );
}

/*!
Destructs the Client socket and parser
*/
TXMLClient::~TXMLClient()
{
    close();
    delete fParser; // don't delete fHandler, it belongs to someone else
}

/*!
Sets the XML handler which is derived from TClientHandler.
It dictates how the parser responds to the XML data.
*/
void TXMLClient::setHandler( TClientHandler* handler )
{
  fHandler = handler;
  fParser->setContentHandler( handler );
}

/*!
  Connects our socket to the server at the address fHost:fPort
 */
void TXMLClient::connectHost(void)
{
  kLogFile << "TXMLClient::connectHost() calling connectToHost()...";
  connectToHost( fHost, fPort );
  kLogFile << " ...done." << endl;
  //   emit statusInfo( VOK, "Sending connection request");
  
  // emit newInfo( "Connected to "+peerName() );
  //  QString s =  "Trying to connect to "+peerName();
  // printf("%s\n",s.latin1());
}

/*!
\brief Sets the XML request string which is sent by default to the server
*/
void TXMLClient::setXmlString( QString doc )
{
  fXmlString = doc;
}

/*!
  Sends the XML request document over the socket (to the server).
  By default uses the fXmlString member.
  \sa setXmlString()
*/
void TXMLClient::request(void)
{
  time_t tmpTime;
  time( &tmpTime );
  char tmpString[200];
  strftime( tmpString, 200,
            "%Y%m%d %T",
            gmtime( &tmpTime ) );
  kLogFile << tmpString
           << ": request(): connected, sending request"
           << endl;
  request( fXmlString );
}
/*!
  Sends the XML request document over the socket (to the server).
  Overloaded to accept a QString parameter
*/
void TXMLClient::request(QString& doc)
{
  kLogFile << "TXMLClient::request(QString& doc):  requesting...";
  QTextStream ts( this );
  ts << doc;
  
//   emit statusInfo( VOK, "Connected: sending request" );
  //  QString s =  "Connected - sending: "+doc;
  //  printf("%s",s.latin1());
  kLogFile << "  ...returning" << endl;
}

/*!
 Uses the server data parser input (set as a textstream from the socket) and starts the XML parser. 
 */
void TXMLClient::receive(void)
{
  time_t tmpTime;
  time( &tmpTime );
  char tmpString[200];
  strftime( tmpString, 200,
            "%Y%m%d %T",
            gmtime( &tmpTime ) );
  kLogFile << tmpString
           << ": TXMLClient::receive(): connection closed, parsing reply."
           << endl;

  QTextStream* ts = new QTextStream( this );
  QXmlInputSource* XMLInput = new QXmlInputSource( *ts );

  if ( fParser->parse( *XMLInput ) ) {
    strftime( tmpString, 200,
	      "%Y%m%d %T",
	      gmtime( &tmpTime ) );
    kLogFile << tmpString
	     << ": TXMLClient::receive(): connection closed, parsed reply: OK"
	     << endl;
    emit statusInfo( VOK, "Received response: parsed OK" );
    kLogFile << "emitted OK" << endl;
  } else {
    strftime( tmpString, 200,
	      "%Y%m%d %T",
	      gmtime( &tmpTime ) );
    kLogFile << tmpString
	     << ": TXMLClient::receive(): connection closed, parsed reply: Error"
	     << endl;
    
    emit statusInfo( VERROR, "Received response: parse failed" ); // UC
    kLogFile << "emitted error" << endl;
  }

  kLogFile << "Closing connection...  ";
  close();   //close after received data
  kLogFile << "Disconnected." << endl;

  delete XMLInput;
  delete ts;
}

void TXMLClient::error(int num)
{
  switch(num){
  case 0:
    emit statusInfo( VERROR, "Socket Error: Connection Refused");
    break;
  case 1:
    emit statusInfo( VERROR, "Socket Error: Host Not Found");
    break;
  case 2:
    emit statusInfo( VERROR, "Socket Error: Socket Read Error");
    break;
  }
}
