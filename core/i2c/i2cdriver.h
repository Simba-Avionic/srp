/**
 * @file i2cdriver.h
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief This file defines I2C driver
 * @version 0.1
 * @date 2024-01-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_I2C_I2CDRIVER_H_
#define CORE_I2C_I2CDRIVER_H_

#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <optional>
#include "core/common/error_code.h"
namespace simba {
namespace core {
class I2C{
 public:
    ErrorCode init(const std::string& path);
    ErrorCode init();
    // czy to wgl działa???
    std::optional<uint8_t> Read(const uint8_t address);
    /**
     * @brief ERPROM Need Read And Write 
     * 
     * @param address 
     * @param reg 
     * @return std::optional<uint8_t> 
     */

    void openFile();
    void closeFile();
    ErrorCode connectTo(const uint8_t deviceAddress);
    std::optional<uint8_t> ReadAndWrite(const uint8_t reg);
    ErrorCode Write(const uint8_t reg, uint8_t data);
 private:
    std::string path;
    int file;
    uint8_t address;
};
}  // namespace core
}  // namespace simba
#endif  // CORE_I2C_I2CDRIVER_H_
