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
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "core/application/application_mw.h"
#include "communication-core/sockets/stream_ipc_socket.h"
#include "core/i2c/i2cdriver.hpp"
#include "mw/i2c_service/data/header.h"
namespace simba {
namespace mw {
class I2CService : public core::ApplicationMW {
 private:
    std::mutex i2c_mtx;
    std::unique_ptr<core::i2c::II2CDriver> i2c_;
    std::unique_ptr<com::soc::ISocketStream> sock_;

 protected:
    core::ErrorCode Init(std::unique_ptr<core::i2c::II2CDriver> i2c, std::unique_ptr<com::soc::ISocketStream> socket);
    std::vector<uint8_t> RxCallback(const std::string& ip, const std::uint16_t& port,
                                         const std::vector<std::uint8_t> data);
    std::optional<std::vector<uint8_t>> ReadWrite(
            const std::vector<uint8_t> &payload, std::shared_ptr<i2c::Header> headerPtr);
    std::optional<std::vector<uint8_t>> WriteRead(const std::vector<uint8_t> &payload,
                                                          std::shared_ptr<i2c::Header> headerPtr);
    std::vector<uint8_t> ActionLogic(const std::shared_ptr<simba::i2c::Header> headerPtr,
                                                   std::optional<std::vector<uint8_t>> payload);

 public:
  core::ErrorCode Run(std::stop_token token) final;
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;
};
}  // namespace mw
}  // namespace simba

#endif  // MW_I2C_SERVICE_SERVICE_I2C_SERVICE_H_
