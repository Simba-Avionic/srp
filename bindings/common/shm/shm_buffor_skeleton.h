/**
 * @file shm_skeleton.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef BINDINGS_COMMON_SHM_SHM_BUFFOR_SKELETON_H_
#define BINDINGS_COMMON_SHM_SHM_BUFFOR_SKELETON_H_

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <bit>

#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
namespace shm {
template <std::size_t buff_size>
class ShmBufforSkeleton final {
 private:
  const ara::core::InstanceSpecifier instance_specifier_;
  int shm_des{0};
  const int mode = S_IRWXU | S_IRWXG;
  uint8_t* handler = nullptr;

 public:
  explicit ShmBufforSkeleton(
      const ara::core::InstanceSpecifier& instance_specifier)
      : instance_specifier_{instance_specifier} {
    shm_unlink(instance_specifier_.ToString().c_str());
  }

  ShmBufforSkeleton(ShmBufforSkeleton&) = delete;
  ShmBufforSkeleton(ShmBufforSkeleton&&) = delete;
  ShmBufforSkeleton& operator=(ShmBufforSkeleton&) = delete;
  ShmBufforSkeleton& operator=(ShmBufforSkeleton&&) = delete;

  ara::core::Result<void> OfferService() noexcept {
    shm_des = shm_open(instance_specifier_.ToString().c_str(),
                       O_CREAT | O_RDWR | O_TRUNC, mode);
    if (shm_des <= 0) {
      return MakeErrorCode(ara::com::ComErrc::kNetworkBindingFailure, "");
    }
    if (ftruncate(shm_des, buff_size) == -1) {
      return MakeErrorCode(ara::com::ComErrc::kFieldValueIsNotValid, "");
    }
    handler = reinterpret_cast<uint8_t*>(
        mmap(NULL, buff_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_des, 0));
    if (handler == MAP_FAILED) {
      return MakeErrorCode(ara::com::ComErrc::kIllegalUseOfAllocate, "");
    }

    return {};
  }

  uint8_t* GetNewSamplesPointer() { return this->handler; }

  ~ShmBufforSkeleton() {
    shmctl(shm_des, IPC_RMID, nullptr);
    shm_unlink(instance_specifier_.ToString().c_str());
  }
};
}  // namespace shm
}  // namespace com
}  // namespace ara
#endif  // BINDINGS_COMMON_SHM_SHM_BUFFOR_SKELETON_H_
