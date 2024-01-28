/**
 * @file exec_header_factory.hpp
 * @author Wiktor Kawka (asinesak353@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef DIAG_EXEC_FACTORIES_EXEC_MSG_FACTORY_HPP_
#define DIAG_EXEC_FACTORIES_EXEC_MSG_FACTORY_HPP_

#include "diag/exec/data/exec_header.hpp"
#include <vector>
#include <memory>

namespace simba {
namespace diag {
namespace exec {

class ExecMsgFactory {
 public:
  /**
   * @brief  This function return ready bit stream to send
   *
   * @param header
   * @return std::vector<uint8_t>
   */
  std::vector<uint8_t> GetBuffer(std::shared_ptr<diag::exec::ExecHeader> header);

  /**
   * @brief Creat header object from raw data
   *
   * @param raw_data
   * @return std::shared_ptr<execHeader>
   */
  std::shared_ptr<diag::exec::ExecHeader> GetHeader(
    std::vector<uint8_t> raw_data);
};

}  // namespace exec
}  // namespace diag
}  // namespace simba

#endif  // DIAG_exec_FACTORIES_exec_MSG_FACTORY_HPP_
