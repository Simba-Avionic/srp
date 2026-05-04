#include <gtest/gtest.h>
#include <fstream>
#include <string>

#include "mw/timestamp_mw/ntp/config/config_manager.hpp"

using namespace srp::tinyNTP;

class ConfigManagerTest : public ::testing::Test {
 protected:
    const std::string temp_filepath = "test_config.json";

    void CreateJsonFile(const std::string& content) {
        std::ofstream file(temp_filepath);
        file << content;
        file.close();
    }

    void TearDown() override {
        std::remove(temp_filepath.c_str()); // Usuwamy plik testowy
    }
};

TEST_F(ConfigManagerTest, LoadsValidConfigCorrectly) {
    CreateJsonFile(R"({
        "ip": "192.168.1.100",
        "ntp_class": 3,
        "T_hb_ms": 500
    })");

    auto config = ConfigManager::LoadConfig(temp_filepath);

    EXPECT_EQ(config.ip, "192.168.1.100");
    EXPECT_EQ(config.ntp_class, 3);
    EXPECT_EQ(config.t_hb_ms, 500);
}

TEST_F(ConfigManagerTest, UsesFallbackWhenFileDoesNotExist) {
    auto config = ConfigManager::LoadConfig("fake_path_that_doesnt_exist.json");

    EXPECT_EQ(config.ip, "127.0.0.1");
    EXPECT_EQ(config.ntp_class, 7);
    EXPECT_EQ(config.t_hb_ms, 1000);
}

TEST_F(ConfigManagerTest, UsesFallbackOnInvalidJsonSyntax) {
    CreateJsonFile(R"({
        "ip": "10.0.0.1",
        "ntp_class": 2
        "T_hb_ms": 500
    )");

    auto config = ConfigManager::LoadConfig(temp_filepath);

    EXPECT_EQ(config.ip, "127.0.0.1");
    EXPECT_EQ(config.ntp_class, 7);
    EXPECT_EQ(config.t_hb_ms, 1000);
}

TEST_F(ConfigManagerTest, ForcesFallbackClassIfOutOfBounds) {
    CreateJsonFile(R"({
        "ip": "10.0.0.2",
        "ntp_class": 15, 
        "T_hb_ms": 200
    })");

    auto config = ConfigManager::LoadConfig(temp_filepath);

    EXPECT_EQ(config.ip, "10.0.0.2");
    EXPECT_EQ(config.t_hb_ms, 200);
    EXPECT_EQ(config.ntp_class, 7);
}

TEST_F(ConfigManagerTest, HandlesMissingFieldsGracefully) {
    CreateJsonFile(R"({
        "ip": "172.16.0.5"
    })"); // Brak ntp_class i T_hb_ms

    auto config = ConfigManager::LoadConfig(temp_filepath);

    EXPECT_EQ(config.ip, "172.16.0.5");
    EXPECT_EQ(config.ntp_class, 7);
    EXPECT_EQ(config.t_hb_ms, 1000);
}