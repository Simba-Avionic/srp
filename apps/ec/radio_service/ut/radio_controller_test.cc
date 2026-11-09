/**
 * @file radio_controller_test.cc
 * @brief UT MavRadioController na mocku IUartDriver: wstrzykniety driver, preset MAVLink przy Open(),
 *        odbior ramki heartbeat przez WaitForData()/Read() i wywolanie callbacku.
 *
 * @copyright Copyright (c) 2025
 */
#include <gtest/gtest.h>

#include <chrono>  // NOLINT
#include <future>  // NOLINT
#include <memory>
#include <stop_token>  // NOLINT
#include <thread>  // NOLINT
#include <utility>
#include <vector>

#include "apps/ec/radio_service/radio_controller.hpp"
#include "core/uart/mock_uart_driver.hpp"

using ::testing::_;
using ::testing::An;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::Truly;
using srp::core::uart::UartConfig;

namespace {
// heartbeat MAVLink v1: sysid 255, compid 190 (GCS)
const std::vector<uint8_t> kHeartbeat{0xfe, 0x09, 0x00, 0xff, 0xbe, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x06, 0x08, 0x00, 0x04, 0x03, 0x49, 0x21};
}  // namespace

TEST(RadioControllerTest, OpensWithMavlinkPresetAndDeliversReceivedFrame) {
  auto mock = std::make_unique<NiceMock<MockUartDriver>>();
  auto* uart = mock.get();
  EXPECT_CALL(*uart, Open(StrEq("/dev/ttyS1"), An<const UartConfig&>()))
      .WillOnce(Invoke([](const std::string&, const UartConfig& c) {
        EXPECT_TRUE(c.raw);                                   // MAVLink = tryb raw
        EXPECT_EQ(c.baudrate, static_cast<speed_t>(B57600));
        EXPECT_EQ(c.vmin, 0);
        EXPECT_EQ(c.vtime, 10);
        return true;
      }));
  // pierwszy WaitForData: dane sa, kolejne: brak (z krotkim snem, zeby nie kreciec petli)
  bool delivered = false;
  ON_CALL(*uart, WaitForData(_)).WillByDefault(Invoke([&delivered](int) {
    if (!delivered) {
      delivered = true;
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return false;
  }));
  ON_CALL(*uart, Read(_)).WillByDefault(Return(std::optional<std::vector<uint8_t>>{kHeartbeat}));

  std::promise<uint32_t> got;
  auto fut = got.get_future();
  std::stop_source stop;
  {
    srp::apps::radio::MavRadioController controller(std::move(mock));
    controller.Init([&got](const mavlink_message_t& msg) {
      try {
        got.set_value(msg.msgid);
      } catch (const std::future_error&) {
        // kolejne ramki - pierwsza wystarczy
      }
    }, stop.get_token());
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(3)), std::future_status::ready);
    EXPECT_EQ(fut.get(), 0u);  // MAVLINK_MSG_ID_HEARTBEAT
  }
}

TEST(RadioControllerTest, TransmitsQueuedPackageThroughDriver) {
  auto mock = std::make_unique<NiceMock<MockUartDriver>>();
  auto* uart = mock.get();
  ON_CALL(*uart, Open(_, An<const UartConfig&>())).WillByDefault(Return(true));
  ON_CALL(*uart, WaitForData(_)).WillByDefault(Invoke([](int) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return false;
  }));
  std::promise<std::vector<uint8_t>> written;
  auto fut = written.get_future();
  EXPECT_CALL(*uart, Write(_)).WillOnce(Invoke([&written](const std::vector<uint8_t>& d) {
    written.set_value(d);
    return srp::core::ErrorCode::kOk;
  }));
  std::stop_source stop;
  {
    srp::apps::radio::MavRadioController controller(std::move(mock));
    controller.Init([](const mavlink_message_t&) {}, stop.get_token());
    controller.Push(kHeartbeat);
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(3)), std::future_status::ready);
    EXPECT_EQ(fut.get(), kHeartbeat);
  }
}
