#include <fake_velodyne_simulator.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

FakeVelodyneSimulator * FakeVelodyneSimulator::velodyne_ = nullptr;

FakeVelodyneSimulator::FakeVelodyneSimulator() : address_("http://localhost:8000") {}

FakeVelodyneSimulator * FakeVelodyneSimulator::get(void)
{
  if (velodyne_ == nullptr) {
    static FakeVelodyneSimulator velodyne;
    velodyne_ = &velodyne;
  }

  return velodyne_;
}

void FakeVelodyneSimulator::loadIniFile(void)
{
  auto env = boost::this_process::environment();
  ini_path_ = env["HOME"].to_string() + "/.config/fake_velodyne_simulator.ini";

  if (!fs::exists(ini_path_)) return;

  pt::ptree pt;
  read_ini(ini_path_, pt);

  if (boost::optional<std::string> v = pt.get_optional<std::string>("address")) {
    const char * str = v.get().c_str();
    strncpy(address_, str, strlen(str));
  }
  if (boost::optional<std::string> v = pt.get_optional<std::string>("info")) {
    const char * str = v.get().c_str();
    strncpy(info_path_, str, strlen(str));
  }
  if (boost::optional<std::string> v = pt.get_optional<std::string>("diag")) {
    const char * str = v.get().c_str();
    strncpy(diag_path_, str, strlen(str));
  }
  if (boost::optional<std::string> v = pt.get_optional<std::string>("status")) {
    const char * str = v.get().c_str();
    strncpy(status_path_, str, strlen(str));
  }
  if (boost::optional<std::string> v = pt.get_optional<std::string>("settings")) {
    const char * str = v.get().c_str();
    strncpy(settings_path_, str, strlen(str));
  }

  //  Load data from info.json
  loadInfoJson();
  // Load data from diag.json
  loadDiagJson();
  // Load data from status.json
  loadStatusJson();
  // Load data from settings.json
  loadSettingsJson();
}

void FakeVelodyneSimulator::saveIniFile(void)
{
  pt::ptree pt;

  pt.put("address", address_);
  pt.put("info", info_path_);
  pt.put("diag", diag_path_);
  pt.put("status", status_path_);
  pt.put("settings", settings_path_);

  write_ini(ini_path_, pt);
}

void FakeVelodyneSimulator::setAddress(const char * address)
{
  strncpy(address_, address, strlen(address));
}

const char * FakeVelodyneSimulator::getAddress(void) const { return address_; }

int FakeVelodyneSimulator::start(void)
{
  int ret = 0;
  stop_thread_ = false;
  pthread_create(&th_, nullptr, &FakeVelodyneSimulator::threadHelper, this);
  return ret;
}

void FakeVelodyneSimulator::stop(void)
{
  pthread_mutex_lock(&mutex_stop_);
  stop_thread_ = true;
  pthread_mutex_unlock(&mutex_stop_);
  pthread_join(th_, NULL);
}

void FakeVelodyneSimulator::setDebugOutput(int is_debug)
{
  pthread_mutex_lock(&mutex_dump_);
  dump_ = is_debug;
  pthread_mutex_unlock(&mutex_dump_);
}

// Info
void FakeVelodyneSimulator::setInfoJson(const char * path)
{
  strncpy(info_path_, path, strlen(path));
  //  Load data from info.json
  loadInfoJson();
}

const char * FakeVelodyneSimulator::getInfoJson(void) const { return info_path_; }

// Diagnostics
void FakeVelodyneSimulator::setDiagJson(const char * path)
{
  strncpy(diag_path_, path, strlen(path));
  // Load data from diag.json
  loadDiagJson();
}

const char * FakeVelodyneSimulator::getDiagJson(void) const { return diag_path_; }

double FakeVelodyneSimulator::getTopHv(void) { return top_hv_; }

