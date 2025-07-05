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
    float input_Dpressure;
    float input_pressure;
    int32_t input_gpslat;
    int32_t input_gpslon;
    std::optional<int64_t> input_timestamp;
    uint8_t input_primer;
    uint8_t input_servo;
    uint8_t input_servovent;
    std::vector<uint8_t> expected_temp;
    std::vector<uint8_t> expected_pressure;
    std::vector<uint8_t> expected_gps;
    std::vector<uint8_t> expected_heartbeat;
    std::vector<uint8_t> expected_actuator;
};

class EventDataTest : public ::testing::TestWithParam<EventDataTestParams> {};

// EXPECTED SHOULD LOOK LIKE: {STX, payloadLen, seq, systemId, compId, msgId, valuesInUint8_t, CRC_calculated}
INSTANTIATE_TEST_SUITE_P(EventDataTestParameters, EventDataTest,
    ::testing::Values(
        EventDataTestParams{
            0, 0, 0, 0, 0, 0, 0,
            std::optional<int64_t>{0},
            0, 0, 0,
            {254, 6, 0, 1, 200, 69, 0, 0, 0, 0, 0, 0, 170, 105},
            {254, 8, 1, 1, 200, 70, 0, 0, 0, 0, 0, 0, 0, 0, 178, 119},
            {254, 12, 2, 1, 200, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 93},
            {254, 10, 3, 1, 200, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 182, 219},
            {254, 1, 4, 1, 200, 68, 0, 142, 237}
        },

        EventDataTestParams {
            0xFFFF, 0xFFFF, 0xFFFF, FLT_MAX, FLT_MAX, 0x7FFFFFFF, 0x7FFFFFFF,
            std::optional<int64_t>{INT64_MAX},
            1, 1, 1,
            {254, 6, 5, 1, 200, 69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 90, 24},
            {254, 8, 6, 1, 200, 70, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 66, 88},
            {254, 12, 7, 1, 200, 72, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0, 0, 0, 0, 253, 87},
            {254, 10, 8, 1, 200, 73, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0, 0, 224, 130},
            {254, 1, 9, 1, 200, 68, 7, 89, 149}
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

    event_data->SetTemp1(params.input_temp1);
    event_data->SetTemp2(params.input_temp2);
    event_data->SetTemp3(params.input_temp3);
    event_data->SetDPress(params.input_Dpressure);
    event_data->SetPress(params.input_pressure);
    event_data->SetGPS(params.input_gpslon, params.input_gpslat);
    event_data->SetActuatorBit(params.input_primer, 0);
    event_data->SetActuatorBit(params.input_servo, 1);
    event_data->SetActuatorBit(params.input_servovent, 2);

    std::jthread transmitting_thread([&wrapper_](const std::stop_token& token) {
        wrapper_.TestTransmittingLoop(token);
    });
    ASSERT_EQ(event_data.get(), wrapper_.GetEventDataPtr().get());
    std::this_thread::sleep_for(std::chrono::milliseconds(900));
    transmitting_thread.request_stop();
    transmitting_thread.join();
    EXPECT_EQ(sent_data.size(), 5);
    EXPECT_EQ(sent_data[0], params.expected_temp);
    EXPECT_EQ(sent_data[1], params.expected_pressure);
    EXPECT_EQ(sent_data[2], params.expected_gps);
    EXPECT_EQ(sent_data[3], params.expected_heartbeat);
    EXPECT_EQ(sent_data[4], params.expected_actuator);
}
