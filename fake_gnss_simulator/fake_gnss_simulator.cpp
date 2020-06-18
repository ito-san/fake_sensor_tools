/**
 * @file fake_gnss_simulator.cpp
 * @brief Fake IMU simulator class
 */

#include <fake_gnss_simulator.h>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/process.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace pt = boost::property_tree;

static constexpr int MAX_SIZE = 1024;
static constexpr int SLEEP_CNT_1MS = 1000;
static constexpr int ADJUST_COUNT = 130;

FakeGNSSSimulator * FakeGNSSSimulator::gnss_ = nullptr;

std::map<UBX_ID, FakeGNSSSimulator::HANDLE_FUNC> FakeGNSSSimulator::handle_map_ = {
  {{0x0A, 0x04}, &FakeGNSSSimulator::handleUbxMonVER},
  {{0x06, 0x00}, &FakeGNSSSimulator::handleUbxCfgPRT},
  {{0x06, 0x01}, &FakeGNSSSimulator::handleUbxCfgMSG},
};

std::map<UBX_ID, FakeGNSSSimulator::PERIODIC_TRANSMIT> FakeGNSSSimulator::periodic_map_ = {
  {{0x01, 0x03}, {&FakeGNSSSimulator::sendUbxNavSTATUS, 0, 0}},
  {{0x01, 0x07}, {&FakeGNSSSimulator::sendUbxNavPVT, 0, 0}},
  {{0x01, 0x3C}, {&FakeGNSSSimulator::sendUbxNavRELPOSNED, 0, 0}},
  {{0x0A, 0x09}, {&FakeGNSSSimulator::sendUbxMonHW, 0, 0}},
  {{0x0A, 0x36}, {&FakeGNSSSimulator::sendUbxMonCOMMS, 0, 0}},
};

std::map<int, PortBlock> FakeGNSSSimulator::port_blocks_ = {
  {PORT_ID_I2C, {false, 0}}, {PORT_ID_UART1, {false, 0}}, {PORT_ID_UART2, {false, 0}},
  {PORT_ID_USB, {true, 0}},  {PORT_ID_SPI, {false, 0}},
};

FakeGNSSSimulator::FakeGNSSSimulator()
: aStatus_(A_STATUS_OK),
  jammingState_(JAMMING_STATE_OK),
  portId_(PORT_ID_I2C),
  spoofDetState_(SPOOF_DET_STATE_NO_SPOOFING)
{
}

FakeGNSSSimulator * FakeGNSSSimulator::get(void)
{
  if (gnss_ == nullptr) {
    static FakeGNSSSimulator gnss;
    gnss_ = &gnss;
  }

  return gnss_;
}

void FakeGNSSSimulator::loadIniFile(void)
{
  auto env = boost::this_process::environment();
  ini_path_ = env["HOME"].to_string() + "/.config/fake_gnss_simulator.ini";

  if (!fs::exists(ini_path_)) return;

  pt::ptree pt;
  read_ini(ini_path_, pt);

  if (boost::optional<std::string> v = pt.get_optional<std::string>("device_name")) {
    const char * str = v.get().c_str();
    strncpy(device_name_, str, strlen(str));
  }
}

void FakeGNSSSimulator::saveIniFile(void)
{
  pt::ptree pt;

  pt.put("device_name", device_name_);

  write_ini(ini_path_, pt);
}

// General
void FakeGNSSSimulator::setDeviceName(const char * device_name)
{
  strncpy(device_name_, device_name, strlen(device_name));
}

const char * FakeGNSSSimulator::getDeviceName(void) const { return device_name_; }

int FakeGNSSSimulator::start(void)
{
  int ret = 0;

  // Preparation for a subsequent run() invocation
  io_.reset();
  port_ = boost::shared_ptr<as::serial_port>(new as::serial_port(io_));

  // Open the serial port using the specified device name
  try {
    port_->open(device_name_);
  } catch (const boost::system::system_error & e) {
    ret = ENOENT;
    std::cerr << e.what() << std::endl;
    return ret;
  }

  stop_thread_ = false;
  pthread_create(&th_, nullptr, &FakeGNSSSimulator::threadHelper, this);
  return ret;
}

void FakeGNSSSimulator::stop(void)
{
  pthread_mutex_lock(&mutex_stop_);
  stop_thread_ = true;
  pthread_mutex_unlock(&mutex_stop_);
  pthread_join(th_, NULL);

  io_.stop();
}

void FakeGNSSSimulator::setChecksumError(int is_error)
{
  pthread_mutex_lock(&mutex_send_);
  checksum_error_ = is_error;
  pthread_mutex_unlock(&mutex_send_);
}

