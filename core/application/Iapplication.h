/**
 * @file Iapplication.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief Interface for all Simba application
 * @version 1.0
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CORE_APPLICATION_IAPPLICATION_H_
#define CORE_APPLICATION_IAPPLICATION_H_
#include <chrono>
#include <string>
#include <thread>
#include <unordered_map>

#include "core/application/parm.h"
namespace simba {
namespace core {
class IApplication {
 protected:
  /**
   * @brief This function is called to launch the application
   *
   * @param parms map with parms
   */
  virtual void Run(const std::unordered_map<std::string, Parm>& parms) = 0;
  virtual void Stop() {}
  /**
   * @brief This is pre-run function only for creting new application
   * interfacess
   *
   * @param parms map with parms
   */
  virtual void onRun(const std::unordered_map<std::string, Parm>& parms) {
    this->Run(parms);
  }
  void SleepMainThred() {
    while (true) {
      std::this_thread::sleep_for(std::chrono::hours::max());
    }
  }

 public:
  void StopApp() {
    this->Stop();
    exit(1);
  }
  virtual ~IApplication() = default;
  void RunApp(int argc, char const* argv[]) {
    std::unordered_map<std::string, Parm> parms{};
    for (int i = 0; i < argc; i++) {
      Parm p{std::string{argv[i]}};
      parms.insert({p.GetName(), p});
    }

    onRun(parms);
  }
};
}  // namespace core
}  // namespace simba
#endif  // CORE_APPLICATION_IAPPLICATION_H_
