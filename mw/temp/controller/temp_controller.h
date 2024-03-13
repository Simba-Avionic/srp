/**
 * @file temp_controller.h
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_TEMP_CONTROLLER_TEMP_CONTROLLER_H_
#define MW_TEMP_CONTROLLER_TEMP_CONTROLLER_H_

#include <string>
#include <cstdio>
#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <sys/types.h>  // NOLINT
#include <sys/socket.h>  // NOLINT
#include <netinet/in.h>  // NOLINT

#include <memory>
#include <vector>
#include <unordered_map>

#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "core/logger/Logger.h"

#include "mw/temp/subscribe_msg/subscribe_header.h"
#include "mw/temp/subscribe_msg/subscribe_msg_factory.h"

#include "mw/temp/temp_reading_msg/temp_reading_msg.h"
#include "mw/temp/temp_reading_msg/temp_reading_msg_factory.h"

#include "mw/temp/service/temp_service.h"

namespace simba {
namespace mw {
namespace temp {

class TempController {
 protected:
    uint16_t service_id;
    com::soc::IpcSocket sub_sock_{};
    std::vector<TempReading> latest_readings;
    simba::mw::temp::TempReadingMsgFactory factory;

 private:
    virtual simba::core::ErrorCode Init(uint16_t service_id);

 public:
    virtual void Callback(const std::string& ip, const std::uint16_t& port,
        const std::vector<std::uint8_t> data);

    virtual simba::core::ErrorCode Subscribe();
};

}  // namespace temp
}  // namespace mw
}  // namespace simba

#endif  // MW_TEMP_CONTROLLER_TEMP_CONTROLLER_H_
