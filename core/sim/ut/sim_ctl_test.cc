/**
 * @file sim_ctl_test.cc
 * @brief UT of the sim control channel (core/sim/sim_ctl.hpp): matching of ctl lines by content, half-written and
 *        truncated files, strict hex/number parsing.
 *
 * @copyright Copyright (c) 2025
 */
#include <gtest/gtest.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include "core/sim/sim_ctl.hpp"

namespace {
using srp::core::sim::Cmd;
using srp::core::sim::Ctl;

class SimCtlTest : public ::testing::Test {
 protected:
  void SetUp() override {
    char tmpl[] = "/tmp/srpsim_ctlXXXXXX";
    ASSERT_NE(mkdtemp(tmpl), nullptr);
    dir_ = tmpl;
    setenv("SRP_SIM_DIR", tmpl, 1);
    Apply();  // no ctl file yet: clears the state left by the previous test
  }

  void Write(const std::string& body, bool append = false) {
    std::ofstream f(dir_ + "/ctl", append ? std::ios::app : std::ios::trunc);
    f << body;
  }

  // number of "rx" commands applied for the first time
  int Apply() {
    int n = 0;
    Ctl::Get().Visit([&n](Cmd& c) {
      if (!c.applied && c.tok.size() >= 3 && c.tok[2] == "rx") {
        c.applied = true;
        ++n;
      }
    });
    return n;
  }

  std::string dir_;
};

TEST_F(SimCtlTest, RemovingALineAboveDoesNotReplayTheNextOnes) {
  Write("gpio 1 value 1\nuart p rx 01\n");
  EXPECT_EQ(Apply(), 1);
  Write("uart p rx 01\n");  // the first line was removed
  EXPECT_EQ(Apply(), 0) << "S1: rx was applied again after a line above it was removed";
}

TEST_F(SimCtlTest, InsertingALineAboveDoesNotReplayTheNextOnes) {
  Write("uart p rx 01\n");
  EXPECT_EQ(Apply(), 1);
  Write("gpio 1 value 1\nuart p rx 01\n");
  EXPECT_EQ(Apply(), 0);
}

TEST_F(SimCtlTest, HalfWrittenLastLineIsIgnoredUntilComplete) {
  Write("uart p rx 01\nuart p rx 02");
  EXPECT_EQ(Apply(), 1) << "S2: the line without a newline must not be applied";
  Write("\n", true);
  EXPECT_EQ(Apply(), 1) << "the completed line is applied once, the first one is not replayed";
  EXPECT_EQ(Apply(), 0);
}

TEST_F(SimCtlTest, TruncatedFileDoesNotReplayRxAfterRewrite) {
  Write("uart p rx 05\n");
  EXPECT_EQ(Apply(), 1);
  Write("");  // `cat > ctl` shows an empty file for a moment
  EXPECT_EQ(Apply(), 0);
  Write("uart p rx 05\n");
  EXPECT_EQ(Apply(), 0) << "S2: identical line after a truncate was applied again";
}

TEST_F(SimCtlTest, RemovedFileClearsTheState) {
  Write("uart p rx 05\n");
  EXPECT_EQ(Apply(), 1);
  ASSERT_EQ(unlink((dir_ + "/ctl").c_str()), 0);
  EXPECT_EQ(Apply(), 0);
  Write("uart p rx 05\n");
  EXPECT_EQ(Apply(), 1);  // rm + write = deliberately applied again
}

TEST(SimParseTest, UnHexRejectsOddLengthAndNonHex) {
  EXPECT_EQ(srp::core::sim::UnHex({"0a0b"}, 0), (std::vector<uint8_t>{0x0a, 0x0b}));
  EXPECT_EQ(srp::core::sim::UnHex({"0x0a", "0B"}, 0), (std::vector<uint8_t>{0x0a, 0x0b}));
  EXPECT_THROW(srp::core::sim::UnHex({"0a0"}, 0), std::invalid_argument);
  EXPECT_THROW(srp::core::sim::UnHex({"1z"}, 0), std::invalid_argument);
}

TEST(SimParseTest, ParseNumRejectsTrailingJunkAndOctal) {
  EXPECT_EQ(srp::core::sim::ParseNum("013"), 13);        // pin numbers are decimal
  EXPECT_EQ(srp::core::sim::ParseNum("0x48", 0), 0x48);  // i2c addresses may be hex
  EXPECT_THROW(srp::core::sim::ParseNum("12abc"), std::invalid_argument);
}
}  // namespace
