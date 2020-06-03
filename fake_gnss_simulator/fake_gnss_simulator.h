#ifndef FAKE_GNSS_SIMULATOR_FAKE_GNSS_SIMULATOR_H_
#define FAKE_GNSS_SIMULATOR_FAKE_GNSS_SIMULATOR_H_

/**
 * @file fake_gnss_simulator.h
 * @brief Fake IMU simulator class
 */

#include <linux/limits.h>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>

namespace as = boost::asio;
namespace fs = boost::filesystem;

class FakeGnssSimulator
{
public:
  static FakeGnssSimulator * get();

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
   * @brief Set path of UBX log file for saving it to ini file
   * @param [in] ubx_log_file path of UBX log file
   */
  void setUBXLogFile(const char * ubx_log_file);

  /**
   * @brief Get path of UBX log file stored in ini file
   * @return path of UBX log file
   */
  const char * getUBXLogFile(void);

  /**
   * @brief Set path of NMEA log file for saving it to ini file
   * @param [in] nmea_log_file path of NMEA log file
   */
  void setNMEALogFile(const char * nmea_log_file);

  /**
   * @brief Get path of NMEA log file stored in ini file
   * @return path of NMEA log file
   */
  const char * getNMEALogFile(void);

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
   * @brief io direction
   */
  enum Direction {
    Read = 0,
    Write,
  };

  /**
   * @brief Constructor
   */
  FakeGnssSimulator();

  /**
   * @brief Thread helper funcion
   * @param[in] arg argument
   */
  static void * threadHelper(void * arg)
  {
    return reinterpret_cast<FakeGnssSimulator *>(arg)->thread();
  }

  /**
   * @brief Thread loop
   * @return nullptr
   */
  void * thread();

  /**
   * @brief Dump sent/received Data
   */
  void dump(Direction dir, const uint8_t * data, std::size_t size);

  /**
   * @brief Handler to be called when the read operation completes
   * @param[in] error error argument of a handler
   * @param[in] bytes_transfered bytes transferred argument of a handler
   * @param[inout] data received data
   */
  void onRead(
    const boost::system::error_code & error, std::size_t bytes_transfered, const uint8_t * data);

  /**
   * @brief Handle UBX data
   * @param[in] data received data
   */
  void handleUBX(const uint8_t * data);

  /**
   * @brief Handler to be called when the write operation completes
   * @param[in] error error argument of a handler
   * @param[in] bytes_transfered bytes transferred argument of a handler
   * @param[inout] data sent data
   */
  void onWrite(
    const boost::system::error_code & error, std::size_t bytes_transfered,
    const std::vector<uint8_t> & data);

  /**
   * @brief Send UBX-ACK-ACK
   * @param[in] message_class message class
   * @param[in] message_id message id
   */
  void sendACK(uint8_t message_class, uint8_t message_id);

  /**
   * @brief Calculate checksum
   * @param data start of rangeint
   * @param size size of range
   * @param ck_a checksum a
   * @param ck_b checksum b
   */
  void calculateChecksum(const uint8_t * data, int size, uint8_t & ck_a, uint8_t & ck_b);

  /**
   * @brief Send NMEA messages
   */
  void sendNMEA(void);

  /**
   * @brief Wait for sending NMEA messages
   * @param data data to send
   */
  void waitSendingNMEA(const std::string & data);

  static FakeGnssSimulator * gnss_;  //!< @brief reference to itself
  std::string ini_path_;             //!< @brief path to ini file
  char device_name_[PATH_MAX];       //!< @brief Device name
  char ubx_log_file_[PATH_MAX];      //!< @brief UBX protcol log file
  char nmea_log_file_[PATH_MAX];     //!< @brief NMEA protocol log file

  bool stop_thread_;             //!< @brief flag to stop thread
  bool checksum_error_;          //!< @brief flag to generate checksum error occur or not
  bool debug_output_;            //!< @brief flag to show debug output or not
  pthread_mutex_t mutex_stop_;   //!< @brief mutex to protect access to stop_thread
  pthread_mutex_t mutex_error_;  //!< @brief mutex to protect access to checksum_error
  pthread_mutex_t mutex_debug_;  //!< @brief mutex to protect access to debug_output
  pthread_t th_;                 //!< @brief thread handle

  as::io_service io_;                        //!< @brief facilities of custom asynchronous services
  boost::shared_ptr<as::serial_port> port_;  //!< @brief wrapper over serial port functionality

  fs::ifstream ifs_nmea_;  //!< @brief Input stream to operate on nmea log file
  time_t current_time_;    //!< @brief Current local time
};

#endif  // FAKE_GNSS_SIMULATOR_FAKE_GNSS_SIMULATOR_H_
