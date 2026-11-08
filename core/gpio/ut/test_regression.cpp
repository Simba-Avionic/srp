/**
 * @copyright Copyright (c) 2025
 */
// Regression test: F-09 was found by the audit on 8813de3 and is fixed on the current base.
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "core/file/Ifile.hpp"
#include "core/gpio/gpio_driver.hpp"

namespace {
// behaves like std::fstream based FileHandler: open() on an already open file fails
class FstreamLikeFile : public srp::core::IFileHandler {
 public:
  bool is_open = false;
  int fail_next_writes = 0;
  int opens = 0, closes = 0;
  bool open(const std::string&, const srp::core::FileMode&) override {
    if (is_open) return false;
    is_open = true; ++opens; return true;
  }
  void close() override { is_open = false; ++closes; }
  bool write(const std::string&, const bool) override {
    if (fail_next_writes > 0) { --fail_next_writes; return false; }
    return true;
  }
  std::optional<std::string> read() override { return std::string("1"); }
};
}  // namespace

/**
 * F-09 (wysoka) core/gpio/gpio_driver.cpp:61-69 (setValue), also :28-37, :73-83, :86.
 * On a failed write() the error path returns without close(); the shared fstream stays open and every later
 * open() fails. Expected: after one transient write error the next setValue() returns kOk.
 * (Before the fix: kError forever = permanent loss of pin control, e.g. igniter off / servo power.)
 */
TEST(Regression, F09_SetValueAfterTransientWriteErrorStillWorks) {
  auto f = std::make_unique<FstreamLikeFile>();
  auto* raw = f.get();
  raw->fail_next_writes = 1;
  srp::core::gpio::GpioDriver drv(std::move(f));
  EXPECT_EQ(drv.setValue(13, 1), srp::core::ErrorCode::kError);  // injected write failure
  auto second = drv.setValue(13, 0);
  EXPECT_EQ(second, srp::core::ErrorCode::kOk)
      << "F-09: after a failed write() the file handler is never closed (opens=" << raw->opens
      << " closes=" << raw->closes << "), next setValue fails (gpio_driver.cpp:61-69)";
}
