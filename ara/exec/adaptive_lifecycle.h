/**
 * @file adaptive_lifecycle.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_EXEC_ADAPTIVE_LIFECYCLE_H_
#define ARA_EXEC_ADAPTIVE_LIFECYCLE_H_

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <thread>  // NOLINT
#include <utility>

#include "ara/core/initialization.h"
#include "ara/exec/adaptive_lifecycle_menager.h"
#include "ara/log/logging_menager.h"

namespace ara {
namespace exec {
void signal_callback_handler(int signum) {
  AdaptiveLifecycleMenager::StopAdaptiveLifecycleMenager(signum);
}
void reap_child(int sig) {
  int status;
  waitpid(0, &status, WNOHANG);
  waitpid(0, &status, SA_NOCLDWAIT);
}
template <typename APPTYPE, typename... Params>
static int RunAdaptiveLifecycle(int argc, char const *argv[],
                                Params &&...args) {
  setsid();
  ara::core::Initialize();
  AdaptiveLifecycleMenager::CreateAdaptiveLifecycleMenager<APPTYPE>(
      argc, argv, true, std::forward<Params>(args)...);

  signal(SIGCHLD, SIG_IGN);
  signal(SIGTERM, signal_callback_handler);
  signal(SIGINT, signal_callback_handler);

  const auto res = AdaptiveLifecycleMenager::StartAdaptiveLifecycleMenager();

  ara::core::Deinitialize();
  // log::LoggingMenager::Clear();
  // std::terminate();
  return res;
}
template <typename APPTYPE, typename... Params>
static int RunAdaptiveLifecycleWithoutControl(int argc, char const *argv[],
                                              Params &&...args) {
  setsid();
  ara::core::Initialize();

  AdaptiveLifecycleMenager::CreateAdaptiveLifecycleMenager<APPTYPE>(
      argc, argv, false, std::forward<Params>(args)...);

  signal(SIGCHLD, SIG_IGN);
  signal(SIGTERM, signal_callback_handler);
  signal(SIGINT, signal_callback_handler);

  const auto res = AdaptiveLifecycleMenager::StartAdaptiveLifecycleMenager();

  ara::core::Deinitialize();
  // log::LoggingMenager::Clear();
  // std::terminate();
  return res;
}
}  // namespace exec
}  // namespace ara

#endif  // ARA_EXEC_ADAPTIVE_LIFECYCLE_H_