void FakeGNSSSimulator::setDebugOutput(int is_debug)
{
  pthread_mutex_lock(&mutex_dump_);
  dump_ = is_debug;
  pthread_mutex_unlock(&mutex_dump_);
}

// UBX-MON-HW
void FakeGNSSSimulator::setAStatus(AStatus aStatus)
{
  pthread_mutex_lock(&mutex_send_);
  aStatus_ = aStatus;
  pthread_mutex_unlock(&mutex_send_);
}

AStatus FakeGNSSSimulator::getAStatus(void) const { return aStatus_; }

void FakeGNSSSimulator::setJammingState(JammingState jammingState)
{
  pthread_mutex_lock(&mutex_send_);
  jammingState_ = jammingState;
  pthread_mutex_unlock(&mutex_send_);
}

JammingState FakeGNSSSimulator::getJammingState(void) const { return jammingState_; }

void FakeGNSSSimulator::setPortId(PortId portId)
{
  pthread_mutex_lock(&mutex_send_);
  portId_ = portId;
  pthread_mutex_unlock(&mutex_send_);
}

PortId FakeGNSSSimulator::getPortId(void) const { return portId_; }

PortBlock FakeGNSSSimulator::getPortBlock(void) const { return port_blocks_[portId_]; }

void FakeGNSSSimulator::setPortEnabled(int port_enabled)
{
  pthread_mutex_lock(&mutex_send_);
  port_blocks_[portId_].port_enabled = port_enabled;
  pthread_mutex_unlock(&mutex_send_);
}

void FakeGNSSSimulator::setTxUsage(int tx_usage)
{
  pthread_mutex_lock(&mutex_send_);
  port_blocks_[portId_].tx_usage = tx_usage;
  pthread_mutex_unlock(&mutex_send_);
}

// UBX-NAV-STATUS
void FakeGNSSSimulator::setSpoofDetState(SpoofDetState spoofDetState)
{
  pthread_mutex_lock(&mutex_send_);
  spoofDetState_ = spoofDetState;
  pthread_mutex_unlock(&mutex_send_);
}

SpoofDetState FakeGNSSSimulator::getSpoofDetState(void) const { return spoofDetState_; }

void * FakeGNSSSimulator::thread(void)
{
  boost::thread thr_io(boost::bind(&as::io_service::run, &io_));

  // asynchronously read data
  uint8_t data[MAX_SIZE] = "";
  port_->async_read_some(
    as::buffer(data), boost::bind(
                        &FakeGNSSSimulator::onRead, this, as::placeholders::error,
                        as::placeholders::bytes_transferred, data));

  while (true) {
    bool b;
    pthread_mutex_lock(&mutex_stop_);
    b = stop_thread_;
    pthread_mutex_unlock(&mutex_stop_);
    if (b) break;

    // Handle periodic transmission
    handlePeriodicTransmit();
    usleep(SLEEP_CNT_1MS);
  }

  return nullptr;
}

void FakeGNSSSimulator::handlePeriodicTransmit(void)
{
  for (auto & p : periodic_map_) {
    if (p.second.rate_ > 0) {
      --p.second.cnt_;
      if (p.second.cnt_ <= 0) {
        (this->*(p.second.func_))();
        p.second.cnt_ = p.second.rate_;
      }
    }
  }
}

void FakeGNSSSimulator::dump(Direction dir, const uint8_t * data, std::size_t size)
{
  printf("%s ", (dir == Read) ? ">" : "<");

  for (std::size_t i = 0; i < size; ++i) {
    printf("%02X", data[i]);
    if (i + 1 <= size) printf(" ");
  }
  printf("\n");
}

void FakeGNSSSimulator::onRead(
  const boost::system::error_code & error, std::size_t bytes_transfered, const uint8_t * data)
{
  if (error) {
    std::cout << error.message() << std::endl;
  } else {
    bool b;
    pthread_mutex_lock(&mutex_dump_);
    b = dump_;
    pthread_mutex_unlock(&mutex_dump_);
    if (b) {
      dump(Read, data, bytes_transfered);
    }

    if (data[0] == 0xB5 && data[1] == 0x62) {
      handleUbx(data);
    }
    // asynchronously read data
    uint8_t next[MAX_SIZE] = "";
    port_->async_read_some(
      as::buffer(next), boost::bind(
                          &FakeGNSSSimulator::onRead, this, as::placeholders::error,
                          as::placeholders::bytes_transferred, next));
  }
}

