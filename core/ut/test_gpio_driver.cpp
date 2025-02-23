/**
 * @file test_gpio_driver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include "core/gpio/gpio_driver.hpp"
#include "core/file/file.hpp"
#include "core/file/mock_file.hpp"
using ::testing::Return;
using ::testing::_;
class GpioTest : public ::testing::TestWithParam<std::tuple<int, std::string, std::string>> {};

TEST_P(GpioTest, TestGetPath) {
    int pin = std::get<0>(GetParam());
    std::string suffix = std::get<1>(GetParam());
    auto val = srp::core::gpio::GpioDriver::getEndpointPath(pin, suffix);
    std::string expectedPath = "/sys/class/gpio/gpio" + std::to_string(pin) + "/" + suffix;
    EXPECT_EQ(val, expectedPath);
}

INSTANTIATE_TEST_SUITE_P(
    GpioTestInstances,
    GpioTest,
    ::testing::Values(
        std::make_tuple(12, "hello", "/sys/class/gpio/gpio12/hello"),
        std::make_tuple(13, "world", "/sys/class/gpio/gpio13/world"),
        std::make_tuple(14, "test", "/sys/class/gpio/gpio14/test"),
        std::make_tuple(0, "", "/sys/class/gpio/gpio0/"),
        std::make_tuple(255, "world", "/sys/class/gpio/gpio255/world"),
        std::make_tuple(65535, "test", "/sys/class/gpio/gpio65535/test")
    )
);

TEST(FileHandlerTest, OpenFileTest) {
    // Tworzymy mocka
    MockFileHandler mock_file;

    // Określamy, jak ma się zachować mock przy wywołaniu metody 'open'
    EXPECT_CALL(mock_file, open("/some/path", srp::core::File_mode_t::WRITE))
        .WillOnce(::testing::Return(true));  // Zwracamy 'true' przy otwarciu pliku

    // Wywołanie testowanej metody
    bool result = mock_file.open("/some/path", srp::core::File_mode_t::WRITE);

    // Sprawdzamy wynik
    EXPECT_TRUE(result);
}

TEST(FileHandlerTest, WriteDataTest) {
    // Tworzymy mocka
    MockFileHandler mock_file;

    // Określamy, jak ma się zachować mock przy wywołaniu metody 'write'
    EXPECT_CALL(mock_file, write("some data"))
        .WillOnce(::testing::Return(true));  // Zwracamy 'true' przy zapisaniu danych

    // Wywołanie testowanej metody
    bool result = mock_file.write("some data");

    // Sprawdzamy wynik
    EXPECT_TRUE(result);
}
