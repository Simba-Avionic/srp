/**
 * @file i2c_service.h
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef MW_I2C_SERVICE_SERVICE_I2C_SERVICE_H_
#define MW_I2C_SERVICE_SERVICE_I2C_SERVICE_H_
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ara/exec/adaptive_application.h"
#include "communication-core/sockets/stream_ipc_socket.h"
#include "core/i2c/i2c_driver.hpp"
#include "mw/i2c_service/data/header.h"
namespace srp {
namespace mw {
class I2CService final : public ara::exec::AdaptiveApplication {
 private:
  const ara::log::Logger& i2c_logger_;
  std::mutex i2c_mtx;
  std::shared_ptr<core::i2c::II2CDriver> i2c_;
  std::unique_ptr<srp::bindings::com::soc::ISocketStream> sock_;

 protected:
  core::ErrorCode Init(
      std::shared_ptr<core::i2c::II2CDriver> i2c,
      std::unique_ptr<srp::bindings::com::soc::ISocketStream> socket);
  std::vector<uint8_t> RxCallback(const std::string& ip,
                                  const std::uint16_t& port,
                                  const std::vector<std::uint8_t> data);
  std::optional<std::vector<uint8_t>> ReadWrite(
      const std::vector<uint8_t>& payload,
      std::shared_ptr<i2c::Header> headerPtr);
  std::optional<std::vector<uint8_t>> WriteRead(
      const std::vector<uint8_t>& payload,
      std::shared_ptr<i2c::Header> headerPtr);
  std::vector<uint8_t> ActionLogic(
      const std::shared_ptr<srp::i2c::Header> headerPtr,
      std::optional<std::vector<uint8_t>> payload);

 public:
  int Run(const std::stop_token& token) override;
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                     parms) override;
  I2CService();
};
}  // namespace mw
}  // namespace srp

#endif  // MW_I2C_SERVICE_SERVICE_I2C_SERVICE_H_
