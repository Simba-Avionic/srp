/**
 * @file ITempDriver.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <optional>
#include <vector>
#include <cstdint>
#include <string>
#include "ara/core/result.h"

namespace simba {
namespace core {
namespace temp {

class ITempDriver {
  public:
   ara::core::Result<double> ReadTemp(std::string sensorPhysicalID);
   ara::core::Result<bool> SetResolution(std::string sensorPhysicalID);
};

}  // namespace temp
}  // namespace core
}  // namespace simba