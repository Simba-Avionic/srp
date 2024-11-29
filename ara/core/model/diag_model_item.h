/**
 * @file diag_model_item.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_CORE_MODEL_DIAG_MODEL_ITEM_H_
#define ARA_CORE_MODEL_DIAG_MODEL_ITEM_H_

#include <cstdint>
#include <string>

namespace ara {
namespace core {
namespace model {
class ModelUds {
 public:
  enum Direction { kOut = 0, kIn };
  const std::uint8_t service_id_;
  const std::uint16_t sub_service_id_;
  const std::string com_id_;
  const Direction com_direction_;

 public:
  ModelUds(const std::uint8_t service_id, const std::uint16_t sub_service_id,
           const std::string& com_id, const Direction com_direction)
      : service_id_{service_id},
        sub_service_id_{sub_service_id},
        com_id_{com_id},
        com_direction_{com_direction} {}

  static core::Result<ModelUds> CreatItem(const std::uint8_t service_id,
                                          const std::uint16_t sub_service_id,
                                          const std::string com_id,
                                          const Direction com_direction) {
    return ModelUds{service_id, sub_service_id, com_id, com_direction};
  }
};

class ModelDtc {
 public:
  const std::string name_;
  const std::uint32_t id_;
  const std::string event_path_;

 public:
  ModelDtc(const std::string& name, const std::uint32_t id,
           const std::string& event_path)
      : name_{name}, id_{id}, event_path_{event_path} {}

  static core::Result<ModelDtc> CreatItem(const std::string& name,
                                          const std::uint32_t id) {
    return ModelDtc{name, id, ""};
  }
  static core::Result<ModelDtc> CreatItem(const std::string& name,
                                          const std::uint32_t id,
                                          const std::string& event_path) {
    return ModelDtc{name, id, event_path};
  }
};

}  // namespace model
}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_MODEL_DIAG_MODEL_ITEM_H_
