/**
 * @file radio_app_test.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <gtest/gtest.h>
#include "apps/fc/radio_service/radio_app.h"
#include "core/uart/mock_uart_driver.hpp"
#include "core/timestamp/mock_timestamp_driver.h"

using ::testing::_;
using ::testing::Invoke;

TEST(RADIOAPPTEST, InitializeTestNoUart) {
    ara::log::LoggingMenager::Create("123", ara::log::LogMode::kConsole);
    const std::map<ara::core::StringView, ara::core::StringView> map;
    srp::apps::RadioApp app;
    EXPECT_EQ(app.Initialize(map), 1);
}

// TEST(RADIOAPPTEST, InitializeTestMock) {
//     auto mock_uart = std::make_unique<MockUartDriver>();
//     EXPECT_CALL(*mock_uart, Open(::testing::_, ::testing::_))
//         .Times(1)
//         .WillOnce(::testing::Return(true));
//     const std::map<ara::core::StringView, ara::core::StringView> map;
//     srp::apps::RadioApp app;
//     app.InitUart(std::move(mock_uart));
//     EXPECT_EQ(app.Initialize(map), 0);
// }

class TestWrapper : public srp::apps::RadioApp {
 public:
            TestWrapper(): RadioApp() {
               this->event_data = srp::apps::EventData::GetInstance();
            }
          void TestInitUart(std::unique_ptr<srp::core::uart::IUartDriver> uart) {
              InitUart(std::move(uart));
          }
          void TestInitTimestamp(std::unique_ptr<srp::core::timestamp::ITimestampController> timestamp) {
              InitTimestamp(std::move(timestamp));
          }
          void TestTransmittingLoop(const std::stop_token& token) {
              this->TransmittingLoop(token);
          }
          std::shared_ptr<srp::apps::EventData> GetEventDataPtr() {
              return this->event_data;
          }
};

struct EventDataTestParams {
    uint16_t input_temp1;
    uint16_t input_temp2;
    uint16_t input_temp3;
    float input_pressure;
    int32_t input_gpslat;
    int32_t input_gpslon;
    std::optional<int64_t> input_timestamp;
    uint8_t input_primer;
    uint8_t input_servo;
    uint8_t input_servovent;
};

class EventDataTest : public ::testing::TestWithParam<EventDataTestParams> {};

// INSTANTIATE_TEST_SUITE_P - Tests that radio app correctly sends messages
INSTANTIATE_TEST_SUITE_P(EventDataTestParameters, EventDataTest,
    ::testing::Values(
        EventDataTestParams{
            0, 0, 0, 0.0f, 0, 0,
            std::optional<int64_t>{0},
            0, 0, 0
        },
        EventDataTestParams{
            0xFFFF, 0xFFFF, 0xFFFF, FLT_MAX, 0x7FFFFFFF, 0x7FFFFFFF,
            std::optional<int64_t>{INT64_MAX},
            1, 1, 1
        }
    )
);

TEST_P(EventDataTest, ValidateEventData) {
    auto params = GetParam();
    TestWrapper wrapper_;
    auto mock_uart = std::make_unique<MockUartDriver>();
    std::vector<std::vector<uint8_t>> sent_data;
    EXPECT_CALL(*mock_uart, Write(_))
    .Times(5)
    .WillRepeatedly(Invoke([&sent_data](const std::vector<uint8_t>& data) -> srp::core::ErrorCode {
        sent_data.push_back(data);
        return srp::core::ErrorCode::kOk;
    }));
    auto mock_timestamp = std::make_unique<MockTimestampController>();
    EXPECT_CALL(*mock_timestamp, GetNewTimeStamp())
        .Times(1)
        .WillOnce(::testing::Return(params.input_timestamp));
    EXPECT_CALL(*mock_timestamp, Init())
        .Times(1);
    wrapper_.TestInitUart(std::move(mock_uart));
    wrapper_.TestInitTimestamp(std::move(mock_timestamp));
    auto event_data = srp::apps::EventData::GetInstance();

    // Reset all values to ensure clean state
    event_data->SetTemp1(0);
    event_data->SetTemp2(0);
    event_data->SetTemp3(0);
    event_data->SetPress(0);
    event_data->SetGPS(0, 0, 0);
    event_data->SetActuatorState(0, 0);
    event_data->SetActuatorState(1, 0);
    event_data->SetActuatorState(2, 0);

    // Set test values
    event_data->SetTemp1(static_cast<int16_t>(params.input_temp1));
    event_data->SetTemp2(static_cast<int16_t>(params.input_temp2));
    event_data->SetTemp3(static_cast<int16_t>(params.input_temp3));
    event_data->SetPress(static_cast<uint16_t>(params.input_pressure));
    event_data->SetGPS(params.input_gpslon, params.input_gpslat, 0);
    event_data->SetActuatorState(0, params.input_primer);
    event_data->SetActuatorState(1, params.input_servo);
    event_data->SetActuatorState(2, params.input_servovent);

    std::jthread transmitting_thread([&wrapper_](const std::stop_token& token) {
        wrapper_.TestTransmittingLoop(token);
    });
    ASSERT_EQ(event_data.get(), wrapper_.GetEventDataPtr().get());
    std::this_thread::sleep_for(std::chrono::milliseconds(900));
    transmitting_thread.request_stop();
    transmitting_thread.join();
    EXPECT_EQ(sent_data.size(), 5);

    // Verify message structure: {STX, payloadLen, seq, systemId, compId, msgId, ... payload ..., CRC1, CRC2}
    // Message order: heartbeat, max_altitude, tank_temperature, tank_pressure, gps
    const uint8_t kFrameStart = 254;
    const uint8_t kSystemId = 1;
    const uint8_t kComponentId = 200;

    // Check heartbeat (msgId=73)
    EXPECT_EQ(sent_data[0][0], kFrameStart);
    EXPECT_EQ(sent_data[0][3], kSystemId);
    EXPECT_EQ(sent_data[0][4], kComponentId);
    EXPECT_EQ(sent_data[0][5], 73);  // SIMBA_ROCKET_HEARTBEAT

    // Check max_altitude (msgId=75)
    EXPECT_EQ(sent_data[1][0], kFrameStart);
    EXPECT_EQ(sent_data[1][3], kSystemId);
    EXPECT_EQ(sent_data[1][4], kComponentId);
    EXPECT_EQ(sent_data[1][5], 75);  // SIMBA_MAX_ALTITUDE

    // Check tank_temperature (msgId=69)
    EXPECT_EQ(sent_data[2][0], kFrameStart);
    EXPECT_EQ(sent_data[2][3], kSystemId);
    EXPECT_EQ(sent_data[2][4], kComponentId);
    EXPECT_EQ(sent_data[2][5], 69);  // SIMBA_TANK_TEMPERATURE

    // Check tank_pressure (msgId=70)
    EXPECT_EQ(sent_data[3][0], kFrameStart);
    EXPECT_EQ(sent_data[3][3], kSystemId);
    EXPECT_EQ(sent_data[3][4], kComponentId);
    EXPECT_EQ(sent_data[3][5], 70);  // SIMBA_TANK_PRESSURE

    // Check GPS (msgId=72)
    EXPECT_EQ(sent_data[4][0], kFrameStart);
    EXPECT_EQ(sent_data[4][3], kSystemId);
    EXPECT_EQ(sent_data[4][4], kComponentId);
    EXPECT_EQ(sent_data[4][5], 72);  // SIMBA_GPS
}
