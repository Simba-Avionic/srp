/**
 * @file discovery_manager_test.cc
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-05-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <chrono>
#include "mw/timestamp_mw/ntp/discovery/discovery_manager.hpp"

using namespace srp::tinyNTP;

class DiscoveryManagerTest : public ::testing::Test {
 protected:
    DiscoveryManager discoveryManager;
    
    NodeInfo default_local_node;

    void SetUp() override {
        default_local_node.ip = "192.168.0.50";
        default_local_node.ntp_class = 5;
        default_local_node.holdover = true;
        default_local_node.last_seen = std::chrono::steady_clock::now(); 
    }
};

TEST_F(DiscoveryManagerTest, LocalNodeIsBestWhenNetworkEmpty) {
    auto best_master = discoveryManager.GetBestMaster(default_local_node);
    
    EXPECT_FALSE(best_master.has_value());
}

TEST_F(DiscoveryManagerTest, ExternalNodeWinsByClass) {
    discoveryManager.UpdateNode("192.168.0.100", 2, true);
    
    auto best_master = discoveryManager.GetBestMaster(default_local_node);
    
    ASSERT_TRUE(best_master.has_value());
    EXPECT_EQ(best_master->ip, "192.168.0.100");
}

TEST_F(DiscoveryManagerTest, LocalNodeWinsByClass) {
    discoveryManager.UpdateNode("192.168.0.100", 7, true);
    
    auto best_master = discoveryManager.GetBestMaster(default_local_node);
    
    ASSERT_FALSE(best_master.has_value());
}

TEST_F(DiscoveryManagerTest, ExternalNodeWinsByHoldover) {
    discoveryManager.UpdateNode("192.168.0.100", 5, false);
    
    auto best_master = discoveryManager.GetBestMaster(default_local_node);
    
    ASSERT_TRUE(best_master.has_value());
    EXPECT_EQ(best_master->ip, "192.168.0.100");
}

TEST_F(DiscoveryManagerTest, ExternalNodeWinsByIpTieBreaker) {
    discoveryManager.UpdateNode("192.168.0.10", 5, true);
    auto best_master = discoveryManager.GetBestMaster(default_local_node);
    
    ASSERT_TRUE(best_master.has_value());
    EXPECT_EQ(best_master->ip, "192.168.0.10");

    discoveryManager.UpdateNode("10.168.0.10", 5, true);
    best_master = discoveryManager.GetBestMaster(default_local_node);

    ASSERT_TRUE(best_master.has_value());
    EXPECT_EQ(best_master->ip, "10.168.0.10");
}

TEST_F(DiscoveryManagerTest, LocalNodeDefeatsLowerIpWithBetterClass) {
    discoveryManager.UpdateNode("10.0.0.1", 7, false);
    
    auto best_master = discoveryManager.GetBestMaster(default_local_node);
    
    EXPECT_FALSE(best_master.has_value());
}