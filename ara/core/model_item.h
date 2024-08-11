/**
 * @file model_item.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_CORE_MODEL_ITEM_H_
#define ARA_CORE_MODEL_ITEM_H_
#include <optional>
#include <variant>

#include "ara/core/model/com_model_item.h"
#include "ara/core/model/diag_model_item.h"

namespace ara {
namespace core {
class ModelItem {
 private:
  std::variant<model::ModelSomeIp, model::ModelDtc, model::ModelUds> value_;

 public:
  template <typename T>
  ModelItem(const T& value) : value_{value} {}  // NOLINT
  template <typename T>
  std::optional<T> GetValue() const;
  ~ModelItem() = default;
};
}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_MODEL_ITEM_H_
