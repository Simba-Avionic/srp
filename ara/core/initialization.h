/**
 * @file initialization.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_CORE_INITIALIZATION_H_
#define ARA_CORE_INITIALIZATION_H_
#include "ara/core/result.h"

namespace ara {
namespace core {
Result<void> Initialize() noexcept;

Result<void> Deinitialize() noexcept;
}  // namespace core
}  // namespace ara
#endif  // ARA_CORE_INITIALIZATION_H_
