/**
 * @file instance_specifier.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/core/instance_specifier.h"

#include <string>

#include "ara/core/core_error_domain.h"

namespace ara {
namespace core {

namespace {
bool is_number(const std::string &s) {
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it)) ++it;
  return !s.empty() && it == s.end();
}
}  // namespace

InstanceSpecifier::InstanceSpecifier(StringView metaModelIdentifier)
    : metaModelIdentifier_{metaModelIdentifier} {}
InstanceSpecifier::InstanceSpecifier(const InstanceSpecifier &other)
    : metaModelIdentifier_{other.metaModelIdentifier_} {}
InstanceSpecifier::InstanceSpecifier(InstanceSpecifier &&other) noexcept
    : metaModelIdentifier_{other.metaModelIdentifier_} {}
InstanceSpecifier &InstanceSpecifier::operator=(
    const InstanceSpecifier &other) {
  this->metaModelIdentifier_ = other.metaModelIdentifier_;
  return *this;
}
InstanceSpecifier &InstanceSpecifier::operator=(InstanceSpecifier &&other) {
  this->metaModelIdentifier_ = other.metaModelIdentifier_;
  return *this;
}
InstanceSpecifier::~InstanceSpecifier() {}
ara::core::Result<InstanceSpecifier> InstanceSpecifier::Create(
    StringView metaModelIdentifier) noexcept {
  if (is_number(metaModelIdentifier)) {
    return MakeErrorCode(CoreErrc::kInvalidMetaModelShortname,
                         metaModelIdentifier + " have numbers");
  }
  return InstanceSpecifier{metaModelIdentifier};
}
bool InstanceSpecifier::operator==(
    const InstanceSpecifier &other) const noexcept {
  return this->metaModelIdentifier_ == other.metaModelIdentifier_;
}
bool InstanceSpecifier::operator==(StringView other) const noexcept {
  return this->metaModelIdentifier_ == other;
}
bool InstanceSpecifier::operator!=(
    const InstanceSpecifier &other) const noexcept {
  return this->metaModelIdentifier_ != other.metaModelIdentifier_;
}
bool InstanceSpecifier::operator!=(StringView other) const noexcept {
  return this->metaModelIdentifier_ != other;
}
bool InstanceSpecifier::operator<(
    const InstanceSpecifier &other) const noexcept {
  return this->metaModelIdentifier_ < other.metaModelIdentifier_;
}
StringView InstanceSpecifier::ToString() const noexcept {
  return this->metaModelIdentifier_;
}
}  // namespace core
}  // namespace ara
