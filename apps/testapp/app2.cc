#include "communication-core/sockets/stream_ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include <thread>
#include <iostream>
int main(int argc, char const *argv[])
{
    simba::com::soc::StreamIpcSocket soc{};
    soc.Init(simba::com::soc::SocketConfig{"test", 0, 0});
    while (true)
    {
        if(soc.Transmit("test",0,{10,11,12,13,14,14}).has_value()){
            std::cout<<"Send pass"<<std::endl;
        } else {
            std::cout<<"Send failed"<<std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds{1});
    }
    
    
    return 0;
}
