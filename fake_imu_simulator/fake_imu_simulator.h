#ifndef FAKE_IMU_SIMULATOR_FAKE_IMU_SIMULATOR_H_
#define FAKE_IMU_SIMULATOR_FAKE_IMU_SIMULATOR_H_

/**
 * @file fake_imu_simulator.h
 * @brief Fake IMU simulator definitions
 */

#include <linux/limits.h>
#include <string>

class FakeIMUSimulator
{
public:
  static FakeIMUSimulator * get();

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

private:
  /**
   * @brief Constructor
   */
  FakeIMUSimulator();

  /**
   * @brief Thread helper funcion
   * @param[in] arg argument
   */
  static void * threadHelper(void * arg)
  {
    return reinterpret_cast<FakeIMUSimulator *>(arg)->thread();
  }

  /**
   * @brief Thread loop
   * @return nullptr
   */
  void * thread(void);

  /**
   * @brief Dump sent/received Data
   * @param[in] data pointer to data
   */
  void dump(const uint8_t * data);

  static FakeIMUSimulator * imu_;  //!< @brief reference to itself
  std::string ini_path_;           //!< @brief path to ini file
  char device_name_[PATH_MAX];     //!< @brief Device name
  char log_file_[PATH_MAX];        //!< @brief log file

  bool stop_thread_;             //!< @brief flag to stop thread
  bool checksum_error_;          //!< @brief flag to generate checksum error occur or not
  bool debug_output_;            //!< @brief flag to show debug output or not
  pthread_mutex_t mutex_stop_;   //!< @brief mutex to protect access to stop_thread
  pthread_mutex_t mutex_error_;  //!< @brief mutex to protect access to checksum_error
  pthread_mutex_t mutex_debug_;  //!< @brief mutex to protect access to debug_output
  pthread_t th_;                 //!< @brief thread handle

  int fd_;  //!< @brief file descriptor to serial port
};

#endif  // FAKE_IMU_SIMULATOR_FAKE_IMU_SIMULATOR_H_
