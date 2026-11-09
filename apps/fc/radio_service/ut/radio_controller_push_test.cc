/**
 * @file radio_controller_push_test.cc
 * @brief Push() nie blokuje i odrzuca ramke, gdy UART nie jest otwarty (F-35)
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <cstdint>
#include <vector>

#include "gtest/gtest.h"
#include "apps/fc/radio_service/radio_controller.hpp"

namespace srp {
namespace apps {
namespace radio {

TEST(RadioControllerPushTest, PushWithoutOpenUartDropsFrameAndReturnsFalse) {
  MavRadioController controller;
  const std::vector<uint8_t> frame{1, 2, 3};
  for (int i = 0; i < 100; ++i) {  // wiecej niz pojemnosc kolejki (20) - nie moze zablokowac
    EXPECT_FALSE(controller.Push(frame));
  }
}

}  // namespace radio
}  // namespace apps
}  // namespace srp
