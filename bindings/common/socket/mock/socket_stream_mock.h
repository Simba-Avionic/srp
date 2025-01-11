/**
 * @file Isocket_stream.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef BINDINGS_COMMON_SOCKET_MOCK_SOCKET_STREAM_MOCK_H_
#define BINDINGS_COMMON_SOCKET_MOCK_SOCKET_STREAM_MOCK_H_

#include <gmock/gmock.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "ara/core/result.h"
#include "bindings/common/socket/Isocket_stream.h"
namespace ara {
namespace com {
namespace soc {
class SocketStreamMock : public ISocketStream {
 public:
  MOCK_METHOD(ara::core::Result<void>, Init, (const std::string& socket_path),
              (override));
  MOCK_METHOD(void, SetRXCallback, (RXCallbackStream && callback), (override));
  MOCK_METHOD(std::optional<std::vector<uint8_t>>, Transmit,
              (const std::string& socket_path,
               std::vector<std::uint8_t> payload),
              (override));
  MOCK_METHOD(void, StartRXThread, (), (override));
  MOCK_METHOD(void, StopRXThread, (), (override));
  SocketStreamMock() = default;
};

class FacadeSocketStream : public ISocketStream {
 private:
  std::shared_ptr<SocketStreamMock> mock_;

 public:
  explicit FacadeSocketStream(std::shared_ptr<SocketStreamMock> mock)
      : mock_{mock} {}
  ara::core::Result<void> Init(const std::string& socket_path) {
    return mock_->Init(socket_path);
  }
  void SetRXCallback(RXCallbackStream&& callback) {
    mock_->SetRXCallback(std::move(callback));
  }
  std::optional<std::vector<uint8_t>> Transmit(
      const std::string& socket_path, std::vector<std::uint8_t> payload) {
    return mock_->Transmit(socket_path, payload);
  }
  void StartRXThread() { mock_->StartRXThread(); }
  void StopRXThread() { mock_->StopRXThread(); }
};

}  // namespace soc
}  // namespace com
}  // namespace ara

#endif  // BINDINGS_COMMON_SOCKET_MOCK_SOCKET_STREAM_MOCK_H_"
