/*
 * Application-specific functions for Positioner
 * interaction with the database.
 */

#include "VDBException.h"
#include "VDBHighVoltage.h"
#include "VDBRow.h"
#include "VDBtablehandling.h"
#include "VDBTransaction.h"



#include <iostream>
using namespace std;


void VDBHV::putVoltage(unsigned int telescope_id, unsigned int channel, float voltage)
  throw(VDBException)
{
  try {
    std::ostringstream tablename;
    tablename <<"tblHV_Telescope" << telescope_id << "_Voltages";
    std::ostringstream insertvals;
    insertvals << "(channel, voltage) ";
    insertvals << "values (";
    insertvals << channel <<",";
    insertvals << voltage <<")";
    std::ostringstream whereclause;
    whereclause << "channel=" << channel;
    VDBTBL::putInTimestampTableNoDups(tablename.str(), insertvals.str());	
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::putVoltage";
    throw ex;
  }
}


float VDBHV::getVoltage(unsigned int telescope_id, unsigned int channel, unsigned long long datetime)
  throw(VDBException)
{
  try {
    std::ostringstream tablename;
    tablename <<"tblHV_Telescope" << telescope_id << "_Voltages";
    std::ostringstream whereclause;
    whereclause << "channel=" << channel;
    VDBRow r=VDBTBL::getFromTimestampTable(tablename.str(), whereclause.str(), datetime);
    return r.getItem< float >("voltage");
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::getVoltage";
    throw ex;
  }
}


float VDBHV::getVoltage(unsigned int telescope_id, unsigned int channel)
  throw(VDBException)
{
  try {
    VDBTransaction t;
    do {
      try { //catch deadlock exceptions
	std::ostringstream strbuf;
	strbuf << "select voltage from tblHV_Telescope" << telescope_id <<"_Voltages ";
	strbuf << " where db_end_time is NULL and channel=" << channel;
	return t.getOneItem< float >(strbuf.str().c_str()); 
      }
      catch(VDBDeadlock &ex) {}
    } while (t.IsNotComplete());
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::getVoltage";
    throw ex;
  }
}


void VDBHV::turnOn(unsigned int telescope_id, unsigned int channel)
  throw(VDBException)
{
  try {
    std::ostringstream tablename;
    tablename <<"tblHV_Telescope" << telescope_id << "_Power";
    std::ostringstream insertvals;
    insertvals << "(channel, status) ";
    insertvals << "values (";
    insertvals << channel <<",";
    insertvals << "\"ON\")";
    std::ostringstream whereclause;
    whereclause << "channel=" << channel;
    VDBTBL::putInTimestampTableNoDups(tablename.str(), insertvals.str(), whereclause.str());	
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::turnOn";
    throw ex;
  }
}


void VDBHV::turnOff(unsigned int telescope_id, unsigned int channel)
  throw(VDBException)
{
  try {
    std::ostringstream tablename;
    tablename <<"tblHV_Telescope" << telescope_id << "_Power";
    std::ostringstream insertvals;
    insertvals << "(channel, status) ";
    insertvals << "values (";
    insertvals << channel <<",";
    insertvals << "\"OFF\")";
    std::ostringstream whereclause;
    whereclause << "channel=" << channel;
    VDBTBL::putInTimestampTableNoDups(tablename.str(), insertvals.str(), whereclause.str());	
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::turnOff";
    throw ex;
  }
}


std::string VDBHV::getPowerStatus(unsigned int telescope_id, unsigned int channel, unsigned long long datetime)
  throw(VDBException)
{
  try {
    std::ostringstream tablename;
    tablename << "tblHV_Telescope" << telescope_id << "_Power";
    std::ostringstream whereclause;
    whereclause << "channel=" << channel;
    VDBRow r=VDBTBL::getFromTimestampTable(tablename.str(), whereclause.str(), datetime);
    HVStatusInfo status;
    return r.getItem<std::string>("status");
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::getPowerStatus";
    throw ex;
  }
}


