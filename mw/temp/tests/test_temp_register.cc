/**
 * @file test_temp_register.cc
 * @brief Register/Subscribe odrzucaja zle physical_id (F-26)
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <optional>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "mw/temp/controller/temp_controller.h"

namespace srp {
namespace mw {
namespace temp {

class TestWrapper : public TempController {
 public:
  std::optional<std::vector<uint8_t>> TestSubscribe(const std::string& name) { return Subscribe(name); }
};

TEST(TempControllerRegisterTest, BadLengthPhysicalIdIsRejected) {
  TestWrapper controller;
  // 13 x 0xFF z pustego EEPROM: "28-" + 13 znakow = 16 != 15
  const std::string erased_eeprom = "28-" + std::string(13, static_cast<char>(0xFF));
  for (const std::string& id : {std::string(""), std::string("28-123"), erased_eeprom,
                                std::string("28-0b2554dfd2cc0")}) {
    EXPECT_FALSE(controller.TestSubscribe(id).has_value()) << id;
    EXPECT_FALSE(controller.Register(id).has_value()) << id;
  }
}

}  // namespace temp
}  // namespace mw
}  // namespace srp
