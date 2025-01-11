/**
 * @file types.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ARA_COM_TYPES_H_
#define ARA_COM_TYPES_H_
#include <functional>
#include <memory>

namespace ara {
namespace com {
using EventReceiveHandler = std::function<void()>;
using FieldReceiveHandler = std::function<void()>;
template <typename T>
using FindServiceHandler = std::function<void(std::shared_ptr<T> handler)>;
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_TYPES_H_
