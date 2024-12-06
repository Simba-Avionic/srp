/**
 * @file i_com_client.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_I_COM_CLIENT_H_
#define ARA_COM_I_COM_CLIENT_H_

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace ara {
namespace com {
class IComClient {
 public:
  enum MsgType : uint8_t { kSomeIp = 0x01, kDiag = 0x02 };
  using SendCallback = std::function<bool(
      uint32_t pid, const std::vector<uint8_t>& payload, MsgType type)>;
  using SendCallbackTo =
      std::function<bool(const std::string& desc,
                         const std::vector<uint8_t>& payload, MsgType type)>;

 protected:
  SendCallback send_callback_;
  SendCallbackTo send_callback_to_;

 public:
  void SetSendCallback(SendCallback&& callback, SendCallbackTo callback_to) {
    send_callback_ = callback;
    send_callback_to_ = callback_to;
  }

  virtual void HandleNewMsg(uint32_t pid,
                            const std::vector<uint8_t>& payload) noexcept = 0;
  virtual ~IComClient() = default;
};
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_I_COM_CLIENT_H_
