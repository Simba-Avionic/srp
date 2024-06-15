/**
 * @file test_adcsensor.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Unit tests for adcsensor
 * @version 0.1
 * @date 2024-06-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "mw/i2c_service/controller/mock/mock_ads7828.h"
#include "mw/i2c_service/controller/adcsensor/controller.hpp"

class TestWrapper : public simba::i2c::ADCSensorController {
 public:
  void TestInit(const std::unordered_map<std::string, std::string>& parms, std::unique_ptr<simba::i2c::IADS7828> adc_) {
    this->Init(parms, std::move(adc_));
  }
  float TestCalculateB(float R, float A, float A_MIN) {
    return this->CalculateB(R, A, A_MIN);
  }
  float TestCalculateA(float R, float RES_MAX, float RES_MIN, float A_MAX, float A_MIN) {
    return this->CalculateA(R, RES_MAX, RES_MIN, A_MAX, A_MIN);
  }
};

TEST(TestADCSensor, CalculateTest) {
    TestWrapper wrapper{};
    constexpr float R = 160;
    constexpr float RES_MAX = 100;
    constexpr float RES_MIN = 0;
    constexpr float A_MAX = 0.02;
    constexpr float A_MIN = 0.004;
    float A = wrapper.TestCalculateA(R, RES_MAX, RES_MIN, A_MAX, A_MIN);
    EXPECT_EQ(wrapper.TestCalculateB(R, A, A_MIN) +  A * A_MAX, RES_MAX);
}