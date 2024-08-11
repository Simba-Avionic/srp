/**
 * @file com_model_item.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_CORE_MODEL_COM_MODEL_ITEM_H_
#define ARA_CORE_MODEL_COM_MODEL_ITEM_H_

#include <string>

#include "ara/core/result.h"
namespace ara {
namespace core {
namespace model {

class ModelSomeIp {
 private:
  const std::string com_id_;
  const std::uint32_t port_;
  const std::uint16_t instance_;
  const std::uint8_t major_version_;
  const std::uint32_t minor_version_;

 public:
  ModelSomeIp(const std::string& com_id, const std::uint32_t port,
              const std::uint16_t instance, const std::uint8_t major_version,
              const std::uint32_t minor_version)
      : com_id_{com_id},
        port_{port},
        instance_{instance},
        major_version_{major_version},
        minor_version_{minor_version} {}

  static core::Result<ModelSomeIp> CreatItem(
      const std::string& com_id, const std::uint32_t port,
      const std::uint16_t instance, const std::uint8_t major_version,
      const std::uint32_t minor_version) {
    return ModelSomeIp(com_id, port, instance, major_version, minor_version);
  }
};
}  // namespace model
}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_MODEL_COM_MODEL_ITEM_H_
