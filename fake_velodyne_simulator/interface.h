#ifndef FAKE_VELODYNE_SIMULATOR_INTERFACE_H_
#define FAKE_VELODYNE_SIMULATOR_INTERFACE_H_

/**
 * @file interface.h
 * @brief Interfacing to C++ code
 */

#include <linux/limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load data from ini file
 */
void loadIniFile(void);

/**
 * @brief Save data to ini file
 */
void saveIniFile(void);

// General
/**
 * @brief Set server address for saving it to ini file
 * @param [in] address server address
 */
void setAddress(const char * address);

/**
 * @brief Get server address stored in ini file
 * @return server address
 */
const char * getAddress(void);

/**
 * @brief Start HTTP server
 * @return 0 on success, otherwise error
 */
int start(void);

/**
 * @brief Stop HTTP server
 */
void stop(void);

/**
 * @brief Show debug output or not
 * @param [in] is_debug debug output or not
 */
void setDebugOutput(int is_debug);

// Info
/**
 * @brief Set path of info.json for saving it to ini file
 * @param [in] path path of info.json
 */
void setInfoJson(const char * path);

/**
 * @brief Get path of info.json stored in ini file
 * @return path of info.json
 */
const char * getInfoJson(void);

// Diagnostics
/**
 * @brief Set path of diag.json for saving it to ini file
 * @param [in] path path of diag.json
 */
void setDiagJson(const char * path);

/**
 * @brief Get path of diag.json stored in ini file
 * @return path of diag.json
 */
const char * getDiagJson(void);

/**
 * @brief Get the high voltage bias to the APD (Avalanche photodiode)
 * @return the high voltage bias to the APD (Avalanche photodiode)
 */
double getTopHv(void);

/**
 * @brief Set the high voltage bias to the APD (Avalanche photodiode)
 * @param [in] top_hv the high voltage bias to the APD (Avalanche photodiode)
 */
void setTopHv(double top_hv);

/**
 * @brief Get the temperature of the top board
 * @return the temperature of the top board
 */
double getTopTemp(void);

/**
 * @brief Set the temperature of the top board
 * @param [in] top_temp the temperature of the top board
 */
void setTopTemp(double top_temp);

/**
 * @brief Get the top board's 5.0 V rail
 * @return top the board's 5.0 V rail
 */
double getTop5v(void);

/**
 * @brief Set the top board's 5.0 V rail
 * @param [in] top_5v the top board's 5.0 V rail
 */
void setTop5v(double top_5v);

/**
 * @brief Get the top board's 2.5 V rail
 * @return the top board's 2.5 V rail
 */
double getTop2_5v(void);

/**
 * @brief Set the top board's 2.5 V rail
 * @param [in] top_2_5v the top board's 2.5 V rail
 */
void setTop2_5v(double top_2_5v);

/**
 * @brief Get the top board's 3.3 V rail
 * @return the top board's 3.3 V rail
 */
double getTop3_3v(void);

/**
 * @brief Set the top board's 3.3 V rail
 * @param [in] top_3_3v the top board's 3.3 V rail
 */
void setTop3_3v(double top_3_3v);

/**
 * @brief Get the top board's unregulated 5.0 V rail
 * @return the top board's unregulated 5.0 V rail
 */
double getTop5vRaw(void);

/**
 * @brief Set the top board's unregulated 5.0 V rail
 * @param [in] top_5v_raw the top board's unregulated 5.0 V rail
 */
void setTop5vRaw(double top_5v_raw);

/**
 * @brief Get the top board's 1.2 V rail
 * @return the top board's 1.2 V rail
 */
double getTopVcc(void);

/**
 * @brief Set the top board's 1.2 V rail
 * @param [in] top_vcc the top board's 1.2 V rail
 */
void setTop5vVcc(double top_vcc);

/**
 * @brief Get the sensor's input power line
 * @return the sensor's input power line
 */
double getBottomIOut(void);

/**
 * @brief Set the sensor's input power line
 * @param [in] bot_i_out the sensor's input power line
 */
void setBottomIOut(double bot_i_out);

/**
 * @brief Get the temperature of the bottom board
 * @return the temperature of the bottom board
 */
double getBottomTemp(void);

/**
 * @brief Set the temperature of the bottom board
 * @param [in] bot_temp the temperature of the bottom board
 */
void setBottomTemp(double bot_temp);

/**
 * @brief Get the bottom board's 1.2 V rail
 * @return the bottom board's 1.2 V rail
 */
double getBottom1_2v(void);

/**
 * @brief Set the bottom board's 1.2 V rail
 * @param [in] bot_1_2v the bottom board's 1.2 V rail
 */
void setBottom1_2v(double bot_1_2v);

/**
 * @brief Get the bottom board's 1.25 V rail
 * @return the bottom board's 1.25 V rail
 */
double getBottom1_25v(void);

/**
 * @brief Set the bottom board's 1.25 V rail
 * @param [in] bot_1_25v the bottom board's 1.25 V rail
 */
void setBottom1_25v(double bot_1_25v);

/**
 * @brief Get the bottom board's 2.5 V rail
 * @return bottom the board's 2.5 V rail
 */
double getBottom2_5v(void);

/**
 * @brief Set the bottom board's 2.5 V rail
 * @param [in] bot_2_5v the bottom board's 2.5 V rail
 */
void setBottom2_5v(double bot_2_5v);

/**
 * @brief Get the bottom board's 3.3 V rail
 * @return the bottom board's 3.3 V rail
 */
double getBottom3_3v(void);

/**
 * @brief Set the bottom board's 3.3 V rail
 * @param [in] bot_3_3v the bottom board's 3.3 V rail
 */
void setBottom3_3v(double bot_3_3v);

/**
 * @brief Get the bottom board's 5.0 V rail
 * @return the bottom board's 5.0 V rail
 */
double getBottom5v(void);

/**
 * @brief Set the bottom board's 5.0 V rail
 * @param [in] bot_5v the bottom board's 5.0 V rail
 */
void setBottom5v(double bot_5v);

/**
 * @brief Get the voltage of the input power
 * @return the voltage of the input power
 */
double getBottomVIn(void);

/**
 * @brief Set the voltage of the input power
 * @param [in] bot_v_in the voltage of the input power
 */
void setBottomVIn(double bot_v_in);

// Status
/**
 * @brief Set path of status.json for saving it to ini file
 * @param [in] path path of status.json
 */
void setStatusJson(const char * path);

/**
 * @brief Get path of status.json stored in ini file
 * @return path of status.json
 */
const char * getStatusJson(void);

/**
 * @brief Set motor state
 * @param [in] motor_state motor state
 */
void setMotorState(int motor_state);

/**
 * @brief Get motor state
 * @return motor state
 */
int getMotorState(void);

/**
 * @brief Set motor rpm
 * @param [in] motor_rpm motor rpm
 */
void setMotorRpm(int motor_rpm);

/**
 * @brief Get motor rpm
 * @return motor rpm
 */
int getMotorRpm(void);

/**
 * @brief Set laser state
 * @param [in] laser_state laser state
 */
void setLaserState(int laser_state);

/**
 * @brief Get laser state
 * @return laser state
 */
int getLaserState(void);

#ifdef __cplusplus
}
#endif

#endif  // FAKE_VELODYNE_SIMULATOR_INTERFACE_H_
