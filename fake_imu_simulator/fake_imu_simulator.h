#ifndef FAKKE_IMU_SIMULATOR_H
#define FAKKE_IMU_SIMULATOR_H

/**
 * @file fake_imu_simulator.h
 * @brief Fake IMU simulator definitions
 */

#include <linux/limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ini file data
 */
typedef struct
{
  char device_name[PATH_MAX];  //!< @brief Device name
  char log_file[PATH_MAX];     //!< @brief Log file
} INI;

/**
 * @brief Load data from ini file
 * @param [out] ini pointer to ini file data
 */
void loadIniFile(INI * ini);

/**
 * @brief Save data to ini file
 * @param [in] ini pointer to ini file data
 */
void saveIniFile(INI * ini);

/**
 * @brief Start serial port communication
 * @param [in] ini pointer to ini file data
 * @return 0 on success, otherwise error
 */
int start(INI * ini);

/**
 * @brief Stop serial port communication
 */
void stop();

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

#endif  // FAKKE_IMU_SIMULATOR_H
