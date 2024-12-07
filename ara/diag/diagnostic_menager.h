/**
 * @file diagnostic_menager.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_DIAG_DIAGNOSTIC_MENAGER_H_
#define ARA_DIAG_DIAGNOSTIC_MENAGER_H_

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "ara/com/i_com_client.h"
#include "ara/core/result.h"
namespace ara {
namespace diag {
class DiagnosticMenager : public com::IComClient {
 public:
  using DtcUpdateCallback = std::function<void(uint8_t)>;

 protected:
  void HandleNewMsg(uint32_t pid,
                    const std::vector<uint8_t>& payload) noexcept override;
  static std::shared_ptr<DiagnosticMenager> instance_;
  std::unordered_map<uint32_t, DtcUpdateCallback> dtc_handler_list_{};

 public:
  DiagnosticMenager(/* args */);
  ~DiagnosticMenager();
  ara::core::Result<void> RegisterDtcHandler(const uint32_t& id,
                                             DtcUpdateCallback&& callback_);

  static std::shared_ptr<DiagnosticMenager> GetInstance() noexcept;

  ara::core::Result<void> SendDtcUpdate(const uint32_t& id,
                                        const uint8_t& new_status) noexcept;
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DIAGNOSTIC_MENAGER_H_
