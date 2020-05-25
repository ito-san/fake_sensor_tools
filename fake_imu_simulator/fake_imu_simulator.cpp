#include "fake_imu_simulator.h"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/process.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <string>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

#ifdef __cplusplus
extern "C" {
#endif

std::string ini_path = "";    //!< @brief path to ini file
bool stop_thread;             //!< @brief flag to stop thread
bool checksum_error;          //!< @brief flag to generate checksum error occur or not
bool debug_output;            //!< @brief flag to show debug output or not
pthread_mutex_t mutex_stop;   //!< @brief mutex to protect access to stop_thread
pthread_mutex_t mutex_error;  //!< @brief mutex to protect access to checksum_error
pthread_mutex_t mutex_debug;  //!< @brief mutex to protect access to debug_output
pthread_t th;                 //!< @brief thread handle
int fd;                       //!< @brief file descriptor to serial port

void loadIniFile(INI * ini)
{
  auto env = boost::this_process::environment();
  ini_path = env["HOME"].to_string() + "/.config/fake_imu_simulator.ini";

  if (!fs::exists(ini_path)) return;

  pt::ptree pt;
  read_ini(ini_path, pt);

  if (boost::optional<std::string> v = pt.get_optional<std::string>("device_name")) {
    strcpy(ini->device_name, v.get().c_str());
  }

  if (boost::optional<std::string> v = pt.get_optional<std::string>("log_file")) {
    strcpy(ini->log_file, v.get().c_str());
  }
}

void saveIniFile(INI * ini)
{
  pt::ptree pt;

  pt.put("device_name", ini->device_name);
  pt.put("log_file", ini->log_file);

  write_ini(ini_path, pt);
}

void output(uint8_t * data)
{
  printf(
    "%c%c%c%c%c%c%c%c%c", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
    data[8]);
  printf("%02X%02X", data[9], data[10]);
  printf(" %02X%02X", data[11], data[12]);
  printf(" %02X%02X", data[13], data[14]);
  printf(" %02X%02X%02X%02X%02X%02X", data[15], data[16], data[17], data[18], data[19], data[20]);
  printf(" %02X%02X%02X%02X%02X%02X", data[21], data[22], data[23], data[24], data[25], data[26]);
  printf(" %02X%02X%02X%02X%02X%02X", data[27], data[28], data[29], data[30], data[31], data[32]);
  printf(" %02X%02X%02X%02X", data[33], data[34], data[35], data[36]);  //jikoku
  printf(
    " %02X%02X%02X%02X%02X%02X%02X%02X", data[37], data[38], data[39], data[40], data[41], data[42],
    data[43], data[44]);
  printf(" %02X%02X%02X%02X%02X%02X", data[45], data[46], data[47], data[48], data[49], data[50]);
  printf(" %02X%02X", data[51], data[52]);
  printf("%c%c%c%c%c", data[53], data[54], data[55], data[56], data[57]);
}

void * thread(void * args)
{
  INI * ini = reinterpret_cast<INI *>(args);

  fs::ifstream ifs(ini->log_file, std::ios::in | std::ios::binary);
  if (!ifs) {
    return nullptr;
  }

  while (true) {
    bool b;
    pthread_mutex_lock(&mutex_stop);
    b = stop_thread;
    pthread_mutex_unlock(&mutex_stop);
    if (b) break;

    uint8_t data[58] = "$TSC,BIN,";
    ifs.read((char *)&data[9], 44);
    if (ifs.eof()) {
      ifs.clear();
      ifs.seekg(0, std::ios_base::beg);
      ifs.read((char *)&data[9], 44);
    }

    uint8_t sum = 0;
    for (int i = 1; i < 52; ++i) {
      sum = sum ^ data[i];
    }
    data[53] = '*';

    pthread_mutex_lock(&mutex_error);
    b = checksum_error;
    pthread_mutex_unlock(&mutex_error);

    if (!b) {
      sprintf((char *)&data[54], "%02X", sum);
    } else {
      data[54] = '?';
      data[55] = '?';
    }

    data[56] = 0xd;
    data[57] = 0xa;
    write(fd, data, 58);

    pthread_mutex_lock(&mutex_debug);
    b = debug_output;
    pthread_mutex_unlock(&mutex_debug);
    if (b) {
      output(data);
    }
    // 30Hz
    usleep(1000000 / 30);
  }

  ifs.close();
  close(fd);

  return nullptr;
}

int start(INI * ini)
{
  int ret = 0;
  fd = open(ini->device_name, O_WRONLY);
  if (fd < 0) {
    ret = errno;
    std::cerr << strerror(ret) << std::endl;
    return ret;
  }

  if (!fs::exists(ini->log_file)) {
    ret = ENOENT;
    std::cerr << strerror(ret) << std::endl;
    return ret;
  }

  stop_thread = false;
  pthread_create(&th, nullptr, thread, ini);
  return ret;
}

void stop()
{
  pthread_mutex_lock(&mutex_stop);
  stop_thread = true;
  pthread_mutex_unlock(&mutex_stop);
  pthread_join(th, NULL);
}

void setChecksumError(int is_error)
{
  pthread_mutex_lock(&mutex_error);
  checksum_error = is_error;
  pthread_mutex_unlock(&mutex_error);
}

void setDebugOutput(int is_debug)
{
  pthread_mutex_lock(&mutex_debug);
  debug_output = is_debug;
  pthread_mutex_unlock(&mutex_debug);
}

#ifdef __cplusplus
}
#endif
