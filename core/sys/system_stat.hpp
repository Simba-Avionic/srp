/**
 * @file system_stat.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-02-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef CORE_SYS_SYSTEM_STAT_HPP_
#define CORE_SYS_SYSTEM_STAT_HPP_

namespace srp {
namespace core {
namespace stat {

class SystemStats {
 public:
  static float get_ram_usage();
  static double get_cpu_usage();
  static double get_disk_space();
};

}  // namespace stat
}  // namespace core
}  // namespace srp

#endif  // CORE_SYS_SYSTEM_STAT_HPP_
