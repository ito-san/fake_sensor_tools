/**
 * @file interface.cpp
 * @brief Interfacing to C++ code
 */

#include <fake_gnss_simulator.h>
#include <interface.h>

#ifdef __cplusplus
extern "C" {
#endif

void loadIniFile(void) { FakeGNSSSimulator::get()->loadIniFile(); }

void saveIniFile(void) { FakeGNSSSimulator::get()->saveIniFile(); }

// General
void setDeviceName(const char * device_name)
{
  FakeGNSSSimulator::get()->setDeviceName(device_name);
}

const char * getDeviceName(void) { return FakeGNSSSimulator::get()->getDeviceName(); }

int start(void) { return FakeGNSSSimulator::get()->start(); }

void stop(void) { FakeGNSSSimulator::get()->stop(); }

void setChecksumError(int is_error) { FakeGNSSSimulator::get()->setChecksumError(is_error); }

void setDebugOutput(int is_debug) { FakeGNSSSimulator::get()->setDebugOutput(is_debug); }

// UBX-MON-HW
void setAStatus(AStatus aStatus) { FakeGNSSSimulator::get()->setAStatus(aStatus); }

AStatus getAStatus(void) { return FakeGNSSSimulator::get()->getAStatus(); }

void setJammingState(JammingState jammingState)
{
  FakeGNSSSimulator::get()->setJammingState(jammingState);
}

JammingState getJammingState(void) { return FakeGNSSSimulator::get()->getJammingState(); }

// UBX-MON-COMMS
void setPortId(PortId portId) { FakeGNSSSimulator::get()->setPortId(portId); }

PortId getPortId(void) { return FakeGNSSSimulator::get()->getPortId(); }

PortBlock getPortBlock(void) { return FakeGNSSSimulator::get()->getPortBlock(); }

void setPortEnabled(int port_enabled) { FakeGNSSSimulator::get()->setPortEnabled(port_enabled); }

void setTxUsage(int tx_usage) { FakeGNSSSimulator::get()->setTxUsage(tx_usage); }

// UBX-NAV-STATUS
void setSpoofDetState(SpoofDetState spoofDetState)
{
  FakeGNSSSimulator::get()->setSpoofDetState(spoofDetState);
}

SpoofDetState getSpoofDetState(void) { return FakeGNSSSimulator::get()->getSpoofDetState(); }

#ifdef __cplusplus
}
#endif
