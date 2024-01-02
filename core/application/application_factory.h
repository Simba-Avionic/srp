/**
 * @file Iapplication.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief Application factory for all simba application
 * @version 1.0
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CORE_APPLICATION_APPLICATION_FACTORY_H_
#define CORE_APPLICATION_APPLICATION_FACTORY_H_
#include <signal.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <memory>

#include "core/application/Iapplication.h"
namespace simba {
namespace core {
namespace {   // NOLINT
static std::unique_ptr<IApplication> app{nullptr};  // NOLINT
}   // NOLINT
void signal_callback_handler(int signum) {
  if (app != nullptr) {
    app->StopApp();
  }
  exit(signum);
}

template <class APP>
class ApplicationFactory {
 private:
 public:
  static void Start(int argc, char const *argv[]) {
    app = std::make_unique<APP>();
    signal(SIGINT, signal_callback_handler);
    app->RunApp(argc, argv);
  }
  static void Stop() {}
};

}  // namespace core
}  // namespace simba
#endif  // CORE_APPLICATION_APPLICATION_FACTORY_H_
