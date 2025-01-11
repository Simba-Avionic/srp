/**
 * @file proxy.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ARA_COM_PROXY_PROXY_H_
#define ARA_COM_PROXY_PROXY_H_
#include <functional>
#include <memory>

#include "ara/core/instance_specifier.h"
namespace ara {
namespace com {
template <typename T>
class Proxy {
 private:
  const ara::core::InstanceSpecifier instance_;

 public:
  using Handler = std::shared_ptr<T>;
  using FindCallback = std::function<void(Handler)>;

  explicit Proxy(const ara::core::InstanceSpecifier instance) noexcept
      : instance_{instance} {}

  explicit Proxy(Proxy&&) = delete;
  explicit Proxy(Proxy&) = delete;
  Proxy operator=(Proxy&) = delete;
  Proxy operator=(Proxy&&) = delete;

  ~IProxy() noexcept {}

  /**
   * @brief This method will trigger finding loop
   *
   * @param callback This callback will be call when Service will be found
   */
  void StartFindService(const FindCallback&& callback) noexcept {}
  /**
   * @brief This method will stop find loop
   *
   */
  void StopFindService() noexcept {}
};
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_PROXY_PROXY_H_
