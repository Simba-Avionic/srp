/**
 * @file event_data_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "gtest/gtest.h"
#include "apps/fc/radio_service/event_data.h"
namespace srp::apps {
// --- Temperature Test ---
class TemperatureTest : public ::testing::TestWithParam<std::tuple<int, int16_t>> {
 protected:
    std::shared_ptr<EventData> data;
    void SetUp() override { data = EventData::GetInstance(); }
};

TEST_P(TemperatureTest, SetAndGetTemperature) {
    int temp_number = std::get<0>(GetParam());
    int16_t value = std::get<1>(GetParam());

    switch (temp_number) {
    case 1:
        data->SetTemp1(value);
        EXPECT_EQ(data->GetTemp1(), value);
        break;
    case 2:
        data->SetTemp2(value);
        EXPECT_EQ(data->GetTemp2(), value);
        break;
    case 3:
        data->SetTemp3(value);
        EXPECT_EQ(data->GetTemp3(), value);
        break;
    default:
        FAIL() << "Invalid temperature sensor number";
    }
}

INSTANTIATE_TEST_SUITE_P(
    TemperatureTests,
    TemperatureTest,
    ::testing::Values(
        std::make_tuple(1, 0),              // minimal
        std::make_tuple(2, INT16_MAX),      // maksymalna wartość int16_t
        std::make_tuple(3, 100),            // środek zakresu
        std::make_tuple(1, 42),             // losowa wartość
        std::make_tuple(2, 1000)            // wartość dodatnia
    )
);

// --- Pressure Test ---
class PressureTest : public ::testing::TestWithParam<std::tuple<std::string, float>> {
 protected:
    std::shared_ptr<EventData> data;
    void SetUp() override { data = EventData::GetInstance(); }
};

TEST_P(PressureTest, SetAndGetPressure) {
    auto [field, value] = GetParam();

    if (field == "Press") {
        uint16_t press_int = static_cast<uint16_t>(value);
        data->SetPress(press_int);
        // GetPress returns float, but SetPress accepts uint16_t, so compare as uint16_t
        EXPECT_EQ(static_cast<uint16_t>(data->GetPress()), press_int);
    } else {
        FAIL() << "Invalid pressure field";
    }
}

INSTANTIATE_TEST_SUITE_P(
    PressureTests,
    PressureTest,
    ::testing::Values(
        std::make_tuple("Press", 0.0f),         // min
        std::make_tuple("Press", 65535.0f),     // max uint16_t
        std::make_tuple("Press", 1000.0f),      // typowa wartość
        std::make_tuple("Press", 123.456f),     // losowa
        std::make_tuple("Press", 789.123f)      // losowa
    )
);

// --- GPS Test ---
class GPSTest : public ::testing::TestWithParam<std::tuple<int32_t, int32_t>> {
 protected:
    std::shared_ptr<EventData> data;
    void SetUp() override { data = EventData::GetInstance(); }
};

TEST_P(GPSTest, SetAndGetGPS) {
    auto [lon, lat] = GetParam();
    data->SetGPS(lon, lat, 0);
    EXPECT_EQ(data->GetGPSLon(), lon);
    EXPECT_EQ(data->GetGPSLat(), lat);
}

INSTANTIATE_TEST_SUITE_P(
    GPSTests,
    GPSTest,
    ::testing::Values(
        std::make_tuple(INT32_MIN, INT32_MIN),    // minimalne
        std::make_tuple(INT32_MAX, INT32_MAX),    // maksymalne
        std::make_tuple(0, 0),                    // zerowe
        std::make_tuple(50000000, 25000000),      // typowe współrzędne
        std::make_tuple(-100000000, 75000000)     // negatywne wartości
    )
);

// --- Actuator State Test ---
class ActuatorStateTest : public ::testing::TestWithParam<std::tuple<uint8_t, uint8_t>> {
 protected:
    std::shared_ptr<EventData> data;
    void SetUp() override { data = EventData::GetInstance(); }
};

TEST_P(ActuatorStateTest, SetAndGetActuatorState) {
    auto [actuator_id, state] = GetParam();
    data->SetActuatorState(actuator_id, state);
    uint8_t actuator_states = data->GetActuatorStates();
    EXPECT_EQ(((actuator_states >> actuator_id) & 1), state);
}

INSTANTIATE_TEST_SUITE_P(
    ActuatorStateTests,
    ActuatorStateTest,
    ::testing::Values(
        std::make_tuple(0, 1),    // pierwszy aktuator włączony
        std::make_tuple(1, 1),    // drugi aktuator włączony
        std::make_tuple(2, 1),    // trzeci aktuator włączony
        std::make_tuple(0, 0),    // pierwszy aktuator wyłączony
        std::make_tuple(1, 0),    // drugi aktuator wyłączony
        std::make_tuple(2, 0)     // trzeci aktuator wyłączony
    )
);

TEST(EventDataSingletonTest, SameInstanceReturned) {
    auto instance1 = EventData::GetInstance();
    auto instance2 = EventData::GetInstance();
    EXPECT_EQ(instance1.get(), instance2.get());
    instance1->SetTemp1(1);
    instance1->SetTemp2(2);
    instance1->SetTemp3(3);
    EXPECT_EQ(instance1->GetTemp1(), instance2->GetTemp1());
    EXPECT_EQ(instance1->GetTemp2(), instance2->GetTemp2());
    EXPECT_EQ(instance1->GetTemp3(), instance2->GetTemp3());
}
}  // namespace srp::apps
