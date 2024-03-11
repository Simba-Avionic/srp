/**
 * @file status.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-09
 * 
 * @copyright Copyright (c) 2024
 * 
*/

#ifndef DIAG_EXEC_CONTROLLER_STATUS_HPP_
#define DIAG_EXEC_CONTROLLER_STATUS_HPP_

namespace simba {
namespace diag {
namespace exec {

enum Status {
    Start_up = 0x00,
    Running = 0x01,
    Running_with_dtc = 0x02,
    Running_after_dtc = 0x03,
    Error = 0x04,
    Running_after_error = 0x05,
    Running_ignore_DTC = 0x06,
    Running_ignore_error = 0x07
};

}  // namespace exec
}  // namespace diag
}  // namespace simba

#endif  // DIAG_EXEC_CONTROLLER_STATUS_HPP_
