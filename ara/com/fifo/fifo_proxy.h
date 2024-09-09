/**
 * @file fifo_proxy.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_FIFO_FIFO_PROXY_H_
#define ARA_COM_FIFO_FIFO_PROXY_H_

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include <array>

#include "ara/com/com_error_domain.h"
#include "ara/com/type_converter.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
namespace fifo {
template <typename fifo_type>
class FifoProxy {
 private:
  const ara::core::InstanceSpecifier instance_;

 public:
  explicit FifoProxy(const ara::core::InstanceSpecifier& instance)
      : instance_{instance} {}
  ara::core::Result<void> FindService() {
    // mkfifo(("/tmp/" + instance_.ToString()).c_str(), 0666);
    return {};
  }
  ara::core::Result<void> StopOfferService() { return {}; }
  ara::core::Result<fifo_type> GetNewSamples() {
    int fd =
        open(("/tmp/" + instance_.ToString()).c_str(), O_RDONLY);
    if (fd < 0) {
      return ara::com::MakeErrorCode(ara::com::ComErrc::kIllegalUseOfAllocate,
                                     "fifo not exist!");
    }
    std::array<uint8_t, 1025> buffor{};
    // const auto data = ara::com::Convert2Vector<fifo_type>::Conv(value);
    const auto res = read(fd, buffor.data(), 1025);
    close(fd);
    if (res > 0) {
      const auto data = ara::com::Convert<fifo_type>::Conv(
          std::vector<uint8_t>{buffor.begin(), buffor.begin() + res});
      if (data.has_value()) {
        return data.value();
      }
      return ara::com::MakeErrorCode(ara::com::ComErrc::kPeerIsUnreachable, " Error 1");
    }
    return ara::com::MakeErrorCode(ara::com::ComErrc::kCommunicationStackError, " Error 2");
  }
};
}  // namespace fifo
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_FIFO_FIFO_PROXY_H_
