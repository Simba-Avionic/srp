/**
 * @file fifo_skeleton.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_FIFO_FIFO_SKELETON_H_
#define ARA_COM_FIFO_FIFO_SKELETON_H_

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ara/com/type_converter.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
namespace fifo {
template <typename fifo_type>
class FifoSkeleton {
 private:
  const ara::core::InstanceSpecifier instance_;

 public:
  explicit FifoSkeleton(const ara::core::InstanceSpecifier& instance)
      : instance_{instance} {}
  ara::core::Result<void> OfferService() {
    mkfifo(("/tmp/" + instance_.ToString()).c_str(), 0666);
    return {};
  }
  ara::core::Result<void> StopOfferService() {
    unlink(("/tmp/" + instance_.ToString()).c_str());
    return {};
  }
  ara::core::Result<void> Send(const fifo_type& value) {
    int fd = open(("/tmp/" + instance_.ToString()).c_str(), O_WRONLY);
    const auto data = ara::com::Convert2Vector<fifo_type>::Conv(value);
    write(fd, data.data(), data.size() + 1);
    close(fd);
    return {};
  }
};
}  // namespace fifo
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_FIFO_FIFO_SKELETON_H_
