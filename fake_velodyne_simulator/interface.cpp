/**
 * @file interface.cpp
 * @brief Interfacing to C++ code
 */

#include <fake_velodyne_simulator.h>
#include <interface.h>

#ifdef __cplusplus
extern "C" {
#endif

void loadIniFile(void) { FakeVelodyneSimulator::get()->loadIniFile(); }

void saveIniFile(void) { FakeVelodyneSimulator::get()->saveIniFile(); }

// General
void setAddress(const char * address) { FakeVelodyneSimulator::get()->setAddress(address); }

const char * getAddress(void) { return FakeVelodyneSimulator::get()->getAddress(); }

int start(void) { return FakeVelodyneSimulator::get()->start(); }

void stop(void) { FakeVelodyneSimulator::get()->stop(); }

void setDebugOutput(int is_debug) { FakeVelodyneSimulator::get()->setDebugOutput(is_debug); }

// Info
void setInfoJson(const char * path) { FakeVelodyneSimulator::get()->setInfoJson(path); }

const char * getInfoJson(void) { return FakeVelodyneSimulator::get()->getInfoJson(); }

// Diagnostics
void setDiagJson(const char * path) { FakeVelodyneSimulator::get()->setDiagJson(path); }

const char * getDiagJson(void) { return FakeVelodyneSimulator::get()->getDiagJson(); }

double getTopHv(void) { return FakeVelodyneSimulator::get()->getTopHv(); }

void setTopHv(double top_hv) { FakeVelodyneSimulator::get()->setTopHv(top_hv); }

double getTopTemp(void) { return FakeVelodyneSimulator::get()->getTopTemp(); }

void setTopTemp(double top_temp) { FakeVelodyneSimulator::get()->setTopTemp(top_temp); }

double getTop5v(void) { return FakeVelodyneSimulator::get()->getTop5v(); }

void setTop5v(double top_5v) { FakeVelodyneSimulator::get()->setTop5v(top_5v); }

double getTop2_5v(void) { return FakeVelodyneSimulator::get()->getTop2_5v(); }

void setTop2_5v(double top_2_5v) { FakeVelodyneSimulator::get()->setTop2_5v(top_2_5v); }

double getTop3_3v() { return FakeVelodyneSimulator::get()->getTop3_3v(); }

void setTop3_3v(double top_3_3v) { FakeVelodyneSimulator::get()->setTop3_3v(top_3_3v); }

double getTop5vRaw(void) { return FakeVelodyneSimulator::get()->getTop5vRaw(); }

void setTop5vRaw(double top_5v_raw) { FakeVelodyneSimulator::get()->setTop5vRaw(top_5v_raw); }

double getTopVcc(void) { return FakeVelodyneSimulator::get()->getTopVcc(); }

void setTop5vVcc(double top_vcc) { FakeVelodyneSimulator::get()->setTop5vVcc(top_vcc); }

double getBottomIOut(void) { return FakeVelodyneSimulator::get()->getBottomIOut(); }

void setBottomIOut(double bot_i_out) { FakeVelodyneSimulator::get()->setBottomIOut(bot_i_out); }

double getBottomTemp(void) { return FakeVelodyneSimulator::get()->getBottomTemp(); }

void setBottomTemp(double bot_temp) { FakeVelodyneSimulator::get()->setBottomTemp(bot_temp); }

double getBottom1_2v(void) { return FakeVelodyneSimulator::get()->getBottom1_2v(); }

void setBottom1_2v(double bot_1_2v) { FakeVelodyneSimulator::get()->setBottom1_2v(bot_1_2v); }

double getBottom1_25v(void) { return FakeVelodyneSimulator::get()->getBottom1_25v(); }

void setBottom1_25v(double bot_1_25v) { FakeVelodyneSimulator::get()->setBottom1_25v(bot_1_25v); }

double getBottom2_5v(void) { return FakeVelodyneSimulator::get()->getBottom2_5v(); }

void setBottom2_5v(double bot_2_5v) { FakeVelodyneSimulator::get()->setBottom2_5v(bot_2_5v); }

double getBottom3_3v(void) { return FakeVelodyneSimulator::get()->getBottom3_3v(); }

void setBottom3_3v(double bot_3_3v) { FakeVelodyneSimulator::get()->setBottom3_3v(bot_3_3v); }

double getBottom5v(void) { return FakeVelodyneSimulator::get()->getBottom5v(); }

void setBottom5v(double bot_5v) { FakeVelodyneSimulator::get()->setBottom5v(bot_5v); }

double getBottomVIn(void) { return FakeVelodyneSimulator::get()->getBottomVIn(); }

void setBottomVIn(double bot_v_in) { FakeVelodyneSimulator::get()->setBottomVIn(bot_v_in); }

// Status
void setStatusJson(const char * path) { FakeVelodyneSimulator::get()->setStatusJson(path); }

const char * getStatusJson(void) { return FakeVelodyneSimulator::get()->getStatusJson(); }

void setMotorState(int motor_state) { FakeVelodyneSimulator::get()->setMotorState(motor_state); }

int getMotorState(void) { return FakeVelodyneSimulator::get()->getMotorState(); }

void setMotorRpm(int motor_rpm) { FakeVelodyneSimulator::get()->setMotorRpm(motor_rpm); }

int getMotorRpm(void) { return FakeVelodyneSimulator::get()->getMotorRpm(); }

void setLaserState(int laser_state) { FakeVelodyneSimulator::get()->setLaserState(laser_state); }

int getLaserState(void) { return FakeVelodyneSimulator::get()->getLaserState(); }

// Settings
void setSettingsJson(const char * path) { FakeVelodyneSimulator::get()->setSettingsJson(path); }

const char * getSettingsJson(void) { return FakeVelodyneSimulator::get()->getSettingsJson(); }

void setRpmSetting(int rpm_setting) { FakeVelodyneSimulator::get()->setRpmSetting(rpm_setting); }

int getRpmSetting(void) { return FakeVelodyneSimulator::get()->getRpmSetting(); }

#ifdef __cplusplus
}
#endif
