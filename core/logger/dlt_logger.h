/**
 * @file dlt_logger.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef CORE_LOGGER_DLT_LOGGER_H_
#define CORE_LOGGER_DLT_LOGGER_H_
#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "communication-core/sockets/ipc_socket.h"
#include "core/common/error_code.h"
#include "core/common/wait_queue.h"
#include "core/logger/ILogger.h"
namespace simba {
namespace core {
namespace logger {
class DltLogger final : public ILogger {
 private:
  uint8_t drop_number{0};
  com::soc::IpcSocket ipc_soc{};
  std::unique_ptr<std::jthread> thread;
  void Loop(std::stop_token token);
  core::WaitQueue<std::vector<uint8_t>, 20> q{};

 public:
  simba::core::ErrorCode Debug(const std::string& log) override;
  simba::core::ErrorCode Info(const std::string& log) override;
  simba::core::ErrorCode Warning(const std::string& log) override;
  simba::core::ErrorCode Error(const std::string& log) override;
  DltLogger(/* args */);
  ~DltLogger();
};

}  // namespace logger
}  // namespace core
}  // namespace simba

#endif  // CORE_LOGGER_DLT_LOGGER_H_
