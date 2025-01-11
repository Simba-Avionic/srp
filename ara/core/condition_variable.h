/**
 * @file condition_variable.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_CORE_CONDITION_VARIABLE_H_
#define ARA_CORE_CONDITION_VARIABLE_H_

#include <pthread.h>

#include <chrono>  // NOLINT
#include <functional>

#include "bindings/common/shm/shm_proxy.h"
#include "bindings/common/shm/shm_skeleton.h"
#include "ara/core/condition_variable_type.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"

namespace ara {
namespace core {

class ConditionVariableProxy final {
 public:
  using CheckCallback = std::function<bool()>;
  template <class Rep, class Period>
  using TimeDuration = std::chrono::duration<Rep, Period>;

 private:
  struct shm_object_t {
    pthread_cond_t cv_;
    pthread_mutex_t mutex_;
    uint8_t status_;
  };
  const InstanceSpecifier instance_;
  ConditionVariableProxy::shm_object_t* shm_object_;
  ara::com::shm::ShmProxy<ConditionVariableProxy::shm_object_t> shm_;

 public:
  ConditionVariableProxy(const InstanceSpecifier& instance);

  ConditionVariableProxy(ConditionVariableProxy&) = delete;
  ConditionVariableProxy(ConditionVariableProxy&&) = delete;
  ConditionVariableProxy& operator=(ConditionVariableProxy&) = delete;
  ConditionVariableProxy& operator=(ConditionVariableProxy&&) = delete;

  ~ConditionVariableProxy() = default;

  // ara::core::Result<void> Offer();
  ara::core::Result<void> Find();

  ara::core::Result<void> NotifyOne();
  ara::core::Result<void> NotifyAll();

  bool Wait(CheckCallback callback);
  bool Wait();
  template <class Rep, class Period>
  bool WaitFor(CheckCallback callback,
               TimeDuration<Rep, Period> time_duration) {
    return false;
  }
};

class ConditionVariableSkeleton final {
 public:
  using CheckCallback = std::function<bool()>;
  template <class Rep, class Period>
  using TimeDuration = std::chrono::duration<Rep, Period>;

 private:
  struct shm_object_t {
    pthread_cond_t cv_;
    pthread_mutex_t mutex_;
    uint8_t status_;
  };
  const InstanceSpecifier instance_;

  ConditionVariableSkeleton::shm_object_t* shm_object_;
  pthread_mutexattr_t attrmutex;
  pthread_condattr_t attrcond;
  ara::com::shm::ShmSkeleton<ConditionVariableSkeleton::shm_object_t> shm_;

 public:
  ConditionVariableSkeleton(const InstanceSpecifier& instance);

  ConditionVariableSkeleton(ConditionVariableSkeleton&) = delete;
  ConditionVariableSkeleton(ConditionVariableSkeleton&&) = delete;
  ConditionVariableSkeleton& operator=(ConditionVariableSkeleton&) = delete;
  ConditionVariableSkeleton& operator=(ConditionVariableSkeleton&&) = delete;

  ~ConditionVariableSkeleton();

  ara::core::Result<void> Offer();

  ara::core::Result<void> NotifyOne();
  ara::core::Result<void> NotifyAll();

  bool Wait(CheckCallback callback);
  bool Wait();
  template <class Rep, class Period>
  bool WaitFor(CheckCallback callback,
               TimeDuration<Rep, Period> time_duration) {
    return false;
  }
};

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_CONDITION_VARIABLE_H_
