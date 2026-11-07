/**
 * @file refuel_controller_test.cc
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-10-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>  // NOLINT
#include <chrono>  // NOLINT
#include "apps/ec/fuel_service/controller/refuel_controller.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::AnyNumber;
using ::testing::Sequence;

namespace srp::apps {

class MockValves {
 public:
    MOCK_METHOD(void, SetMainValve, (const uint8_t pos));
    MOCK_METHOD(void, SetVentValve, (const uint8_t pos));
    MOCK_METHOD(void, SetDumpValve, (const uint8_t pos));
};

class RefuelControllerTest : public ::testing::Test {
 protected:
    RefuelController controller;
    Config config;
    TimeoutConfig timeouts;
    std::shared_ptr<MockValves> valves;

    void SetUp() override {
        valves = std::make_shared<MockValves>();

        config.tank_name = "UT_TANK";
        config.min_gs_pressure = 4000;
        config.max_gs_pressure = 6000;
        config.test_pressure = 400;
        config.test_max_drop_percent = 10;
        config.max_safe_pressure = 6500;
        config.max_tanking_time_s = 1;

        timeouts.flow_check_interval_ms = 5;
        timeouts.pressure_stabilization_time_ms = 3;
        timeouts.pressure_test_time_s = 1;
        timeouts.valve_delay_ms = 1;

        EXPECT_CALL(*valves, SetMainValve(_)).Times(AnyNumber());
        EXPECT_CALL(*valves, SetVentValve(_)).Times(AnyNumber());
        EXPECT_CALL(*valves, SetDumpValve(_)).Times(AnyNumber());

        bool init_ok = controller.Initialize(
            config,
            [this](uint8_t pos) { valves->SetMainValve(pos); },
            [this](uint8_t pos) { valves->SetVentValve(pos); },
            [this](uint8_t pos) { valves->SetDumpValve(pos); });
        ASSERT_TRUE(init_ok);

        controller.timeouts = timeouts;
    }

    void WaitForState(RefuelingState_t expected_state, int timeout_ms = 2000) {
        int waited = 0;
        while (controller.GetState() != expected_state && waited < timeout_ms) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            waited += 2;
        }
        ASSERT_EQ(controller.GetState(), expected_state) << "Timeout waiting for state transition to "
            << to_string(expected_state);
    }
};


TEST_F(RefuelControllerTest, InitializeFailsWithMissingMainOrDumpCallbacks) {
    RefuelController local_controller;

    EXPECT_FALSE(local_controller.Initialize(config, nullptr, nullptr, nullptr));

    EXPECT_FALSE(local_controller.Initialize(
        config,
        [](uint8_t pos){},
        nullptr,
        nullptr));

    EXPECT_FALSE(local_controller.Initialize(
        config,
        nullptr,
        nullptr,
        [](uint8_t pos){}));
}

TEST_F(RefuelControllerTest, InitializeSucceedsWithoutVentValve) {
    RefuelController local_controller;
    EXPECT_TRUE(local_controller.Initialize(
        config,
        [](uint8_t pos){},
        nullptr,
        [](uint8_t pos){}));
}

TEST_F(RefuelControllerTest, StartAbortsIfGSPressureTooLow) {
    EXPECT_CALL(*valves, SetMainValve(0)).Times(AtLeast(1));
    EXPECT_CALL(*valves, SetDumpValve(1)).Times(AtLeast(1));

    controller.OnGSPressureReceived(3000);

    controller.StartRefueling(RefuelingType_t::MASS, 1000);
    WaitForState(RefuelingState_t::ABORT, 100);
}

TEST_F(RefuelControllerTest, StartAbortsIfGSPressureTooHigh) {
    EXPECT_CALL(*valves, SetMainValve(0)).Times(AtLeast(1));
    EXPECT_CALL(*valves, SetDumpValve(1)).Times(AtLeast(1));

    controller.OnGSPressureReceived(7000);

    controller.StartRefueling(RefuelingType_t::MASS, 1000);
    WaitForState(RefuelingState_t::ABORT, 100);
}

TEST_F(RefuelControllerTest, PressureTestTimeoutAborts) {
    controller.OnGSPressureReceived(5000);
    controller.StartRefueling(RefuelingType_t::MASS, 1000);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    WaitForState(RefuelingState_t::ABORT, 1500);
}

TEST_F(RefuelControllerTest, DetectsLeakDuringPressureTest) {
    controller.OnGSPressureReceived(5000);
    controller.StartRefueling(RefuelingType_t::MASS, 1000);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    controller.OnTankPressureReceived(400);
    std::this_thread::sleep_for(std::chrono::milliseconds(15));

    controller.OnTankPressureReceived(300);

    WaitForState(RefuelingState_t::ABORT, 100);
}

TEST_F(RefuelControllerTest, ExternalAbortTriggersAbortState) {
    controller.OnGSPressureReceived(5000);
    controller.StartRefueling(RefuelingType_t::MASS, 1000);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    EXPECT_CALL(*valves, SetMainValve(0)).Times(AtLeast(1));
    EXPECT_CALL(*valves, SetVentValve(0)).Times(AtLeast(1));
    EXPECT_CALL(*valves, SetDumpValve(1)).Times(AtLeast(1));

    controller.RequestAbort();
    WaitForState(RefuelingState_t::ABORT, 100);
}

TEST_F(RefuelControllerTest, NominalPressureRefuelingSuccess) {
    controller.OnGSPressureReceived(5000);
    controller.StartRefueling(RefuelingType_t::PRESSURE, 5500);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    controller.OnTankPressureReceived(400);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    WaitForState(RefuelingState_t::REFUELING);

    controller.OnTankPressureReceived(4000);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    controller.OnTankPressureReceived(5000);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    controller.OnTankPressureReceived(5500);

    WaitForState(RefuelingState_t::TANKING_COMPLETE);
}

TEST_F(RefuelControllerTest, NominalMassRefuelingSuccess) {
    controller.OnGSPressureReceived(5000);
    controller.OnRocketMassReceived(7000);
    controller.StartRefueling(RefuelingType_t::MASS, 1000);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    controller.OnTankPressureReceived(400);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    WaitForState(RefuelingState_t::REFUELING);

    controller.OnRocketMassReceived(7000);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    controller.OnRocketMassReceived(7500);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    controller.OnRocketMassReceived(8060);

    WaitForState(RefuelingState_t::TANKING_COMPLETE);
}

TEST_F(RefuelControllerTest, MassFlowStoppedPulsesVent) {
    controller.OnGSPressureReceived(5000);
    controller.StartRefueling(RefuelingType_t::MASS, 1000);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    for (int i = 0; i < 20; ++i) {
        controller.OnTankPressureReceived(450);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    WaitForState(RefuelingState_t::REFUELING);

    EXPECT_CALL(*valves, SetVentValve(1U)).Times(AtLeast(1));
    EXPECT_CALL(*valves, SetVentValve(0U)).Times(AtLeast(1));

    controller.OnTankPressureReceived(5000);

    for (int i = 0; i < 30; ++i) {
        controller.OnRocketMassReceived(500);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

TEST_F(RefuelControllerTest, PressureFlowStoppedNotPulsesVent) {
    controller.OnGSPressureReceived(5000);
    controller.StartRefueling(RefuelingType_t::PRESSURE, 1000);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    for (int i = 0; i < 20; ++i) {
        controller.OnTankPressureReceived(450);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    WaitForState(RefuelingState_t::REFUELING);

    EXPECT_CALL(*valves, SetVentValve(1U)).Times(0);
    EXPECT_CALL(*valves, SetVentValve(0U)).Times(0);

    controller.OnTankPressureReceived(5000);

    for (int i = 0; i < 30; ++i) {
        controller.OnRocketMassReceived(500);
        controller.OnTankPressureReceived(5000);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

TEST_F(RefuelControllerTest, OverpressureAfterCompleteTriggersVent) {
    controller.OnGSPressureReceived(5000);
    controller.OnRocketMassReceived(7000);
    controller.StartRefueling(RefuelingType_t::MASS, 1000);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    controller.OnTankPressureReceived(400);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    WaitForState(RefuelingState_t::REFUELING);

    controller.OnRocketMassReceived(8060);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    controller.OnRocketMassReceived(8060);
    WaitForState(RefuelingState_t::TANKING_COMPLETE);

    EXPECT_CALL(*valves, SetVentValve(1U)).Times(AtLeast(1));
    EXPECT_CALL(*valves, SetVentValve(0U)).Times(AtLeast(1));

    controller.OnTankPressureReceived(6600);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    controller.OnGSPressureReceived(6000);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_F(RefuelControllerTest, OverpressureWithoutVentValveDoesNotCrash) {
    RefuelController local_controller;
    local_controller.Initialize(
        config,
        [this](uint8_t pos){ valves->SetMainValve(pos); },
        nullptr,
        [this](uint8_t pos){ valves->SetDumpValve(pos); });
    local_controller.timeouts = timeouts;

    auto WaitForLocalState = [&](RefuelingState_t expected) {
        int waited = 0;
        while (local_controller.GetState() != expected && waited < 2000) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            waited += 2;
        }
        ASSERT_EQ(local_controller.GetState(), expected);
    };

    local_controller.OnGSPressureReceived(5000);
    local_controller.StartRefueling(RefuelingType_t::PRESSURE, 5000);

    local_controller.OnTankPressureReceived(400);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    WaitForLocalState(RefuelingState_t::REFUELING);

    local_controller.OnTankPressureReceived(5000);
    WaitForLocalState(RefuelingState_t::TANKING_COMPLETE);

    EXPECT_CALL(*valves, SetVentValve(_)).Times(0);

    local_controller.OnTankPressureReceived(7000);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_F(RefuelControllerTest, RefuelingTimeoutAborts) {
    controller.OnGSPressureReceived(5000);
    controller.StartRefueling(RefuelingType_t::MASS, 1000);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    uint16_t test_press = 450;
    for (int i = 0; i < 20; ++i) {
        controller.OnTankPressureReceived(test_press);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    WaitForState(RefuelingState_t::REFUELING);

    EXPECT_CALL(*valves, SetMainValve(0)).Times(AtLeast(1));
    EXPECT_CALL(*valves, SetDumpValve(1)).Times(AtLeast(1));

    auto start_time = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start_time < std::chrono::milliseconds(1500)) {
        controller.OnRocketMassReceived(500);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    WaitForState(RefuelingState_t::ABORT);
}

TEST_F(RefuelControllerTest, FullAppTestWithFluctuations) {
    controller.OnGSPressureReceived(5000);
    controller.StartRefueling(RefuelingType_t::MASS, 10000);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    uint16_t test_press = 0;

    while (test_press < 500) {
        test_press += 50;

        int r_val = random() % 20;
        int op = random() % 2;
        if (op == 0) {
            test_press -= r_val;
        } else {
            test_press += r_val;
        }

        controller.OnTankPressureReceived(test_press);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));


    for (int i = 0; i < 50; ++i) {
        uint16_t temp_press = test_press;

        int r_val = random() % 50;

        if (i % 2 == 0) {
            temp_press -= r_val;
        } else {
            temp_press += r_val;
        }

        controller.OnTankPressureReceived(temp_press);

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    WaitForState(RefuelingState_t::REFUELING);

    int16_t current_mass = 0;
    while (current_mass < 10600) {
        current_mass += 500;

        int r_val = random() % 50;
        int op = random() % 2;
        int16_t noise_mass = (op == 0) ? (current_mass - r_val) : (current_mass + r_val);

        controller.OnRocketMassReceived(noise_mass);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    WaitForState(RefuelingState_t::TANKING_COMPLETE);

    EXPECT_CALL(*valves, SetVentValve(1U)).Times(AtLeast(1));
    EXPECT_CALL(*valves, SetVentValve(0U)).Times(AtLeast(1));

    uint16_t over_press = 6700;
    for (int i = 0; i < 20; ++i) {
        int r_val = random() % 30;
        uint16_t temp_press = (i % 2 == 0) ? (over_press - r_val) : (over_press + r_val);

        controller.OnTankPressureReceived(temp_press);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    uint16_t safe_press = 6000;
    for (int i = 0; i < 20; ++i) {
        int r_val = random() % 20;
        uint16_t temp_press = (i % 2 == 0) ? (safe_press - r_val) : (safe_press + r_val);

        controller.OnTankPressureReceived(temp_press);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    WaitForState(RefuelingState_t::TANKING_COMPLETE);
}

TEST_F(RefuelControllerTest, StartRefuelingBlockedAfterAbort) {
    controller.OnGSPressureReceived(5000);
    controller.StartRefueling(RefuelingType_t::PRESSURE, 5000);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    controller.RequestAbort();
    WaitForState(RefuelingState_t::ABORT);

    controller.StartRefueling(RefuelingType_t::PRESSURE, 5500);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(controller.GetState(), RefuelingState_t::ABORT);
}

TEST_F(RefuelControllerTest, MultipleStartRefuelingCallsNotResetRefuelingState) {
    controller.OnGSPressureReceived(5000);
    controller.StartRefueling(RefuelingType_t::PRESSURE, 5000);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    controller.OnTankPressureReceived(400);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    WaitForState(RefuelingState_t::REFUELING);

    controller.StartRefueling(RefuelingType_t::PRESSURE, 5000);
    EXPECT_EQ(controller.GetState(), RefuelingState_t::REFUELING);
}

TEST_F(RefuelControllerTest, RapidSamplesDoNotCauseDivisionByZero) {
    controller.OnGSPressureReceived(5000);
    controller.StartRefueling(RefuelingType_t::PRESSURE, 5500);
    WaitForState(RefuelingState_t::PRESSURE_TEST);

    for (int i = 0; i < 15; ++i) {
        controller.OnTankPressureReceived(400 + i);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    WaitForState(RefuelingState_t::REFUELING);

    for (int i = 0; i < 15; ++i) {
        controller.OnTankPressureReceived(5000 + i);
    }

    controller.OnTankPressureReceived(5500);
    WaitForState(RefuelingState_t::TANKING_COMPLETE);
}

}  // namespace srp::apps
