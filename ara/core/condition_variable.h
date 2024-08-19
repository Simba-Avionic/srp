#ifndef ARA_CORE_CONDITION_VARIABLE_H_
#define ARA_CORE_CONDITION_VARIABLE_H_

#include <pthread.h>

#include <chrono>
#include <functional>

#include "ara/core/condition_variable_type.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"

namespace ara {
namespace core {

class ConditionVariable final {
 public:
  using CheckCallback = std::function<bool()>;
  template <class Rep, class Period>
  using TimeDuration = std::chrono::duration<Rep, Period>;

 private:
  struct shm_object_t {
    pthread_cond_t cv_;
    pthread_mutex_t mutex_;
  };

  const ConditionVariableMode mode_;
  const InstanceSpecifier instance_;
  ConditionVariable::shm_object_t* shm_object_;

 public:
  ConditionVariable(
      const InstanceSpecifier& instance,
      ConditionVariableMode mode = ConditionVariableMode::kInternal);

  ConditionVariable(ConditionVariable&) = delete;
  ConditionVariable(ConditionVariable&&) = delete;
  ConditionVariable& operator=(ConditionVariable&) = delete;
  ConditionVariable& operator=(ConditionVariable&&) = delete;

  ~ConditionVariable();

  ara::core::Result<void> Offer();
  ara::core::Result<void> Find();

  void StopOffer();
  void StopFind();

  void NotifyOne();
  void NotifyAll();

  bool Wait(CheckCallback callback);
  template <class Rep, class Period>
  bool WaitFor(CheckCallback callback,
               TimeDuration<Rep, Period> time_duration) {}
};

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_CONDITION_VARIABLE_H_
