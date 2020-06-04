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

void setDeviceName(const char * device_name)
{
  FakeGNSSSimulator::get()->setDeviceName(device_name);
}

const char * getDeviceName(void) { return FakeGNSSSimulator::get()->getDeviceName(); }

void setUBXLogFile(const char * ubx_log_file)
{
  return FakeGNSSSimulator::get()->setUBXLogFile(ubx_log_file);
}

const char * getUBXLogFile(void) { return FakeGNSSSimulator::get()->getUBXLogFile(); }

void setUBXPVTLogFile(const char * ubx_pvt_log_file)
{
  return FakeGNSSSimulator::get()->setUBXPVTLogFile(ubx_pvt_log_file);
}

const char * getUBXPVTLogFile(void) { return FakeGNSSSimulator::get()->getUBXPVTLogFile(); }

void setNMEALogFile(const char * nmea_log_file)
{
  FakeGNSSSimulator::get()->setNMEALogFile(nmea_log_file);
}

const char * getNMEALogFile(void) { return FakeGNSSSimulator::get()->getNMEALogFile(); }

int start(void) { return FakeGNSSSimulator::get()->start(); }

void stop(void) { FakeGNSSSimulator::get()->stop(); }

void setChecksumError(int is_error) { FakeGNSSSimulator::get()->setChecksumError(is_error); }

void setDebugOutput(int is_debug) { FakeGNSSSimulator::get()->setDebugOutput(is_debug); }

#ifdef __cplusplus
}
#endif
