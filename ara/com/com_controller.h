/**
 * @file com_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_COM_CONTROLLER_H_
#define ARA_COM_COM_CONTROLLER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ara/com/i_com_client.h"
#include "ara/com/socket/proccess_socket.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
class ComController {
 private:
  std::unordered_map<IComClient::MsgType, std::shared_ptr<IComClient>>
      handlers_{};

  ComController(/* args */);
  std::unique_ptr<ProccessSocket> ipc_soc_;
  void IpcRxCallback(const uint32_t pid, const std::vector<uint8_t>& payload);
  void SendCallback(uint32_t pid, const std::vector<uint8_t>& payload,
                    IComClient::MsgType type);
  void SendCallbackTo(const std::string& desc,
                      const std::vector<uint8_t>& payload,
                      IComClient::MsgType type);

 public:
  ara::core::Result<void> Init() noexcept;
  bool AddHandler(IComClient::MsgType msg_type,
                  std::shared_ptr<IComClient> handler_);
  static ComController& GetInstance() noexcept;
  ~ComController();
};

}  // namespace com
}  // namespace ara

#endif  // ARA_COM_COM_CONTROLLER_H_
