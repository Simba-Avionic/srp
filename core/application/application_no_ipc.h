/**
 * @file Iapplication.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief Interface for all Simba application
 * @version 1.0
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CORE_APPLICATION_APPLICATION_NO_IPC_H_
#define CORE_APPLICATION_APPLICATION_NO_IPC_H_
#include "core/application/application_common.h"
namespace simba {
namespace core {
class ApplicationNoIPC : public ApplicationCommon {
 public:
  virtual ~ApplicationNoIPC() = default;
};
}  // namespace core
}  // namespace simba
#endif  // CORE_APPLICATION_APPLICATION_NO_IPC_H_
