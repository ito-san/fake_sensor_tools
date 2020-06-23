/**
 * @file interface.cpp
 * @brief Interfacing to C++ code
 */

#include <fake_imu_simulator.h>
#include <interface.h>

#ifdef __cplusplus
extern "C" {
#endif

void loadIniFile(void) { FakeIMUSimulator::get()->loadIniFile(); }

void saveIniFile(void) { FakeIMUSimulator::get()->saveIniFile(); }

// General
void setDeviceName(const char * device_name)
{
  FakeIMUSimulator::get()->setDeviceName(device_name);
}

const char * getDeviceName(void) { return FakeIMUSimulator::get()->getDeviceName(); }

int start(void) { return FakeIMUSimulator::get()->start(); }

void stop(void) { FakeIMUSimulator::get()->stop(); }

void setChecksumError(int is_error) { FakeIMUSimulator::get()->setChecksumError(is_error); }

void setDebugOutput(int is_debug) { FakeIMUSimulator::get()->setDebugOutput(is_debug); }

// BIN
void setLogFile(const char * log_file) { return FakeIMUSimulator::get()->setLogFile(log_file); }

const char * getLogFile(void) { return FakeIMUSimulator::get()->getLogFile(); }

#ifdef __cplusplus
}
#endif
