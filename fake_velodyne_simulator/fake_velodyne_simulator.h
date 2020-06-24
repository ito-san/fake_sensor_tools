#ifndef FAKE_IMU_SIMULATOR_FAKE_VELODYNE_SIMULATOR_H_
#define FAKE_IMU_SIMULATOR_FAKE_VELODYNE_SIMULATOR_H_

/**
 * @file fake_velodyne_simulator.h
 * @brief Fake Velodyne simulator definitions
 */

#include <cpprest/http_listener.h>
#include <linux/limits.h>
#include <string>

namespace http = web::http;
namespace json = web::json;

class FakeVelodyneSimulator
{
public:
  static FakeVelodyneSimulator * get();

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
   * @brief Set server address for saving it to ini file
   * @param [in] address server address
   */
  void setAddress(const char * address);

  /**
   * @brief Get server address stored in ini file
   * @return server addess
   */
  const char * getAddress(void) const;

  /**
   * @brief Start HTTP server
   * @return 0 on success, otherwise error
   */
  int start(void);

  /**
   * @brief Stop HTTP server
   */
  void stop(void);

  /**
   * @brief Show debug output or not
   * @param [in] is_debug debug output or not
   */
  void setDebugOutput(int is_debug);

  // Info
  /**
   * @brief Set path of info.json for saving it to ini file
   * @param [in] path path of info.json
   */
  void setInfoJson(const char * path);

  /**
   * @brief Get path of info.json stored in ini file
   * @return path of info.json
   */
  const char * getInfoJson(void) const;

  // Diagnostics
  /**
   * @brief Set path of diag.json for saving it to ini file
   * @param [in] path path of diag.json
   */
  void setDiagJson(const char * path);

  /**
   * @brief Get path of diag.json stored in ini file
   * @return path of diag.json
   */
  const char * getDiagJson(void) const;

  /**
   * @brief Get the high voltage bias to the APD (Avalanche photodiode)
   * @return the high voltage bias to the APD (Avalanche photodiode)
   */
  double getTopHv();

  /**
   * @brief Set the high voltage bias to the APD (Avalanche photodiode)
   * @param [in] top_hv the high voltage bias to the APD (Avalanche photodiode)
   */
  void setTopHv(double top_hv);

  /**
   * @brief Get the temperature of the top board
   * @return the temperature of the top board
   */
  double getTopTemp(void);

  /**
   * @brief Set the temperature of the top board
   * @param [in] top_hv the temperature of the top board
   */
  void setTopTemp(double top_temp);

  /**
   * @brief Get the top board's 5.0 V rail
   * @return the top board's 5.0 V rail
   */
  double getTop5v(void);

  /**
   * @brief Set the top board's 5.0 V rail
   * @param [in] top_5v the top board's 5.0 V rail
   */
  void setTop5v(double top_5v);

  /**
   * @brief Get the top board's 2.5 V rail
   * @return the top board's 2.5 V rail
   */
  double getTop2_5v(void);

  /**
   * @brief Set the top board's 2.5 V rail
   * @param [in] top_2_5v the top board's 2.5 V rail
   */
  void setTop2_5v(double top_2_5v);

  /**
   * @brief Get the top board's 3.3 V rail
   * @return top the board's 3.3 V rail
   */
  double getTop3_3v(void);

  /**
   * @brief Set the top board's 3.3 V rail
   * @param [in] top_3_3v the top board's 3.3 V rail
   */
  void setTop3_3v(double top_3_3v);

  /**
   * @brief Get the top board's unregulated 5.0 V rail
   * @return the top board's unregulated 5.0 V rail
   */
  double getTop5vRaw(void);

  /**
   * @brief Set the top board's unregulated 5.0 V rail
   * @param [in] top_5v_raw the top board's unregulated 5.0 V rail
   */
  void setTop5vRaw(double top_5v_raw);

  /**
   * @brief Get the top board's 1.2 V rail
   * @return the top board's 1.2 V rail
   */
  double getTopVcc(void);

