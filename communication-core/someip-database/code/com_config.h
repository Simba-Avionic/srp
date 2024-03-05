/**
 * @file com_config.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_COM_CONFIG_H_
#define COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_COM_CONFIG_H_

#include <fstream>
#include <memory>
#include <optional>
#include <string>

#include "communication-core/someip-database/code/config_db.h"
#include "communication-core/someip-database/code/config_db_parser.h"

namespace simba {
namespace com {
namespace config {
class ComConfig {
 private:
  static std::shared_ptr<someip::objects::ConfigDb> config_db;

 public:
  ~ComConfig() = default;
  static std::optional<someip::objects::Endpoint> FindObject(
      const std::string& key);
  static void SetConfigDb(std::shared_ptr<someip::objects::ConfigDb> config);
  static void InitConfigDb(const std::string& path);
};
}  // namespace config
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_COM_CONFIG_H_
