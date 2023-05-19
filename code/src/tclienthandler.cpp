///////////////////////////////////////////////////////////////////////////////
// $Id: tclienthandler.cpp,v 1.1.1.1 2006/01/18 20:18:51 ergin Exp $
//
// Filename : 
// Created  : Tue Mar 19 2002 by Jim Hinton
///////////////////////////////////////////////////////////////////////////////
#include "tclienthandler.h"
#include <iostream>

#include "tdevicedata.h"

using namespace std;

extern ofstream kLogFile;

/*! \class TClientHandler tclienthandler.h
\brief XML client. Parses XML information recieved from the currents server
*/

/** Constructor. Initialises the default handler */
TClientHandler::TClientHandler()  : QXmlDefaultHandler()
{
}
/*!
An implementation of the virtual function in the base TClientHandler class.
This sets the internal fDeviceData member to point at a TimeDeviceData object which is used to store the recieved info.
*/
void TClientHandler::setDeviceData( TDeviceData* device )
{
    fDeviceData = static_cast<TDeviceData*>(device);
}

/*! This function is invoked when the client parser encounters a start element (e.g. <image>).
For the Time, start elements will be tags surrounding the following data:
  image data
  temperature data
  other status information
A boolean flag (private) is set based on the name of the tag to provide a context for the data.
This provides a context when in the characters() function.
 For example, the 'fInImage' flag is set when a Time image start tag is encountered.
*/
bool TClientHandler::startElement(  const QString& namespaceURI, const QString& localName, const QString& name, const QXmlAttributes& attributes)
{
  QString timeString;
  QString currentString;
  QString channelString;
  QString statusString;
  QString temperatureString;

  //  printf("START NAME %s\n",name.latin1());

  if ( name == "time") {
    fInTime = TRUE;
    return TRUE;
  } else if ( name == "pmt") {
    currentString = attributes.value("cua");
    channelString = attributes.value("id");
    statusString = attributes.value("st");
    fDeviceData->setPMT
      ( channelString.toInt(), currentString.toFloat(), statusString.toInt() );
    return TRUE;
  } else if ( name == "data") {
    return TRUE;
  } else if ( name == "shutter") {
    return TRUE;
  } else if ( name == "illum") {
    return TRUE;
  } else if ( name == "timestamp") {
    timeString = attributes.value("mst");
    fDeviceData->setTimeString( timeString );
    return TRUE;
  } else if ( name == "temp" ) {
//     cout << "reading temp info" << endl;
//     fprintf( stdout, "reading temp info -- writing via fprintf\n" );
    temperatureString = attributes.value( "tc" );
    channelString = attributes.value( "id" );
//     cout << "sensor id: " << channelString << "\ttemp: " << temperatureString
// 	 << endl;
    if ( channelString.toInt() == 1 ) {
      fDeviceData->setTemp( (int)(temperatureString.toFloat()) );
    }
    if ( channelString.toInt() == 2 ) {
      fDeviceData->setSensor( SENSOR_AMBTEMP,
			      (int)(temperatureString.toFloat()) );
    }
    return TRUE;
  } else if ( name == "hum" ) {
//     cout << "reading hum info" << endl;
//     fprintf( stdout, "reading hum info -- writing via fprintf\n" );
    temperatureString = attributes.value( "relhum" );
    channelString = attributes.value( "id" );
//     cout << "sensor id: " << channelString << "\thum: " << temperatureString
// 	 << endl;
    if ( channelString.toInt() == 1 ) {
      fDeviceData->setHum ( (int)(temperatureString.toFloat()) );
    }
    if ( channelString.toInt() == 2 ) {
      fDeviceData->setSensor( SENSOR_AMBHUM,
			      (int)(temperatureString.toFloat()) );
    }
    return TRUE;
  }
  kLogFile << "TClientHandler::startElement():  " << name << ":  ";
  kLogFile << "startElement returning FALSE" << endl;
  
  return FALSE;
}

