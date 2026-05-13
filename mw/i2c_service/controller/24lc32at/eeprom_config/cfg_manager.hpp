/**
 * @file cfg_manager.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Manager konfiguracji przechowywanej w pamięci EEPROM 24LC32AT.
 * @version 0.1
 * @date 2026-05-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef MW_I2C_SERVICE_CONTROLLER_24LC32AT_EEPROM_CONFIG_CFG_MANAGER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_24LC32AT_EEPROM_CONFIG_CFG_MANAGER_HPP_

#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>

#include "core/common/error_code.h"
#include "mw/i2c_service/controller/24lc32at/controller.hpp"

namespace srp {
namespace eeprom {

struct [[gnu::packed]] EEPROM_config {
    float pca9685_XO_corelation;
    char board_temp1_id[13];
    char board_temp2_id[13];
    char board_temp3_id[13];
};

static_assert(sizeof(EEPROM_config) == 43,
              "EEPROM_config must be packed (expected 43 bytes layout)");

namespace detail {
    static constexpr uint16_t kConfig_start_address = 0x0000;
    static constexpr std::size_t kConfig_size = sizeof(EEPROM_config);
}  // namespace detail

class ConfigManager {
 private:
  i2c::EEPROM24LC32AT eeprom;

 public:
  /**
   * @brief Inicjalizuje warstwę EEPROM (otwiera socket IPC, weryfikuje chip).
   * @return kOk jeśli się udało; inaczej kInitializeError.
   */
  core::ErrorCode Init() {
    return eeprom.Init();
  }

  /**
   * @brief Pobiera konfigurację z EEPROM.
   * @return std::optional z danymi lub nullopt w przypadku błędu magistrali.
   */
  std::optional<EEPROM_config> GetConfig() {
    auto res = eeprom.ReadBuffer(detail::kConfig_start_address, detail::kConfig_size);
    if (!res.has_value() || res.value().size() != detail::kConfig_size) {
        return std::nullopt;
    }
    EEPROM_config cfg;
    std::memcpy(&cfg, res.value().data(), detail::kConfig_size);
    return cfg;
  }

  /**
   * @brief Zapisuje konfigurację do EEPROM (auto-chunking po stronach 32 B
   *        zgodnie z 24LC32A datasheet sekcja 6.2).
   */
  core::ErrorCode SetConfig(const EEPROM_config& cfg) {
    std::vector<uint8_t> buffer(detail::kConfig_size);
    std::memcpy(buffer.data(), &cfg, detail::kConfig_size);
    return eeprom.WriteBuffer(detail::kConfig_start_address, buffer);
  }
};

}  // namespace eeprom
}  // namespace srp

#endif  // MW_I2C_SERVICE_CONTROLLER_24LC32AT_EEPROM_CONFIG_CFG_MANAGER_HPP_
