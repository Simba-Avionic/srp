#include "communication-core/sockets/stream_ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include <thread>
#include <iostream>
int main(int argc, char const *argv[])
{
    simba::com::soc::StreamIpcSocket soc{};
    
    soc.SetRXCallback([](const std::string &ip, const std::uint16_t &port,
                         std::vector<std::uint8_t> payload)
                      { std::cout << "RX callback"<<std::endl;
                      for (const auto& i : payload){
                        std::cout<<std::to_string(i)<<" ";
                      }
                      std::cout<<std::endl;
                        return std::vector<uint8_t>{10, 11, 12}; });
    if(soc.Init(simba::com::soc::SocketConfig{"test", 0, 0}) != 0x00){
        std::cout<<"Init error"<<std::endl;
    }
    soc.StartRXThread();
    std::this_thread::sleep_for(std::chrono::days{1});
    return 0;
}
