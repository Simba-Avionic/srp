/**
 * @file error_domain.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_CORE_ERROR_DOMAIN_H_
#define ARA_CORE_ERROR_DOMAIN_H_

#include <cstdint>
#include <string>


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

class ErrorCode;

class ErrorDomain {
 public:
  using IdType = std::uint64_t;
  using CodeType = std::int32_t;
  using SupportDataType = std::string;

 private:
  const IdType id_;

 public:
  ErrorDomain(const ErrorDomain&) = delete;
  ErrorDomain(const ErrorDomain&&) = delete;
  explicit ErrorDomain(IdType id) noexcept;

  virtual ~ErrorDomain() noexcept = default;

  ErrorDomain& operator=(const ErrorDomain&) = delete;
  ErrorDomain& operator=(const ErrorDomain&&) = delete;

  bool operator==(const ErrorDomain& other) const noexcept;
  bool operator!=(const ErrorDomain& other) const noexcept;

  IdType Id() const noexcept;

  virtual const std::string Name() const noexcept = 0;
  virtual const std::string Message(CodeType error_code) const noexcept = 0;
};
}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_ERROR_DOMAIN_H_
