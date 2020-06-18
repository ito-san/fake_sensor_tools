#include <fake_imu_simulator.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/process.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

static constexpr int MAX_SIZE = 1024;
static constexpr int MAX_BIN_SIZE = 58;

FakeIMUSimulator * FakeIMUSimulator::imu_ = nullptr;

FakeIMUSimulator::FakeIMUSimulator() : bin_req_(false) {}

FakeIMUSimulator * FakeIMUSimulator::get(void)
{
  if (imu_ == nullptr) {
    static FakeIMUSimulator imu;
    imu_ = &imu;
  }

  return imu_;
}

void FakeIMUSimulator::loadIniFile(void)
{
  auto env = boost::this_process::environment();
  ini_path_ = env["HOME"].to_string() + "/.config/fake_imu_simulator.ini";

  if (!fs::exists(ini_path_)) return;

  pt::ptree pt;
  read_ini(ini_path_, pt);

  if (boost::optional<std::string> v = pt.get_optional<std::string>("device_name")) {
    const char * str = v.get().c_str();
    strncpy(device_name_, str, strlen(str));
  }

  if (boost::optional<std::string> v = pt.get_optional<std::string>("log_file")) {
    const char * str = v.get().c_str();
    strncpy(log_file_, str, strlen(str));
  }
}

void FakeIMUSimulator::saveIniFile(void)
{
  pt::ptree pt;

  pt.put("device_name", device_name_);
  pt.put("log_file", log_file_);

  write_ini(ini_path_, pt);
}

void FakeIMUSimulator::setDeviceName(const char * device_name)
{
  strncpy(device_name_, device_name, strlen(device_name));
}

const char * FakeIMUSimulator::getDeviceName(void) { return device_name_; }

void FakeIMUSimulator::setLogFile(const char * log_file)
{
  strncpy(log_file_, log_file, strlen(log_file));
}

const char * FakeIMUSimulator::getLogFile(void) { return log_file_; }

int FakeIMUSimulator::start(void)
{
  int ret = 0;

  if (!fs::exists(log_file_)) {
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

  bin_req_ = false;
  stop_thread_ = false;
  pthread_create(&th_, nullptr, &FakeIMUSimulator::threadHelper, this);
  return ret;
}

void FakeIMUSimulator::stop()
{
  pthread_mutex_lock(&mutex_stop_);
  stop_thread_ = true;
  pthread_mutex_unlock(&mutex_stop_);
  pthread_join(th_, NULL);

  io_.stop();
}

void FakeIMUSimulator::setChecksumError(int is_error)
{
  pthread_mutex_lock(&mutex_error_);
  checksum_error_ = is_error;
  pthread_mutex_unlock(&mutex_error_);
}

void FakeIMUSimulator::setDebugOutput(int is_debug)
{
  pthread_mutex_lock(&mutex_dump_);
  dump_ = is_debug;
  pthread_mutex_unlock(&mutex_dump_);
}

void * FakeIMUSimulator::thread(void)
{
  boost::thread thr_io(boost::bind(&as::io_service::run, &io_));

  // asynchronously read data
  uint8_t data[MAX_SIZE] = "";
  port_->async_read_some(
    as::buffer(data), boost::bind(
                        &FakeIMUSimulator::onRead, this, as::placeholders::error,
                        as::placeholders::bytes_transferred, data));

  fs::ifstream ifs(log_file_, std::ios::in | std::ios::binary);
  if (!ifs) {
    return nullptr;
  }

  while (true) {
    bool b;
    pthread_mutex_lock(&mutex_stop_);
    b = stop_thread_;
    pthread_mutex_unlock(&mutex_stop_);
    if (b) break;

    if (bin_req_) {
      uint8_t data[MAX_BIN_SIZE] = {};
      int len = sizeof(data);
      ifs.read(reinterpret_cast<char *>(data), len);
      if (ifs.eof()) {
        ifs.clear();
        ifs.seekg(0, std::ios_base::beg);
        ifs.read(reinterpret_cast<char *>(data), len);
      }

      pthread_mutex_lock(&mutex_error_);
      b = checksum_error_;
      pthread_mutex_unlock(&mutex_error_);

      if (b) {
        data[len - 3] = '?';
        data[len - 4] = '?';
      }

      std::vector<uint8_t> frame(data, data + len);
      // asynchronously write data
      port_->async_write_some(
        as::buffer(frame), boost::bind(
                             &FakeIMUSimulator::onWrite, this, as::placeholders::error,
                             as::placeholders::bytes_transferred, frame));
    }

    // 30Hz
    usleep(1000000 / 30);
  }

  ifs.close();

  return nullptr;
}

void FakeIMUSimulator::dump(Direction dir, const uint8_t * data, std::size_t size)
{
  printf("%s ", (dir == Read) ? ">" : "<");

  for (std::size_t i = 0; i < size; ++i) {
    printf("%02X", data[i]);
    if (i + 1 <= size) printf(" ");
  }
  printf("\n");
}

void FakeIMUSimulator::dumpBIN(const uint8_t * data)
{
  printf(
    "< %c%c%c%c%c%c%c%c%c", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
    data[8]);
  printf("%02X%02X", data[9], data[10]);
  printf(" %02X%02X", data[11], data[12]);
  printf(" %02X%02X", data[13], data[14]);
  printf(" %02X%02X%02X%02X%02X%02X", data[15], data[16], data[17], data[18], data[19], data[20]);
  printf(" %02X%02X%02X%02X%02X%02X", data[21], data[22], data[23], data[24], data[25], data[26]);
  printf(" %02X%02X%02X%02X%02X%02X", data[27], data[28], data[29], data[30], data[31], data[32]);
  printf(" %02X%02X%02X%02X", data[33], data[34], data[35], data[36]);
  printf(
    " %02X%02X%02X%02X%02X%02X%02X%02X", data[37], data[38], data[39], data[40], data[41], data[42],
    data[43], data[44]);
  printf(" %02X%02X%02X%02X%02X%02X", data[45], data[46], data[47], data[48], data[49], data[50]);
  printf(" %02X%02X", data[51], data[52]);
  printf("%c%c%c%c%c", data[53], data[54], data[55], data[56], data[57]);
}

void FakeIMUSimulator::onRead(
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

    std::string str(data, data + bytes_transfered);
    boost::remove_erase_if(str, boost::is_any_of("\r\n"));

    if (str == "$TSC,BIN,30") {
      bin_req_ = true;
    }

    // asynchronously read data
    uint8_t next[MAX_SIZE] = "";
    port_->async_read_some(
      as::buffer(next), boost::bind(
                          &FakeIMUSimulator::onRead, this, as::placeholders::error,
                          as::placeholders::bytes_transferred, next));
  }
}

void FakeIMUSimulator::onWrite(
  const boost::system::error_code & error, std::size_t bytes_transfered,
  const std::vector<uint8_t> & data)
{
  bool b;
  pthread_mutex_lock(&mutex_dump_);
  b = dump_;
  pthread_mutex_unlock(&mutex_dump_);
  if (b) {
    dumpBIN(&data[0]);
  }
}