  /**
   * @brief Set the top board's 1.2 V rail
   * @param [in] top_vcc the top board's 1.2 V rail
   */
  void setTop5vVcc(double top_vcc);

  /**
   * @brief Get the sensor's input power line
   * @return the sensor's input power line
   */
  double getBottomIOut(void);

  /**
   * @brief Set the sensor's input power line
   * @param [in] bot_i_out the sensor's input power line
   */
  void setBottomIOut(double bot_i_out);

  /**
   * @brief Get the temperature of the bottom board
   * @return the temperature of the bottom board
   */
  double getBottomTemp(void);

  /**
   * @brief Set the temperature of the bottom board
   * @param [in] bot_temp the temperature of the bottom board
   */
  void setBottomTemp(double bot_temp);

  /**
   * @brief Get the bottom board's 1.2 V rail
   * @return the bottom board's 1.2 V rail
   */
  double getBottom1_2v(void);

  /**
   * @brief Set the bottom board's 1.2 V rail
   * @param [in] bot_1_2v the bottom board's 1.2 V rail
   */
  void setBottom1_2v(double bot_1_2v);

  /**
   * @brief Get the bottom board's 1.25 V rail
   * @return the bottom board's 1.25 V rail
   */
  double getBottom1_25v(void);

  /**
   * @brief Set the bottom board's 1.25 V rail
   * @param [in] bot_1_25v the bottom board's 1.25 V rail
   */
  void setBottom1_25v(double bot_1_25v);

  /**
   * @brief Get the bottom board's 2.5 V rail
   * @return bottom the board's 2.5 V rail
   */
  double getBottom2_5v(void);

  /**
   * @brief Set the bottom board's 2.5 V rail
   * @param [in] bot_2_5v the bottom board's 2.5 V rail
   */
  void setBottom2_5v(double bot_2_5v);

  /**
   * @brief Get the bottom board's 3.3 V rail
   * @return the bottom board's 3.3 V rail
   */
  double getBottom3_3v(void);

  /**
   * @brief Set the bottom board's 3.3 V rail
   * @param [in] bot_3_3v the bottom board's 3.3 V rail
   */
  void setBottom3_3v(double bot_3_3v);

  /**
   * @brief Get the bottom board's 5.0 V rail
   * @return the bottom board's 5.0 V rail
   */
  double getBottom5v(void);

  /**
   * @brief Set the bottom board's 5.0 V rail
   * @param [in] bot_5v the bottom board's 5.0 V rail
   */
  void setBottom5v(double bot_5v);

  /**
   * @brief Get the voltage of the input power
   * @return the voltage of the input power
   */
  double getBottomVIn(void);

  /**
   * @brief Set the voltage of the input power
   * @param [in] bot_v_in the voltage of the input power
   */
  void setBottomVIn(double bot_v_in);

  // Status
  /**
   * @brief Set path of status.json for saving it to ini file
   * @param [in] path path of status.json
   */
  void setStatusJson(const char * path);

  /**
   * @brief Get path of status.json stored in ini file
   * @return path of status.json
   */
  const char * getStatusJson(void) const;

  /**
   * @brief Set motor state
   * @param [in] motor_state motor state
   */
  void setMotorState(bool motor_state);

  /**
   * @brief Get motor state
   * @return motor state
   */
  bool getMotorState(void) const;

  /**
   * @brief Set motor rpm
   * @param [in] motor_rpm motor rpm
   */
  void setMotorRpm(int motor_rpm);

  /**
   * @brief Get motor rpm
   * @return motor rpm
   */
  int getMotorRpm(void) const;

  /**
   * @brief Set laser state
   * @param [in] laser_state laser state
   */
  void setLaserState(bool laser_state);

  /**
   * @brief Get laser state
   * @return laser state
   */
  bool getLaserState(void) const;

  // Settings
  /**
   * @brief Set path of settings.json for saving it to ini file
   * @param [in] path path of settings.json
   */
  void setSettingsJson(const char * path);

  /**
   * @brief Get path of settings.json stored in ini file
   * @return path of settings.json
   */
  const char * getSettingsJson(void) const;

