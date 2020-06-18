#ifndef FAKE_GNSS_SIMULATOR_INTERFACE_H_
#define FAKE_GNSS_SIMULATOR_INTERFACE_H_

/**
 * @file interface.h
 * @brief Interfacing to C++ code
 */

#include <defines.h>
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

// UBX-MON-HW
/**
 * @brief Set aStatus
 * @param [in] aStatus aStatus
 */
void setAStatus(AStatus aStatus);

/**
 * @brief Get aStatus
 * @return aStatus
 */
AStatus getAStatus(void);

/**
 * @brief Set jammingState
 * @param [in] jammingState jammingState
 */
void setJammingState(JammingState jammingState);

/**
 * @brief Get jammingState
 * @return jammingState
 */
JammingState getJammingState(void);

// UBX-MON-COMMS
/**
 * @brief Set port id
 * @param [in] portId portId
 */
void setPortId(PortId portId);

/**
 * @brief Get port id
 * @return Port block
 */
PortId getPortId(void);

/**
 * @brief Get port block
 * @return Port block
 */
PortBlock getPortBlock(void);

/**
 * @brief Set port enabled
 * @param [in] port_enabled port enabled
 */
void setPortEnabled(int port_enabled);

/**
 * @brief Set txUsage
 * @param [in] tx_usage txUsage
 */
void setTxUsage(int tx_usage);

// UBX-NAV-STATUS
/**
 * @brief Set spoofDetState
 * @param [in] spoofDetState spoofDetState
 */
void setSpoofDetState(SpoofDetState spoofDetState);

/**
 * @brief Get spoofDetState
 * @return spoofDetState
 */
SpoofDetState getSpoofDetState(void);

#ifdef __cplusplus
}
#endif

#endif  // FAKE_GNSS_SIMULATOR_INTERFACE_H_
