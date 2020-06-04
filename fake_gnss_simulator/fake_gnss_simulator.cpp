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
static constexpr int PVT_SIZE = 100;

FakeGNSSSimulator * FakeGNSSSimulator::gnss_ = nullptr;

FakeGNSSSimulator::FakeGNSSSimulator() {}

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

  if (boost::optional<std::string> v = pt.get_optional<std::string>("ubx_log_file")) {
    const char * str = v.get().c_str();
    strncpy(ubx_log_file_, str, strlen(str));
  }

  if (boost::optional<std::string> v = pt.get_optional<std::string>("ubx_pvt_log_file")) {
    const char * str = v.get().c_str();
    strncpy(ubx_pvt_log_file_, str, strlen(str));
  }

  if (boost::optional<std::string> v = pt.get_optional<std::string>("nmea_log_file")) {
    const char * str = v.get().c_str();
    strncpy(nmea_log_file_, str, strlen(str));
  }
}

void FakeGNSSSimulator::saveIniFile(void)
{
  pt::ptree pt;

  pt.put("device_name", device_name_);
  pt.put("ubx_log_file", ubx_log_file_);
  pt.put("ubx_pvt_log_file", ubx_pvt_log_file_);
  pt.put("nmea_log_file", nmea_log_file_);

  write_ini(ini_path_, pt);
}

void FakeGNSSSimulator::setDeviceName(const char * device_name)
{
  strncpy(device_name_, device_name, strlen(device_name));
}

const char * FakeGNSSSimulator::getDeviceName(void) { return device_name_; }

void FakeGNSSSimulator::setUBXLogFile(const char * ubx_log_file)
{
  strncpy(ubx_log_file_, ubx_log_file, strlen(ubx_log_file));
}

const char * FakeGNSSSimulator::getUBXLogFile(void) { return ubx_log_file_; }

void FakeGNSSSimulator::setUBXPVTLogFile(const char * ubx_pvt_log_file)
{
  strncpy(ubx_pvt_log_file_, ubx_pvt_log_file, strlen(ubx_pvt_log_file));
}

const char * FakeGNSSSimulator::getUBXPVTLogFile(void) { return ubx_pvt_log_file_; }

void FakeGNSSSimulator::setNMEALogFile(const char * nmea_log_file)
{
  strncpy(nmea_log_file_, nmea_log_file, strlen(nmea_log_file));
}

const char * FakeGNSSSimulator::getNMEALogFile(void) { return nmea_log_file_; }

int FakeGNSSSimulator::start(void)
{
  int ret = 0;

  if (!fs::exists(ubx_log_file_)) {
    ret = ENOENT;
    std::cerr << strerror(ret) << std::endl;
    return ret;
  }

  ifs_nmea_.open(nmea_log_file_, std::ios::in | std::ios::binary);
  if (!ifs_nmea_) {
    ret = ENOENT;
    std::cerr << strerror(ret) << std::endl;
    return ret;
  }

  ifs_pvt_.open(ubx_pvt_log_file_, std::ios::in | std::ios::binary);
  if (!ifs_pvt_) {
    ret = ENOENT;
    std::cerr << strerror(ret) << std::endl;
    return ret;
  }

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

  current_time_ = 0;
  stop_thread_ = false;
  pthread_create(&th_, nullptr, &FakeGNSSSimulator::threadHelper, this);
  pthread_create(&th_pvt_, nullptr, &FakeGNSSSimulator::pvt_threadHelper, this);
  return ret;
}

void FakeGNSSSimulator::stop(void)
{
  pthread_mutex_lock(&mutex_stop_);
  stop_thread_ = true;
  pthread_mutex_unlock(&mutex_stop_);
  pthread_join(th_, NULL);
  pthread_join(th_pvt_, NULL);

  io_.stop();
  ifs_nmea_.close();
  ifs_pvt_.close();
}

void FakeGNSSSimulator::setChecksumError(int is_error)
{
  pthread_mutex_lock(&mutex_error_);
  checksum_error_ = is_error;
  pthread_mutex_unlock(&mutex_error_);
}

void FakeGNSSSimulator::setDebugOutput(int is_debug)
{
  pthread_mutex_lock(&mutex_debug_);
  debug_output_ = is_debug;
  pthread_mutex_unlock(&mutex_debug_);
}

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

    // Send NMEA messages
    sendNMEA();
  }

  return nullptr;
}

