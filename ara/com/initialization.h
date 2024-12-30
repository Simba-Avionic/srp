/**
 * @file initialization.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_INITIALIZATION_H_
#define ARA_COM_INITIALIZATION_H_
#include "ara/core/result.h"

namespace ara {
namespace com {
ara::core::Result<void> Initialize(const uint32_t& app_id) noexcept;

ara::core::Result<void> Deinitialize() noexcept;
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_INITIALIZATION_H_
