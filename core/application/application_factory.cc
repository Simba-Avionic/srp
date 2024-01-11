/**
 * @file application_factory.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief Application factory for all simba application
 * @version 0.1
 * @date 2024-01-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "core/application/application_factory.h"
namespace simba {
namespace core {
void signal_callback_handler(int signum) {
  if (app != nullptr) {
    app->StopApp();
  }
  exit(signum);
}
}  // namespace core
}  // namespace simba
