#include <string>
#include <unordered_map>
#include <stdio.h>
#include <inttypes.h>
#include <iostream>

#include <fstream>
#include <memory>
#include <vector>

#include "core/application/application_mw.h"
#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "communication-core/network-data/network_data_structure.h"

std::chrono::milliseconds timespan(1000);

int main() {
    std::vector<uint8_t> payload{0,1};

    std::cout << "TempController is running!" << std::endl;
    simba::com::soc::IpcSocket sock_{};
    sock_.Init(simba::com::soc::SocketConfig("SIMBA.TEMP.CONTROLLER", 0, 0));

    while (true)
    {
        if (!sock_.SocketExist("/tmp/simba_temp_service"))
        {
            std::cout << "socket not exists" << std::endl;
        }
        auto ret = sock_.Transmit("simba_temp_service", 0, payload);


        std::this_thread::sleep_for(timespan);
    }
}