void FakeGNSSSimulator::onWrite(
  const boost::system::error_code & error, std::size_t bytes_transfered,
  const std::vector<uint8_t> & data)
{
  bool b;
  pthread_mutex_lock(&mutex_dump_);
  b = dump_;
  pthread_mutex_unlock(&mutex_dump_);
  if (b) {
    dump(Write, &data[0], bytes_transfered);
  }
}

void FakeGNSSSimulator::handleUbx(const uint8_t * data)
{
  auto it = handle_map_.find(UBX_ID(data[2], data[3]));

  if (it != handle_map_.end()) {
    (this->*(it->second))(data);
  } else {
    // UBX-CFG-???
    if (data[2] == 0x06) {
      sendUbxAck(true, data[2], data[3]);
    }
  }
}

// UBX-MON-VER
void FakeGNSSSimulator::handleUbxMonVER(const uint8_t * data)
{
  uint8_t d[] = {
    0xB5, 0x62, 0x0A, 0x04, 0xDC, 0x00, 0x45, 0x58, 0x54, 0x20, 0x43, 0x4F, 0x52, 0x45, 0x20, 0x31,
    0x2E, 0x30, 0x30, 0x20, 0x28, 0x36, 0x31, 0x62, 0x32, 0x64, 0x64, 0x29, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x31, 0x39, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x52, 0x4F,
    0x4D, 0x20, 0x42, 0x41, 0x53, 0x45, 0x20, 0x30, 0x78, 0x31, 0x31, 0x38, 0x42, 0x32, 0x30, 0x36,
    0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x57, 0x56, 0x45,
    0x52, 0x3D, 0x48, 0x50, 0x47, 0x20, 0x31, 0x2E, 0x31, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x52, 0x4F, 0x54, 0x56, 0x45,
    0x52, 0x3D, 0x32, 0x37, 0x2E, 0x31, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4D, 0x4F, 0x44, 0x3D, 0x5A, 0x45, 0x44, 0x2D,
    0x46, 0x39, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x50, 0x53, 0x3B, 0x47, 0x4C, 0x4F, 0x3B, 0x47, 0x41,
    0x4C, 0x3B, 0x42, 0x44, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x51, 0x5A, 0x53, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xBD, 0x36};

  send(d, sizeof(d));
}

void FakeGNSSSimulator::handleUbxCfgPRT(const uint8_t * data)
{
  uint8_t d[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00,
                 0xC0, 0x08, 0x00, 0x00, 0x00, 0xE1, 0x00, 0x00, 0x07, 0x00,
                 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCD, 0xC3};

  send(d, sizeof(d));
}

void FakeGNSSSimulator::handleUbxCfgMSG(const uint8_t * data)
{
  auto it = periodic_map_.find(UBX_ID(data[6], data[7]));
  bool f = false;

  if (it != periodic_map_.end()) {
    it->second.rate_ = (SLEEP_CNT_1MS / data[8]) - ADJUST_COUNT;
    it->second.cnt_ = it->second.rate_;
    f = true;
  }

  sendUbxAck(f, data[2], data[3]);
}

