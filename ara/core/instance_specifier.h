#ifndef ARA_CORE_INSTANCE_SPECIFIER_H_
#define ARA_CORE_INSTANCE_SPECIFIER_H_
#include "ara/core/result.h"
#include "ara/core/string_view.h"
namespace ara {
namespace core {
class InstanceSpecifier final {
 private:
  StringView metaModelIdentifier_;

 public:
  explicit InstanceSpecifier(StringView metaModelIdentifier);
  InstanceSpecifier(const InstanceSpecifier &other);
  InstanceSpecifier(InstanceSpecifier &&other) noexcept;
  InstanceSpecifier &operator=(const InstanceSpecifier &other);
  InstanceSpecifier &operator=(InstanceSpecifier &&other);
  ~InstanceSpecifier() noexcept;
  static Result<InstanceSpecifier> Create(
      StringView metaModelIdentifier) noexcept;
  bool operator==(const InstanceSpecifier &other) const noexcept;
  bool operator==(StringView other) const noexcept;
  bool operator!=(const InstanceSpecifier &other) const noexcept;
  bool operator!=(StringView other) const noexcept;
  bool operator<(const InstanceSpecifier &other) const noexcept;
  StringView ToString() const noexcept;
};
}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_INSTANCE_SPECIFIER_H_
