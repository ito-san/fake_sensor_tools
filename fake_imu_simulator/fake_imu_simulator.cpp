#include <fake_imu_simulator.h>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/process.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <string>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

static constexpr int MAX_SIZE = 58;

FakeIMUSimulator * FakeIMUSimulator::imu_ = nullptr;

FakeIMUSimulator::FakeIMUSimulator() {}

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
  fd_ = open(device_name_, O_WRONLY);
  if (fd_ < 0) {
    ret = errno;
    std::cerr << strerror(ret) << std::endl;
    return ret;
  }

  if (!fs::exists(log_file_)) {
    ret = ENOENT;
    std::cerr << strerror(ret) << std::endl;
    return ret;
  }

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

  close(fd_);
}

void FakeIMUSimulator::setChecksumError(int is_error)
{
  pthread_mutex_lock(&mutex_error_);
  checksum_error_ = is_error;
  pthread_mutex_unlock(&mutex_error_);
}

void FakeIMUSimulator::setDebugOutput(int is_debug)
{
  pthread_mutex_lock(&mutex_debug_);
  debug_output_ = is_debug;
  pthread_mutex_unlock(&mutex_debug_);
}

void * FakeIMUSimulator::thread(void)
{
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

    uint8_t data[MAX_SIZE] = "";
    ifs.read(reinterpret_cast<char *>(data), sizeof(data));
    if (ifs.eof()) {
      ifs.clear();
      ifs.seekg(0, std::ios_base::beg);
      ifs.read(reinterpret_cast<char *>(data), sizeof(data));
    }

    pthread_mutex_lock(&mutex_error_);
    b = checksum_error_;
    pthread_mutex_unlock(&mutex_error_);

    if (b) {
      data[MAX_SIZE - 3] = '?';
      data[MAX_SIZE - 4] = '?';
    }

    write(fd_, data, sizeof(data));

    pthread_mutex_lock(&mutex_debug_);
    b = debug_output_;
    pthread_mutex_unlock(&mutex_debug_);
    if (b) {
      dump(data);
    }
    // 30Hz
    usleep(1000000 / 30);
  }

  ifs.close();

  return nullptr;
}

void FakeIMUSimulator::dump(const uint8_t * data)
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