std::string VDBHV::getPowerStatus(unsigned int telescope_id, unsigned int channel)
  throw(VDBException)
{
  try {
    VDBTransaction t;
    do {
      try { //catch deadlock exceptions
	std::ostringstream strbuf;
	strbuf << "select status from tblHV_Telescope" << telescope_id <<"_Power";
	strbuf << " where db_end_time is NULL and channel=" << channel;
	return t.getOneItem<std::string>(strbuf.str().c_str()); 
      }
      catch(VDBDeadlock &ex) {}
    } while (t.IsNotComplete());
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::getPowerStatus";
    throw ex;
  }
}


void VDBHV::putHVStatus(unsigned int telescope_id, unsigned int channel, float voltage_meas, float current_meas)
  throw(VDBException)
{
  try {
    std::ostringstream tablename;
    tablename <<"tblHV_Telescope" << telescope_id << "_Status";
    std::ostringstream insertvals;
    insertvals << "(channel, voltage_meas, current_meas) ";
    insertvals << "values (";

    insertvals << channel <<",";
    insertvals << voltage_meas <<",";
    insertvals << current_meas <<")";
    VDBTBL::putInTimestampTable(tablename.str(), insertvals.str());	
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::putHVStatus";
    throw ex;
  }
}


VDBHV::HVStatusInfo VDBHV::getHVStatus(unsigned int telescope_id, unsigned int channel, unsigned long long datetime)
  throw(VDBException)
{
  try {
    std::ostringstream tablename;
    tablename << "tblHV_Telescope" << telescope_id << "_Status";
    std::ostringstream whereclause;
    whereclause << "channel=" << channel;
    VDBRow r=VDBTBL::getFromTimestampTable(tablename.str(), whereclause.str(), datetime);
    HVStatusInfo status;
    status.voltage_measured=r.getItem<float>("voltage_meas");
    status.voltage_target=getVoltage(telescope_id,channel,datetime);
    status.current_measured=r.getItem<float>("current_meas");
    status.isPowerOn = true;
    return status;
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::getHVStatus";
    throw ex;
  }
}


VDBHV::HVStatusInfo VDBHV::getHVStatus(unsigned int telescope_id, unsigned int channel)
  throw(VDBException)
{
  try {
    VDBTransaction t;
    do {
      try { //catch deadlock exceptions
	std::ostringstream strbuf;
	strbuf << "select * from tblHV_Telescope" << telescope_id <<"_Status";
	strbuf << " where db_end_time is NULL and channel=" << channel;
	VDBRow r = t.getOneRow(strbuf.str().c_str()); 
	HVStatusInfo status;
	status.voltage_measured=r.getItem<float>("voltage_meas");
	status.voltage_target=getVoltage(telescope_id,channel);
	status.current_measured=r.getItem<float>("current_meas");
	status.isPowerOn = true;
	return status;
      }
      catch(VDBDeadlock &ex) {}
    } while (t.IsNotComplete());
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::getHVStatus";
    throw ex;
  }
}


void VDBHV::putDefaultVoltage(unsigned int telescope_id, unsigned int channel, float voltage)
  throw(VDBException)
{
  try {
    VDBTransaction t;
    do {
      try { //catch deadlock exceptions
	std::ostringstream strbuf;
	strbuf << "update tblHV_Telescope" << telescope_id <<"_DefaultVoltage";
	strbuf << " set voltage=" << voltage;
	strbuf << " where channel=" << channel;
	t.execute(strbuf.str().c_str()); 
      }
      catch(VDBDeadlock &ex) {}
    } while (t.IsNotComplete());
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::putDefaultVoltage";
    throw ex;
  }
}


float VDBHV::getDefaultVoltage(unsigned int telescope_id, unsigned int channel)
  throw(VDBException)
{
  try {
    VDBTransaction t;
    do {
      try { //catch deadlock exceptions
	std::ostringstream strbuf;
	strbuf << "select voltage from tblHV_Telescope" << telescope_id <<"_DefaultVoltage";
	strbuf << " where channel=" << channel;
	return t.getOneItem< float >(strbuf.str().c_str()); 
      }
      catch(VDBDeadlock &ex) {}
    } while (t.IsNotComplete());
  }
  catch(VDBException &ex) {
    ex.stream() << "called from VDBHV::getDefaultVoltage";
    throw ex;
  }
}