void FakeGNSSSimulator::sendUbxAck(bool ack, uint8_t message_class, uint8_t message_id)
{
  uint8_t data[] = {0xB5, 0x62, 0x05, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
  data[3] = ack ? 0x01 : 0x00;
  data[6] = message_class;
  data[7] = message_id;

  send(data, sizeof(data));
}

void FakeGNSSSimulator::sendUbxNavSTATUS(void)
{
  uint8_t d[] = {0xB5, 0x62, 0x01, 0x03, 0x10, 0x00, 0x38, 0x4A, 0x79, 0x17, 0x03, 0xDD,
                 0x00, 0x00, 0xC4, 0x05, 0x00, 0x00, 0xE6, 0x1C, 0x09, 0x00, 0xDA, 0xF0};

  SpoofDetState s;
  pthread_mutex_lock(&mutex_send_);
  s = spoofDetState_;
  pthread_mutex_unlock(&mutex_send_);
  d[13] = s << 3;
  send(d, sizeof(d));
}

void FakeGNSSSimulator::sendUbxNavPVT(void)
{
  uint8_t d[] = {0xB5, 0x62, 0x01, 0x07, 0x5C, 0x00, 0x50, 0x32, 0x20, 0x17, 0xE3, 0x07, 0x0A,
                 0x11, 0x0B, 0x2E, 0x08, 0x37, 0x0F, 0x00, 0x00, 0x00, 0x2E, 0x3A, 0x01, 0x00,
                 0x03, 0x01, 0xEA, 0x0F, 0x46, 0x4E, 0x44, 0x04, 0xFE, 0x81, 0x90, 0x1E, 0x03,
                 0x9A, 0x03, 0x00, 0x46, 0xE4, 0x02, 0x00, 0xD3, 0x10, 0x00, 0x00, 0xB9, 0x1B,
                 0x00, 0x00, 0x89, 0xFF, 0xFF, 0xFF, 0xE5, 0xFF, 0xFF, 0xFF, 0x7C, 0xFF, 0xFF,
                 0xFF, 0x7A, 0x00, 0x00, 0x00, 0x63, 0xFD, 0xC9, 0x00, 0xFF, 0x01, 0x00, 0x00,
                 0x1C, 0x0D, 0x4B, 0x00, 0x8D, 0x00, 0x00, 0x00, 0xB8, 0x41, 0x47, 0x3D, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D, 0x7C};

  time_t nowt = time(nullptr);
  struct tm now;
  gmtime_r(&nowt, &now);
  uint16_t * year = reinterpret_cast<uint16_t *>(&d[10]);
  *year = now.tm_year + 1900;
  d[12] = now.tm_mon + 1;
  d[13] = now.tm_mday;
  d[14] = now.tm_hour;
  d[15] = now.tm_min;
  d[16] = now.tm_sec;

  send(d, sizeof(d));
}

void FakeGNSSSimulator::sendUbxNavRELPOSNED(void)
{
  uint8_t d[] = {0xB5, 0x62, 0x01, 0x3C, 0x40, 0x00, 0x01, 0x00, 0x00, 0x00, 0xB0, 0xAF,
                 0x47, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3C, 0x86};

  send(d, sizeof(d));
}

void FakeGNSSSimulator::sendUbxMonHW(void)
{
  uint8_t d[] = {0xB5, 0x62, 0x0A, 0x09, 0x3C, 0x00, 0x00, 0xC4, 0x01, 0x00, 0x00, 0x28, 0x00, 0x00,
                 0x00, 0x00, 0x01, 0x00, 0xEF, 0xC7, 0x01, 0x00, 0x78, 0x00, 0x5C, 0x0A, 0x02, 0x01,
                 0x00, 0x84, 0xFF, 0x7B, 0x01, 0x00, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x01, 0x00,
                 0x02, 0x03, 0xFF, 0x05, 0x11, 0x04, 0x13, 0xFF, 0x35, 0x08, 0x0F, 0x5E, 0x00, 0x00,
                 0x00, 0x00, 0x80, 0xEF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6A, 0xEB};

  AStatus a;
  JammingState j;
  pthread_mutex_lock(&mutex_send_);
  a = aStatus_;
  j = jammingState_;
  pthread_mutex_unlock(&mutex_send_);

  d[26] = a;
  d[28] = j << 2;
  send(d, sizeof(d));
}

void FakeGNSSSimulator::sendUbxMonCOMMS(void)
{
  std::vector<uint8_t> d{0xB5, 0x62, 0x0A, 0x36, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  for (const auto & p : port_blocks_) {
    uint8_t b[40] = {};
    if (p.second.port_enabled) {
      // nPorts
      ++d[7];
      uint16_t * portId = reinterpret_cast<uint16_t *>(&b[0]);
      *portId = p.first;
      b[8] = p.second.tx_usage;
      d.insert(d.end(), &b[0], &b[sizeof(b)]);
    }
  }

  uint16_t * length = reinterpret_cast<uint16_t *>(&d[4]);
  *length = d.size() - 6;
  d.resize(d.size() + 2);
  send(&d[0], d.size());
}

void FakeGNSSSimulator::send(uint8_t * data, int size)
{
  calculateChecksum(&data[2], size - 4, data[size - 2], data[size - 1]);
  std::vector<uint8_t> frame(data, data + size);

  bool b;
  pthread_mutex_lock(&mutex_send_);
  b = checksum_error_;
  pthread_mutex_unlock(&mutex_send_);

  if (b) {
    frame[frame.size() - 1] = '?';
    frame[frame.size() - 2] = '?';
  }

  // asynchronously write data
  port_->async_write_some(
    as::buffer(frame), boost::bind(
                         &FakeGNSSSimulator::onWrite, this, as::placeholders::error,
                         as::placeholders::bytes_transferred, frame));
}

void FakeGNSSSimulator::calculateChecksum(
  const uint8_t * data, int size, uint8_t & ck_a, uint8_t & ck_b)
{
  ck_a = 0;
  ck_b = 0;
  for (int i = 0; i < size; ++i) {
    ck_a = ck_a + data[i];
    ck_b = ck_b + ck_a;
  }
}
