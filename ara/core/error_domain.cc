/**
 * @file error_domain.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/core/error_domain.h"

namespace ara {
namespace core {
/**
 * @brief Encapsulation of an error domain.
 * An error domain is the controlling entity for ErrorCode’s error code values,
 * and defines the mapping of such error code values to textual representations.
 * This class is a literal type, and subclasses are strongly advised to be
 * literal types as well.
 *
 */
ErrorDomain::ErrorDomain(ErrorDomain::IdType id) noexcept : id_{id} {}
bool ErrorDomain::operator==(const ErrorDomain& other) const noexcept {
  return id_ == other.id_;
}
bool ErrorDomain::operator!=(const ErrorDomain& other) const noexcept {
  return id_ != other.id_;
}
ErrorDomain::IdType ErrorDomain::Id() const noexcept { return id_; }

}  // namespace core
}  // namespace ara
