/**
 * @file remote_log_sink.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_LOG_SINK_REMOTE_LOG_SINK_H_
#define ARA_LOG_SINK_REMOTE_LOG_SINK_H_

#include <memory>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "ara/log/sink/log_sink.h"
#include "communication-core/sockets/ipc_socket.h"
#include "core/common/wait_queue.h"

namespace ara {
namespace log {
namespace sink {
class RemoteLogSink : public LogSink {
 private:
  uint8_t drop_number{0};
  simba::com::soc::IpcSocket ipc_soc{};
  std::unique_ptr<std::jthread> thread;
  void Loop(std::stop_token token);
  simba::core::WaitQueue<std::vector<uint8_t>, 400> q{};
  uint32_t last{0};

 public:
  RemoteLogSink();
  ~RemoteLogSink();
  void Log(const LogStream &logStream) override;
};
}  // namespace sink
}  // namespace log
}  // namespace ara

#endif  // ARA_LOG_SINK_REMOTE_LOG_SINK_H_