void * FakeGNSSSimulator::pvt_thread(void)
{
  while (true) {
    bool b;
    pthread_mutex_lock(&mutex_stop_);
    b = stop_thread_;
    pthread_mutex_unlock(&mutex_stop_);
    if (b) break;

    // Send UBX PVT message
    std::vector<uint8_t> frame;

    frame.resize(PVT_SIZE);
    ifs_pvt_.read(reinterpret_cast<char *>(&frame[0]), frame.size());
    if (ifs_pvt_.eof()) {
      ifs_pvt_.clear();
      ifs_pvt_.seekg(0, std::ios_base::beg);
      ifs_pvt_.read(reinterpret_cast<char *>(&frame[0]), frame.size());
    }

    time_t nowt = time(nullptr);
    struct tm now;
    gmtime_r(&nowt, &now);
    int * year = reinterpret_cast<int *>(&frame[10]);
    *year = now.tm_year + 1900;
    frame[12] = now.tm_mon + 1;
    frame[13] = now.tm_mday;
    frame[14] = now.tm_hour;
    frame[15] = now.tm_min;
    frame[16] = now.tm_sec;

    pthread_mutex_lock(&mutex_error_);
    b = checksum_error_;
    pthread_mutex_unlock(&mutex_error_);

    if (!b) {
      calculateChecksum(&frame[2], 96, frame[98], frame[99]);
    } else {
      frame[frame.size() - 1] = '?';
      frame[frame.size() - 2] = '?';
    }

    port_->write_some(as::buffer(frame));
    pthread_mutex_lock(&mutex_debug_);
    b = debug_output_;
    pthread_mutex_unlock(&mutex_debug_);
    if (b) {
      dump(Write, reinterpret_cast<const uint8_t *>(&frame[0]), frame.size());
    }

    // 5Hz
    usleep(1000000 / 5);
  }

  return nullptr;
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
    pthread_mutex_lock(&mutex_debug_);
    b = debug_output_;
    pthread_mutex_unlock(&mutex_debug_);
    if (b) {
      dump(Read, data, bytes_transfered);
    }

    if (data[0] == 0xB5 && data[1] == 0x62) {
      handleUBX(data);
    }
    // asynchronously read data
    uint8_t next[MAX_SIZE] = "";
    port_->async_read_some(
      as::buffer(next), boost::bind(
                          &FakeGNSSSimulator::onRead, this, as::placeholders::error,
                          as::placeholders::bytes_transferred, next));
  }
}

void FakeGNSSSimulator::handleUBX(const uint8_t * data)
{
  fs::ifstream ifs;
  ifs.open(ubx_log_file_, std::ios::in | std::ios::binary);
  if (!ifs) {
    return;
  }

  while (!ifs.eof()) {
    std::vector<uint8_t> frame;
    uint16_t length;
    bool b;

    frame.resize(6);
    ifs.read(reinterpret_cast<char *>(&frame[0]), 6);
    length = frame[5] << 8 | frame[4];

    frame.resize(6 + length + 2);
    ifs.read(reinterpret_cast<char *>(&frame[6]), length + 2);

    // Same message class, message id
    if (data[2] == frame[2] && data[3] == frame[3]) {
      pthread_mutex_lock(&mutex_error_);
      b = checksum_error_;
      pthread_mutex_unlock(&mutex_error_);

      if (b) {
        frame[frame.size() - 1] = '?';
        frame[frame.size() - 2] = '?';
      }
      // asynchronously write data
      port_->async_write_some(
        as::buffer(frame), boost::bind(
                             &FakeGNSSSimulator::onWrite, this, as::placeholders::error,
                             as::placeholders::bytes_transferred, frame));
      break;
    }
  }

  ifs.close();

  // UBX-CFG
  if (data[2] == 0x06) {
    sendACK(data[2], data[3]);
  }
}

void FakeGNSSSimulator::sendACK(uint8_t message_class, uint8_t message_id)
{
  uint8_t ack[] = {0xB5, 0x62, 0x05, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
  ack[6] = message_class;
  ack[7] = message_id;
  calculateChecksum(&ack[2], 6, ack[8], ack[9]);
  std::vector<uint8_t> frame(ack, ack + 10);

  bool b;
  pthread_mutex_lock(&mutex_error_);
  b = checksum_error_;
  pthread_mutex_unlock(&mutex_error_);

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

void FakeGNSSSimulator::onWrite(
  const boost::system::error_code & error, std::size_t bytes_transfered,
  const std::vector<uint8_t> & data)
{
  bool b;
  pthread_mutex_lock(&mutex_debug_);
  b = debug_output_;
  pthread_mutex_unlock(&mutex_debug_);
  if (b) {
    dump(Write, &data[0], bytes_transfered);
  }
}

void FakeGNSSSimulator::sendNMEA(void)
{
  std::string line;
  std::getline(ifs_nmea_, line);
  if (ifs_nmea_.eof()) {
    current_time_ = 0;
    ifs_nmea_.clear();
    ifs_nmea_.seekg(0, std::ios_base::beg);
    std::getline(ifs_nmea_, line);
  }
  line += "\n";

  waitSendingNMEA(line);

  bool b;
  pthread_mutex_lock(&mutex_error_);
  b = checksum_error_;
  pthread_mutex_unlock(&mutex_error_);

  if (b) {
    line[line.length() - 3] = '?';
    line[line.length() - 4] = '?';
  }

  port_->write_some(as::buffer(line));
  pthread_mutex_lock(&mutex_debug_);
  b = debug_output_;
  pthread_mutex_unlock(&mutex_debug_);
  if (b) {
    dump(Write, reinterpret_cast<const uint8_t *>(line.c_str()), line.length());
  }
}

void FakeGNSSSimulator::waitSendingNMEA(const std::string & data)
{
  std::string id = data.substr(1, 5);

  if (id == "GNRMC" || id == "GNGGA") {
    std::string time_str = data.substr(7, 6);
    struct tm tm;
    if (strptime(time_str.c_str(), "%H%M%S", &tm) != nullptr) {
      time_t nowt = time(nullptr);
      struct tm * now = std::localtime(&nowt);
      tm.tm_year = now->tm_year;
      tm.tm_mon = now->tm_mon;
      tm.tm_mday = now->tm_mday;
      time_t t_ms = mktime(&tm);

      if (current_time_ > 0) {
        time_t sub = t_ms - current_time_;
        if (sub > 0) {
          sleep(1);
        }
      }
      current_time_ = t_ms;
    }
  }
}
