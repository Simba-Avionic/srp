/**
 * @file application_mw.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief Interface for all Simba mw application
 * @version 1.0
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CORE_APPLICATION_APPLICATION_MW_H_
#define CORE_APPLICATION_APPLICATION_MW_H_
#include <memory>
#include <string>
#include <unordered_map>

#include "core/application/Iapplication.h"
#include "core/logger/Logger.h"
#include "core/logger/console_logger.h"
namespace simba {
namespace core {
class ApplicationMW : public IApplication {
 protected:
 std::string app_name{"NONE"};
  void Run(const std::unordered_map<std::string, Parm>& parms) override {
    AppLogger::Warning("Application function: Run is not implemented");
  }
  void Stop() {
    AppLogger::Info("Application stopped unexpected");
    AppLogger::Warning("Application function: Stop is not implemented");
  }
  void onRun(const std::unordered_map<std::string, Parm>& parms) override {
    AppLogger::SetParms(app_name, logger::loggingLevel::DEBUG);
    AppLogger::AddLogger(std::make_shared<logger::ConsoleLogger>());
    AppLogger::Info("Application starting");
    this->Run(parms);
    AppLogger::Info("Application stopped");
  }

 public:
  virtual ~ApplicationMW() = default;
};
}  // namespace core
}  // namespace simba
#endif  // CORE_APPLICATION_APPLICATION_MW_H_