void FakeVelodyneSimulator::setTopHv(double top_hv)
{
  top_hv_ = top_hv;
  int raw = 4096 * (top_hv_ / 101.0 + 5.0) / 5.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["hv"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getTopTemp(void) { return top_temp_; }

void FakeVelodyneSimulator::setTopTemp(double top_temp)
{
  top_temp_ = top_temp;
  double d =
    std::round((1.8639 - (std::pow(top_temp_ + 1481.96, 2.0) - 2.1962e6) * 3.88e-6) * 10000) /
    10000;
  int raw = std::round((4096 * d / 5.0) * 1000) / 1000;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["lm20_temp"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getTop5v(void) { return top_5v_; }

void FakeVelodyneSimulator::setTop5v(double top_5v)
{
  top_5v_ = top_5v;
  int raw = 4096 * (top_5v_ / 2.0) / 5.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["pwr_5v"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getTop2_5v(void) { return top_2_5v_; }

void FakeVelodyneSimulator::setTop2_5v(double top_2_5v)
{
  top_2_5v_ = top_2_5v;
  int raw = 4096 * top_2_5v_ / 5.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["pwr_2_5v"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getTop3_3v(void) { return top_3_3v_; }

void FakeVelodyneSimulator::setTop3_3v(double top_3_3v)
{
  top_3_3v_ = top_3_3v;
  int raw = 4096 * top_3_3v_ / 5.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["pwr_3_3v"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getTop5vRaw(void) { return top_5v_raw_; }

void FakeVelodyneSimulator::setTop5vRaw(double top_5v_raw)
{
  top_5v_raw_ = top_5v_raw;
  int raw = 4096 * (top_5v_raw_ / 2.0) / 5.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["pwr_5v_raw"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getTopVcc(void) { return top_vcc_; }

void FakeVelodyneSimulator::setTop5vVcc(double top_vcc)
{
  top_vcc_ = top_vcc;
  int raw = 4096 * top_vcc_ / 5.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["pwr_vccint"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getBottomIOut(void) { return bot_i_out_; }

void FakeVelodyneSimulator::setBottomIOut(double bot_i_out)
{
  bot_i_out_ = bot_i_out;
  int raw = 4096 * (bot_i_out_ / 10.0 + 2.5) / 5.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["bot"]["i_out"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getBottomTemp() { return bot_temp_; }

void FakeVelodyneSimulator::setBottomTemp(double bot_temp)
{
  bot_temp_ = bot_temp;
  double d =
    std::round((1.8639 - (std::pow(bot_temp_ + 1481.96, 2.0) - 2.1962e6) * 3.88e-6) * 10000) /
    10000;
  int raw = std::round((4096 * d / 5.0) * 1000) / 1000;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["bot"]["lm20_temp"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getBottom1_2v(void) { return bot_1_2v_; }

void FakeVelodyneSimulator::setBottom1_2v(double bot_1_2v)
{
  bot_1_2v_ = bot_1_2v;
  int raw = 4096 * bot_1_2v_ / 5.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["pwr_1_2v"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getBottom1_25v(void) { return bot_1_25v_; }

void FakeVelodyneSimulator::setBottom1_25v(double bot_1_25v)
{
  bot_1_25v_ = bot_1_25v;
  int raw = 4096 * bot_1_25v_ / 5.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["pwr_1_25v"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getBottom2_5v(void) { return bot_2_5v_; }

void FakeVelodyneSimulator::setBottom2_5v(double bot_2_5v)
{
  bot_2_5v_ = bot_2_5v;
  int raw = 4096 * bot_2_5v_ / 5.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["pwr_2_5v"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getBottom3_3v(void) { return bot_3_3v_; }

void FakeVelodyneSimulator::setBottom3_3v(double bot_3_3v)
{
  bot_3_3v_ = bot_3_3v;
  int raw = 4096 * bot_3_3v_ / 5.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["pwr_3_3v"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getBottom5v(void) { return bot_5v_; }

void FakeVelodyneSimulator::setBottom5v(double bot_5v)
{
  bot_5v_ = bot_5v;
  int raw = 4096 * bot_5v_ / 5.0 / 2.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["top"]["pwr_5v"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

double FakeVelodyneSimulator::getBottomVIn(void) { return bot_v_in_; }

void FakeVelodyneSimulator::setBottomVIn(double bot_v_in)
{
  bot_v_in_ = bot_v_in;
  int raw = 4096 * bot_v_in_ / 5.0 / 11.0;

  pthread_mutex_lock(&mutex_json_);
  diag_json_["volt_temp"]["bot"]["pwr_v_in"] = raw;
  pthread_mutex_unlock(&mutex_json_);
}

// Status
void FakeVelodyneSimulator::setStatusJson(const char * path)
{
  strncpy(status_path_, path, strlen(path));
  // Load data from status.json
  loadStatusJson();
}

const char * FakeVelodyneSimulator::getStatusJson(void) const { return status_path_; }

void FakeVelodyneSimulator::setMotorState(bool motor_state)
{
  motor_state_ = motor_state;

  pthread_mutex_lock(&mutex_json_);
  status_json_["motor"]["state"] = json::value::string(motor_state_ ? "On" : "Off");
  pthread_mutex_unlock(&mutex_json_);
}

bool FakeVelodyneSimulator::getMotorState(void) const { return motor_state_; }

void FakeVelodyneSimulator::setMotorRpm(int motor_rpm)
{
  motor_rpm_ = motor_rpm;

  pthread_mutex_lock(&mutex_json_);
  status_json_["motor"]["rpm"] = motor_rpm_;
  pthread_mutex_unlock(&mutex_json_);
}

int FakeVelodyneSimulator::getMotorRpm(void) const { return motor_rpm_; }

void FakeVelodyneSimulator::setLaserState(bool laser_state)
{
  laser_state_ = laser_state;

  pthread_mutex_lock(&mutex_json_);
  status_json_["laser"]["state"] = json::value::string(laser_state_ ? "On" : "Off");
  pthread_mutex_unlock(&mutex_json_);
}

bool FakeVelodyneSimulator::getLaserState(void) const { return laser_state_; }

// Settings
void FakeVelodyneSimulator::setSettingsJson(const char * path)
{
  strncpy(settings_path_, path, strlen(path));
  // Load data from settings.json
  loadSettingsJson();
}

const char * FakeVelodyneSimulator::getSettingsJson(void) const { return settings_path_; }

void FakeVelodyneSimulator::setRpmSetting(int rpm_setting)
{
  rpm_setting_ = rpm_setting;

  pthread_mutex_lock(&mutex_json_);
  settings_json_["rpm"] = rpm_setting_;
  pthread_mutex_unlock(&mutex_json_);
}

int FakeVelodyneSimulator::getRpmSetting(void) const { return rpm_setting_; }

void FakeVelodyneSimulator::loadInfoJson(void)
{
  fs::ifstream ifs(info_path_, std::ios::in);
  if (!ifs) return;
  info_json_ = json::value::parse(ifs);
  ifs.close();
}

void FakeVelodyneSimulator::loadDiagJson(void)
{
  fs::ifstream ifs(diag_path_, std::ios::in);
  if (!ifs) return;
  diag_json_ = json::value::parse(ifs);
  ifs.close();

  int top_hv = diag_json_["volt_temp"]["top"]["hv"].as_integer();
  top_hv_ = std::round(((top_hv * 5.0) / 4096) * 1000) / 1000;
  top_hv_ = std::round((101.0 * (top_hv_ - 5.0)) * 100) / 100;

  int top_temp = diag_json_["volt_temp"]["top"]["lm20_temp"].as_integer();
  top_temp_ = std::round(((top_temp * 5.0) / 4096) * 10000) / 10000;
  top_temp_ =
    std::round((-1481.96 + std::sqrt(2.1962e6 + ((1.8639 - top_temp_) / 3.88e-6))) * 1000) / 1000;

  int top_5v = diag_json_["volt_temp"]["top"]["pwr_5v"].as_integer();
  top_5v_ = std::round((((top_5v * 5.0) / 4096) * 2.0) * 1000) / 1000;

  int top_2_5v = diag_json_["volt_temp"]["top"]["pwr_2_5v"].as_integer();
  top_2_5v_ = std::round(((top_2_5v * 5.0) / 4096) * 1000) / 1000;

  int top_3_3v = diag_json_["volt_temp"]["top"]["pwr_3_3v"].as_integer();
  top_3_3v_ = std::round(((top_3_3v * 5.0) / 4096) * 1000) / 1000;

  int top_5v_raw = diag_json_["volt_temp"]["top"]["pwr_5v_raw"].as_integer();
  top_5v_raw_ = std::round((((top_5v_raw * 5.0) / 4096) * 2.0) * 1000) / 1000;

  int top_vcc = diag_json_["volt_temp"]["top"]["pwr_vccint"].as_integer();
  top_vcc_ = std::round(((top_vcc * 5.0) / 4096) * 1000) / 1000;

  int bot_i_out = diag_json_["volt_temp"]["bot"]["i_out"].as_integer();
  bot_i_out_ = std::round(((bot_i_out * 5.0) / 4096) * 10000) / 10000;
  bot_i_out_ = std::round((10.0 * (bot_i_out_ - 2.5)) * 1000) / 1000;

  int bot_temp = diag_json_["volt_temp"]["bot"]["lm20_temp"].as_integer();
  bot_temp_ = std::round(((bot_temp * 5.0) / 4096) * 10000) / 10000;
  bot_temp_ =
    std::round((-1481.96 + std::sqrt(2.1962e6 + ((1.8639 - bot_temp_) / 3.88e-6))) * 1000) / 1000;

  int bot_1_2v = diag_json_["volt_temp"]["bot"]["pwr_1_2v"].as_integer();
  bot_1_2v_ = std::round(((bot_1_2v * 5.0) / 4096) * 1000) / 1000;

  int bot_5v = diag_json_["volt_temp"]["bot"]["pwr_5v"].as_integer();
  bot_5v_ = std::round(((bot_5v * 5.0) / 4096) * 2 * 1000) / 1000;

  int bot_2_5v = diag_json_["volt_temp"]["bot"]["pwr_2_5v"].as_integer();
  bot_2_5v_ = std::round(((bot_2_5v * 5.0) / 4096) * 1000) / 1000;

  int bot_3_3v = diag_json_["volt_temp"]["bot"]["pwr_3_3v"].as_integer();
  bot_3_3v_ = std::round(((bot_3_3v * 5.0) / 4096) * 1000) / 1000;

  int bot_v_in = diag_json_["volt_temp"]["bot"]["pwr_v_in"].as_integer();
  bot_v_in_ = std::round(((bot_v_in * 5.0) / 4096) * 11.0 * 1000) / 1000;

  int bot_1_25v = diag_json_["volt_temp"]["bot"]["pwr_1_25v"].as_integer();
  bot_1_25v_ = std::round(((bot_1_25v * 5.0) / 4096) * 1000) / 1000;
}

void FakeVelodyneSimulator::loadStatusJson(void)
{
  fs::ifstream ifs(status_path_, std::ios::in);
  if (!ifs) return;
  status_json_ = json::value::parse(ifs);
  ifs.close();

  std::string state = status_json_["motor"]["state"].as_string();
  motor_state_ = (state == "On") ? true : false;
  motor_rpm_ = status_json_["motor"]["rpm"].as_integer();

  state = status_json_["laser"]["state"].as_string();
  laser_state_ = (state == "On") ? true : false;
}

void FakeVelodyneSimulator::loadSettingsJson(void)
{
  fs::ifstream ifs(settings_path_, std::ios::in);
  if (!ifs) return;
  settings_json_ = json::value::parse(ifs);
  ifs.close();

  rpm_setting_ = settings_json_["rpm"].as_integer();
}

void FakeVelodyneSimulator::handleGet(http::http_request request)
{
  // Get the underling URI of the request message
  std::string path = request.request_uri().path();

  if (path == "/cgi/info.json") {
    request.reply(http::status_codes::OK, info_json_);
  } else if (path == "/cgi/diag.json") {
    request.reply(http::status_codes::OK, diag_json_);
  } else if (path == "/cgi/status.json") {
    request.reply(http::status_codes::OK, status_json_);
  } else if (path == "/cgi/settings.json") {
    request.reply(http::status_codes::OK, settings_json_);
  } else {
    request.reply(http::status_codes::BadRequest);
  }
}

void * FakeVelodyneSimulator::thread(void)
{
  web::http::experimental::listener::http_listener listener(address_);

  // Add a general handler to support all requests
  listener.support(
    web::http::methods::GET, boost::bind(&FakeVelodyneSimulator::handleGet, this, _1));
  // Asynchronously open the listener
  listener.open().wait();

  // Handle incoming requests.
  std::cout << "Starting up HTTP server." << std::endl;

  while (true) {
    bool b;
    pthread_mutex_lock(&mutex_stop_);
    b = stop_thread_;
    pthread_mutex_unlock(&mutex_stop_);
    if (b) break;

    usleep(1000);
  }

  listener.close();

  return nullptr;
}

void FakeVelodyneSimulator::dump(Direction dir, const uint8_t * data, std::size_t size)
{
  printf("%s ", (dir == Read) ? ">" : "<");

  for (std::size_t i = 0; i < size; ++i) {
    printf("%02X", data[i]);
    if (i + 1 <= size) printf(" ");
  }
  printf("\n");
}