  /**
   * @brief Set rpm setting
   * @param [in] rpm_setting rpm setting
   */
  void setRpmSetting(int rpm_setting);

  /**
   * @brief Get rpm setting
   * @return rpm setting
   */
  int getRpmSetting(void) const;

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
  FakeVelodyneSimulator();

  /**
   * @brief Thread helper funcion
   * @param[in] arg argument
   */
  static void * threadHelper(void * arg)
  {
    return reinterpret_cast<FakeVelodyneSimulator *>(arg)->thread();
  }

  /**
   * @brief Thread loop
   * @return nullptr
   */
  void * thread(void);

  /**
   * @brief Load data from info.json
   */
  void loadInfoJson(void);

  /**
   * @brief Load data from diag.json
   */
  void loadDiagJson(void);

  /**
   * @brief Load data from status.json
   */
  void loadStatusJson(void);

  /**
   * @brief Load data from settings.json
   */
  void loadSettingsJson(void);

  /**
   * @brief Handle 'GET' request
   * @param [in] request HTTP request with the 'GET' method
   */
  void handleGet(http::http_request request);

  /**
   * @brief Dump sent/received Data
   * @param[in] dir io direction
   * @param[in] data pointer to data
   * @param[in] size size of data
   */
  void dump(Direction dir, const uint8_t * data, std::size_t size);

  static FakeVelodyneSimulator * velodyne_;  //!< @brief reference to itself
  std::string ini_path_;                     //!< @brief path to ini file
  pthread_mutex_t mutex_stop_;               //!< @brief mutex to protect access to stop_thread
  pthread_mutex_t mutex_dump_;               //!< @brief mutex to protect access to dump flag
  pthread_mutex_t mutex_json_;               //!< @brief mutex to protect access to json
  pthread_t th_;                             //!< @brief thread handle

  // General
  char address_[PATH_MAX];  //!< @brief Server address
  bool stop_thread_;        //!< @brief flag to stop thread
  bool dump_;               //!< @brief flag to show debug output or not

  // Info
  char info_path_[PATH_MAX];  //!< @brief path of info.json
  json::value info_json_;     //!< @brief values of info.json

  // Diagnostics
  char diag_path_[PATH_MAX];  //!< @brief path of diag.json
  json::value diag_json_;     //!< @brief values of diag.json
  double top_hv_;             //!< @brief the high voltage bias to the APD (Avalanche photodiode)
  double top_temp_;           //!< @brief the temperature of the top board
  double top_5v_;             //!< @brief the top board's 5.0 V rail
  double top_2_5v_;           //!< @brief the top board's 2.5 V rail
  double top_3_3v_;           //!< @brief the top board's 3.3 V rail
  double top_5v_raw_;         //!< @brief the top board's unregulated 5.0 V rail
  double top_vcc_;            //!< @brief the top board's 1.2 V rail

  double bot_i_out_;  //!< @brief the sensor's input power line
  double bot_temp_;   //!< @brief the temperature of the bottom board
  double bot_1_2v_;   //!< @brief the bottom board's 1.2 V rail
  double bot_1_25v_;  //!< @brief the bottom board's 1.25 V rail
  double bot_2_5v_;   //!< @brief the bottom board's 2.5 V rail
  double bot_3_3v_;   //!< @brief the bottom board's 3.3 V rail
  double bot_5v_;     //!< @brief the bottom board's 5.0 V rail
  double bot_v_in_;   //!< @brief the voltage of the input power

  // Status
  char status_path_[PATH_MAX];  //!< @brief path of status.json
  json::value status_json_;     //!< @brief values of diag.json
  bool motor_state_;            //!< @brief motor state
  int motor_rpm_;               //!< @brief motor rpm
  bool laser_state_;            //!< @brief laser state

  // Settings
  char settings_path_[PATH_MAX];  //!< @brief path of settings.json
  json::value settings_json_;     //!< @brief values of settings.json
  int rpm_setting_;               //!< @brief rpm setting
};

#endif  // FAKE_IMU_SIMULATOR_FAKE_VELODYNE_SIMULATOR_H_
