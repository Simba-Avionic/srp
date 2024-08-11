#ifndef ARA_CORE_CONDITION_VARIABLE_TYPE_H_
#define ARA_CORE_CONDITION_VARIABLE_TYPE_H_

#include <cstdint>

namespace ara {
namespace core {
enum ConditionVariableMode : std::uint32_t { kInternal = 0, kExternal = 1 };
}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_CONDITION_VARIABLE_TYPE_H_
