/**
 * @file condition_variable_type.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_CORE_CONDITION_VARIABLE_TYPE_H_
#define ARA_CORE_CONDITION_VARIABLE_TYPE_H_

#include <cstdint>

namespace ara {
namespace core {
enum ConditionVariableMode : std::uint32_t { kInternal = 0, kExternal = 1 };
}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_CONDITION_VARIABLE_TYPE_H_
