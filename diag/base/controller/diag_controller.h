/**
 * @file diag_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef DIAG_BASE_CONTROLLER_DIAG_CONTROLLER_H_
#define DIAG_BASE_CONTROLLER_DIAG_CONTROLLER_H_
#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "communication-core/sockets/Isocket.h"
#include "core/common/error_code.h"
#include "core/results/result.h"
#include "diag/base/controller/diag_transfer.h"
#include "diag/base/controller/idiag_controller.h"
#include "diag/base/data/data_structure.h"
namespace simba {
namespace diag {

class DiagController : public IDiagController {
 protected:
  /// @brief Diagnostic service id number
  const uint16_t service_id_;
  /// @brief Current transfer id number
  std::atomic<uint16_t> transfer_id{0};
  /// @brief Pointer at socket transfer (IPC)
  const std::unique_ptr<com::soc::ISocket> socket_driver;
  /// @brief Vector with all sended transfer (it'could be save as one element)
  std::vector<std::shared_ptr<DiagTransfer>> transfer_list{};
  /// @brief List with callbacks
  std::unordered_map<std::uint8_t, DiagMethod> callback_list{};
  /// @brief Callback for socket driver
  /// @param ip Not used
  /// @param port Not used
  /// @param payload Not used
  void RxCallback(const std::string& ip, const std::uint16_t& port,
                  std::vector<std::uint8_t> payload);
  /// @brief This method prepare and send error
  /// @param req request without any changes
  void SendError(const data::DataStructure req);
  /// @brief This method send req or res to diagnostic serwice
  /// @param req request or response
  /// @return OK, if the message was sent correctly
  simba::core::ErrorCode Send(const data::DataStructure& req);
  /// @brief This function proceed request msg
  /// @param req request
  void Response(const data::DataStructure req);
  /// @brief This function proceed response msg
  /// @param req request
  void Request(const data::DataStructure req);
  /// @brief return and increment transfer id
  /// @return transfer id
  const uint16_t GetTransferId();

 public:
  simba::core::ErrorCode AddMethod(const uint8_t diag_id, DiagMethod callback,
                                   const DiagMethodType method_type) override;
  simba::core::ErrorCode Init() override;
  simba::core::Result<std::vector<uint8_t>> Read(
      const uint16_t service_id, const uint8_t diag_id) override;
  simba::core::ErrorCode Write(const uint16_t service_id, const uint8_t diag_id,
                               const std::vector<uint8_t> payload) override;
  simba::core::Result<std::vector<uint8_t>> Job(
      const uint16_t service_id, const uint8_t diag_id,
      const std::vector<uint8_t> payload) override;
  DiagController(const uint16_t service_id,
                 std::unique_ptr<com::soc::ISocket> soc);
  ~DiagController() { this->callback_list.erase(this->callback_list.begin(), this->callback_list.end());}

};
}  // namespace diag
}  // namespace simba
#endif  // DIAG_BASE_CONTROLLER_DIAG_CONTROLLER_H_
