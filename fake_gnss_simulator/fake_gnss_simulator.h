#ifndef FAKE_GNSS_SIMULATOR_FAKE_GNSS_SIMULATOR_H_
#define FAKE_GNSS_SIMULATOR_FAKE_GNSS_SIMULATOR_H_

/**
 * @file fake_gnss_simulator.h
 * @brief Fake IMU simulator class
 */

#include <defines.h>
#include <linux/limits.h>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <map>
#include <string>
#include <vector>

/**
 * @brief UBX Message Class and ID
 */
struct UBX_ID
{
  uint8_t classId_;
  uint8_t messageId_;

  UBX_ID() : classId_(0), messageId_(0) {}

  UBX_ID(uint8_t classId, uint8_t messageId) : classId_(classId), messageId_(messageId) {}

  bool operator<(const UBX_ID & value) const
  {
    if (classId_ == value.classId_) {
      return messageId_ < value.messageId_;
    }
    return classId_ < value.classId_;
  }
};

namespace as = boost::asio;
namespace fs = boost::filesystem;

class FakeGNSSSimulator
{
public:
  static FakeGNSSSimulator * get();

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
  const char * getDeviceName(void) const;

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
  AStatus getAStatus(void) const;

  /**
   * @brief Set jammingState
   * @param [in] jammingState jammingState
   */
  void setJammingState(JammingState jammingState);

  /**
   * @brief Get jammingState
   * @return jammingState
   */
  JammingState getJammingState(void) const;

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
  PortId getPortId(void) const;

  /**
   * @brief Get port block
   * @return Port block
   */
  PortBlock getPortBlock(void) const;

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
  SpoofDetState getSpoofDetState(void) const;

private:
  /**
   * @brief io direction
   */
  enum Direction {
    Read = 0,
    Write,
  };

  typedef void (FakeGNSSSimulator::*HANDLE_FUNC)(const uint8_t * data);  //!< @brief message handler
  typedef void (FakeGNSSSimulator::*TRANSMIT_FUNC)();  //!< @brief transmit function

  /**
   * @brief Periodic transmission
   */
  typedef struct
  {
    TRANSMIT_FUNC func_;
    int rate_;
    int cnt_;
  } PERIODIC_TRANSMIT;

  /**
   * @brief Constructor
   */
  FakeGNSSSimulator();

  /**
   * @brief Thread helper funcion
   * @param[in] arg argument
   */
  static void * threadHelper(void * arg)
  {
    return reinterpret_cast<FakeGNSSSimulator *>(arg)->thread();
  }

  /**
   * @brief Thread loop
   * @return nullptr
   */
  void * thread(void);

  /**
   * @brief Handle periodic transmission
   */
  void handlePeriodicTransmit(void);

  /**
   * @brief Dump sent/received Data
   * @param[in] dir io direction
   * @param[in] data pointer to data
   * @param[in] size size of data
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
   * @brief Handler to be called when the write operation completes
   * @param[in] error error argument of a handler
   * @param[in] bytes_transfered bytes transferred argument of a handler
   * @param[inout] data sent data
   */
  void onWrite(
    const boost::system::error_code & error, std::size_t bytes_transfered,
    const std::vector<uint8_t> & data);

  /**
   * @brief Handle UBX data
   * @param[in] data received data
   */
  void handleUbx(const uint8_t * data);

  /**
   * @brief Handle UBX-MON-VER
   * @param[in] data received data
   */
  void handleUbxMonVER(const uint8_t * data);

  /**
   * @brief Handle UBX-CFG-PRT
   * @param[in] data received data
   */
  void handleUbxCfgPRT(const uint8_t * data);

  /**
   * @brief Handle UBX-CFG-MSG
   * @param[in] data received data
   */
  void handleUbxCfgMSG(const uint8_t * data);

  /**
   * @brief Send UBX-ACK-ACK
   * @param[in] ack true on UBX-ACK-ACK, false on UBX-ACK-NAK
   * @param[in] message_class message class
   * @param[in] message_id message id
   */
  void sendUbxAck(bool ack, uint8_t message_class, uint8_t message_id);

  /**
   * @brief Send UBX-NAV-STATUS
   */
  void sendUbxNavSTATUS(void);

  /**
   * @brief Send UBX-NAV-PVT
   */
  void sendUbxNavPVT(void);

  /**
   * @brief Send UBX-NAV-RELPOSNED
   */
  void sendUbxNavRELPOSNED(void);

  /**
   * @brief Send UBX-MON-HW
   */
  void sendUbxMonHW(void);

  /**
   * @brief Send UBX-MON-COMMS
   */
  void sendUbxMonCOMMS(void);

  /**
   * @brief Send cdata
   * @param data start of range
   * @param size size of range
   */
  void send(uint8_t * data, int size);

  /**
   * @brief Calculate checksum
   * @param data start of range
   * @param size size of range
   * @param ck_a checksum a
   * @param ck_b checksum b
   */
  void calculateChecksum(const uint8_t * data, int size, uint8_t & ck_a, uint8_t & ck_b);

  static FakeGNSSSimulator * gnss_;          //!< @brief reference to itself
  std::string ini_path_;                     //!< @brief path to ini file
  as::io_service io_;                        //!< @brief facilities of custom asynchronous services
  boost::shared_ptr<as::serial_port> port_;  //!< @brief wrapper over serial port functionality
  pthread_mutex_t mutex_stop_;               //!< @brief mutex to protect access to stop_thread
  pthread_mutex_t mutex_send_;  //!< @brief mutex to protect access to data regarding send data
  pthread_mutex_t mutex_dump_;  //!< @brief mutex to protect access to dump flag
  pthread_t th_;                //!< @brief thread handle
  static std::map<UBX_ID, HANDLE_FUNC> handle_map_;          //!< @brief message handler map
  static std::map<UBX_ID, PERIODIC_TRANSMIT> periodic_map_;  //!< @brief Periodic transmission map

  // General
  char device_name_[PATH_MAX];  //!< @brief Device name
  bool stop_thread_;            //!< @brief flag to stop thread
  bool checksum_error_;         //!< @brief flag to generate checksum error occur or not
  bool dump_;                   //!< @brief flag to show debug output or not

  // UBX-MON-HW
  AStatus aStatus_;            //!< @brief Status of the antenna supervisor state machine
  JammingState jammingState_;  //!< @brief output from Jamming/Interference Monitor

  // UBX-MON-COMMS
  PortId portId_;                                //!< @brief current portId
  static std::map<int, PortBlock> port_blocks_;  //!< @brief Port blocks

  // UBX-NAV-STATUS
  SpoofDetState spoofDetState_;  //!< @brief Spoofing detection state
};

#endif  // FAKE_GNSS_SIMULATOR_FAKE_GNSS_SIMULATOR_H_
