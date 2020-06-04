#ifndef FAKE_IMU_SIMULATOR_INTERFACE_H_
#define FAKE_IMU_SIMULATOR_INTERFACE_H_

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

/**
 * @brief Set device name for saving it to ini file
 * @param [in] device_name device name
 */
void setDeviceName(const char * device_name);

/**
 * @brief Get device name stored in ini file
 * @return device name
 */
const char * getDeviceName(void);

/**
 * @brief Set path of log file for saving it to ini file
 * @param [in] log_file path of log file
 */
void setLogFile(const char * log_file);

/**
 * @brief Get path of log file stored in ini file
 * @return path of log file
 */
const char * getLogFile(void);

/**
 * @brief Start serial port communication
 * @return 0 on success, otherwise error
 */
int start(void);

/**
 * @brief Stop serial port communication
 */
void stop(void);

/**
 * @brief Generate checksum error or not
 * @param [in] is_error generate checksum error or not
 */
void setChecksumError(int is_error);

/**
 * @brief Show debug output or not
 * @param [in] is_debug debug output or not
 */
void setDebugOutput(int is_debug);

#ifdef __cplusplus
}
#endif

#endif  // FAKE_IMU_SIMULATOR_INTERFACE_H_
