/**
 * @file interface.cpp
 * @brief Interfacing to C++ code
 */

#include <fake_gnss_simulator.h>
#include <interface.h>

#ifdef __cplusplus
extern "C" {
#endif

void loadIniFile(void) { FakeGnssSimulator::get()->loadIniFile(); }

void saveIniFile(void) { FakeGnssSimulator::get()->saveIniFile(); }

void setDeviceName(const char * device_name)
{
  FakeGnssSimulator::get()->setDeviceName(device_name);
}

const char * getDeviceName(void) { return FakeGnssSimulator::get()->getDeviceName(); }

void setUBXLogFile(const char * ubx_log_file)
{
  return FakeGnssSimulator::get()->setUBXLogFile(ubx_log_file);
}

const char * getUBXLogFile(void) { return FakeGnssSimulator::get()->getUBXLogFile(); }

void setNMEALogFile(const char * nmea_log_file)
{
  FakeGnssSimulator::get()->setNMEALogFile(nmea_log_file);
}

const char * getNMEALogFile(void) { return FakeGnssSimulator::get()->getNMEALogFile(); }

int start(void) { return FakeGnssSimulator::get()->start(); }

void stop(void) { FakeGnssSimulator::get()->stop(); }

void setChecksumError(int is_error) { FakeGnssSimulator::get()->setChecksumError(is_error); }

void setDebugOutput(int is_debug) { FakeGnssSimulator::get()->setDebugOutput(is_debug); }

#ifdef __cplusplus
}
#endif
