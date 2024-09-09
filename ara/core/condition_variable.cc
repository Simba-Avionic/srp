/**
 * @file condition_variable.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/core/condition_variable.h"

#include "ara/core/core_error_domain.h"
#include <thread>  // NOLINT

namespace ara {
namespace core {

ConditionVariableProxy::ConditionVariableProxy(
    const InstanceSpecifier& instance)
    : instance_{instance}, shm_object_{nullptr}, shm_{instance} {}

ara::core::Result<void> ConditionVariableProxy::Find() {
  {
    const auto res = shm_.FindService();
    if (!res.HasValue()) {
      return res.Error();
    }
  }
  {
    const auto res = shm_.GetNewSamplesPointer();
    if (!res.HasValue()) {
      return res.Error();
    }
    shm_object_ = res.Value();
  }
  return {};
}

ara::core::Result<void> ConditionVariableProxy::NotifyOne() {
  if (shm_object_ == nullptr) {
    return MakeErrorCode(ara::core::CoreErrc::kInvalidArgument, "");
  }
  pthread_mutex_lock(&shm_object_->mutex_);
  pthread_cond_signal(&shm_object_->cv_);
  pthread_mutex_unlock(&shm_object_->mutex_);
  return {};
}
ara::core::Result<void> ConditionVariableProxy::NotifyAll() {
  if (shm_object_ == nullptr) {
    return MakeErrorCode(ara::core::CoreErrc::kInvalidArgument, "");
  }
  pthread_mutex_lock(&shm_object_->mutex_);
  pthread_cond_broadcast(&shm_object_->cv_);
  pthread_mutex_unlock(&shm_object_->mutex_);
  return {};
}
bool ConditionVariableProxy::Wait() {
  if (shm_object_ == nullptr) {
    return false;
  }
  pthread_mutex_lock(&shm_object_->mutex_);
  pthread_cond_wait(&shm_object_->cv_, &shm_object_->mutex_);
  pthread_mutex_unlock(&shm_object_->mutex_);
  if (shm_object_->status_ == 0) {
    return true;
  }
  return false;
}
bool ConditionVariableProxy::Wait(CheckCallback callback) {
  if (shm_object_ == nullptr) {
    return false;
  }
  while (true) {
    if (callback()) {
      return true;
    }
    pthread_mutex_lock(&shm_object_->mutex_);
    pthread_cond_wait(&shm_object_->cv_, &shm_object_->mutex_);
    pthread_mutex_unlock(&shm_object_->mutex_);
  }
  return true;
}

ConditionVariableSkeleton::ConditionVariableSkeleton(
    const InstanceSpecifier& instance)
    : instance_{instance}, shm_object_{nullptr}, shm_{instance} {}

ara::core::Result<void> ConditionVariableSkeleton::Offer() {
  {
    const auto res = shm_.OfferService();
    if (!res.HasValue()) {
      return res.Error();
    }
  }
  {
    const auto res = shm_.GetNewSamplesPointer();
    if (!res.HasValue()) {
      return res.Error();
    }
    shm_object_ = res.Value();
  }
  pthread_mutexattr_init(&attrmutex);
  pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&shm_object_->mutex_, &attrmutex);

  pthread_condattr_init(&attrcond);
  pthread_condattr_setpshared(&attrcond, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&shm_object_->cv_, &attrcond);
  return {};
}

ara::core::Result<void> ConditionVariableSkeleton::NotifyOne() {
  if (shm_object_ == nullptr) {
    return MakeErrorCode(ara::core::CoreErrc::kInvalidArgument, "");
  }
  pthread_mutex_lock(&shm_object_->mutex_);
  pthread_cond_signal(&shm_object_->cv_);
  pthread_mutex_unlock(&shm_object_->mutex_);
  return {};
}
ara::core::Result<void> ConditionVariableSkeleton::NotifyAll() {
  if (shm_object_ == nullptr) {
    return MakeErrorCode(ara::core::CoreErrc::kInvalidArgument, "");
  }
  pthread_mutex_lock(&shm_object_->mutex_);
  pthread_cond_broadcast(&shm_object_->cv_);
  pthread_mutex_unlock(&shm_object_->mutex_);
  return {};
}
bool ConditionVariableSkeleton::Wait() {
  if (shm_object_ == nullptr) {
    return false;
  }
  pthread_mutex_lock(&shm_object_->mutex_);
  pthread_cond_wait(&shm_object_->cv_, &shm_object_->mutex_);
  pthread_mutex_unlock(&shm_object_->mutex_);
  return true;
}
bool ConditionVariableSkeleton::Wait(CheckCallback callback) {
  if (shm_object_ == nullptr) {
    return false;
  }
  while (true) {
    if (callback()) {
      return true;
    }
    pthread_mutex_lock(&shm_object_->mutex_);
    pthread_cond_wait(&shm_object_->cv_, &shm_object_->mutex_);
    pthread_mutex_unlock(&shm_object_->mutex_);
  }
  return true;
}
ConditionVariableSkeleton::~ConditionVariableSkeleton() {
  if (shm_object_ == nullptr) {
    return;
  }
  shm_object_->status_ = 255;
  this->NotifyAll();
  std::this_thread::sleep_for(std::chrono::milliseconds{100});
  pthread_mutex_destroy(&shm_object_->mutex_);
  pthread_mutexattr_destroy(&attrmutex);
  pthread_cond_destroy(&shm_object_->cv_);
  pthread_condattr_destroy(&attrcond);
}
}  // namespace core
}  // namespace ara