/*! 
This function is invoked when the client parser encounters an end element (e.g. </image>).
For the Time, the end tag will follow the data, which follows the start element.
The corresponding boolean flag is cleared to provide the correct context.
*/
bool TClientHandler::endElement(  const QString& namespaceURI, const QString& localName, const QString& name)
{

  //  printf("END NAME %s\n",name.latin1());

  // Legitimate end tags are time, data, pmt or timestamp.
  // but what happens if I allow temp and hum?
  if ( ( name == "time") || ( name == "data" ) 
       || (name == "pmt") || (name == "timestamp")
       || ( name == "temp" ) || ( name == "hum" )
       || ( name == "shutter" ) || ( name == "illum" ) ) {
    return TRUE;
  } else {
    kLogFile << "TClientHandler::endElement returning FALSE" << endl;
    return FALSE;
  }
}

/*!
This function is invoked when the client parsers encounters character data (i.e. text) surrounded by tags.
Based on the tags previously encountered, a context for the data (i.e. which tags surround it) is known.
This data is assigned to the correct location based on the current context.
*/
bool TClientHandler::characters( const QString& ch )
{

  kLogFile << "characters: " << ch.latin1() << endl;;

  // Only expect character input in time field.
  if (fInTime) {
    fDeviceData->setTimeString( ch );
    return TRUE;
  } 

  kLogFile << "characters returning FALSE" << endl;

  return FALSE;
}
/*!
An implementation of the handlers provided by the default handler.
Writes to stderr all of the available information about the error.
*/
bool TClientHandler::error( const QXmlParseException& ex )
{
  cerr << "Error:  Parse Error @ "
       << " line number: " << ex.lineNumber()       
       << " Column number: " << ex.columnNumber()
       << " Message: " << ex.message()
       << " System ID: " << ex.systemId()
       << " Public ID " << ex.publicId()
       << endl;
  kLogFile << "Error:  Parse Error @ "
	   << " line number: " << ex.lineNumber()       
	   << " Column number: " << ex.columnNumber()
	   << " Message: " << ex.message()
	   << " System ID: " << ex.systemId()
	   << " Public ID " << ex.publicId()
	   << endl;
  return TRUE; //continue parsing
}

/*!
An implementation of the handlers provided by the default handler.
Writes to stderr all of the available information about the error.
*/
bool TClientHandler::warning( const QXmlParseException& ex )
{
  cerr << "Warning:  Parse Error @ "
       << " line number: " << ex.lineNumber()
       << " Column number: " << ex.columnNumber()
       << " Message: " << ex.message()
       << " System ID: " << ex.systemId()
       << " Public ID " << ex.publicId()
       << endl;
  kLogFile << "Warning:  Parse Error @ "
	   << " line number: " << ex.lineNumber()
	   << " Column number: " << ex.columnNumber()
	   << " Message: " << ex.message()
	   << " System ID: " << ex.systemId()
	   << " Public ID " << ex.publicId()
	   << endl;

  return TRUE; //continue parsing
}

/*!
An implementation of the handlers provided by the default handler.
Writes to stderr all of the available information about the error.
*/
bool TClientHandler::fatalError( const QXmlParseException& ex )
{
  cerr << "Fatal Error:  Parse Error @ "
       << " line number: " << ex.lineNumber()
       << " Column number: " << ex.columnNumber()
       << " Message: " << ex.message()
       << " System ID: " << ex.systemId()
       << " Public ID " << ex.publicId()
       << endl;
  kLogFile << "Fatal Error:  Parse Error @ "
	   << " line number: " << ex.lineNumber()
	   << " Column number: " << ex.columnNumber()
	   << " Message: " << ex.message()
	   << " System ID: " << ex.systemId()
	   << " Public ID " << ex.publicId()
	   << endl;

  return FALSE; // stop parsing if false
}

/** Function called at end of document. Does nothing, returns true. */
bool TClientHandler::endDocument()
{
  kLogFile << "TClientHandler::endDocument() -- returning TRUE" << endl;
  return(TRUE);

}
