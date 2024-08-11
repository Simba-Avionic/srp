#include "ara/core/condition_variable.h"

namespace ara {
namespace core {

ConditionVariable::ConditionVariable(
    const InstanceSpecifier& instance,
    ConditionVariableMode mode = ConditionVariableMode::kInternal)
    : mode_{mode}, instance_{instance}, shm_object_{nullptr} {}

ConditionVariable::~ConditionVariable() {}

}  // namespace core
}  // namespace ara
