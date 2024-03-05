#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

int main() {
    // Create a socket
    int serverSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error: Couldn't create socket\n";
        return 1;
    }

    // Bind the socket to an address
    sockaddr_un serverAddress;
    serverAddress.sun_family = AF_UNIX;
    strcpy(serverAddress.sun_path, "/tmp/socket_server");

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        std::cerr << "Error: Couldn't bind socket\n";
        return 1;
    }

    std::cout << "Server listening on Unix domain socket /tmp/socket_server...\n";

    // Receive data from client
    char buffer[1024] = {0};
    sockaddr_un clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int bytesRead = recvfrom(serverSocket, buffer, sizeof(buffer), 0, 
        reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);
    if (bytesRead == -1) {
        std::cerr << "Error: Couldn't receive data from client\n";
        return 1;
    }

    std::cout << "Received message from client: " << buffer << std::endl;

    // Send data back to client
    const char* message = "Hello from server";
    int bytesSent = sendto(serverSocket, message, strlen(message), 0, 
        reinterpret_cast<sockaddr*>(&clientAddress), clientAddressSize);
    if (bytesSent == -1) {
        std::cerr << "Error: Couldn't send data to client\n";
        return 1;
    }

    std::cout << "Message sent to client\n";

    // Close socket
    close(serverSocket);

    // Unlink socket file
    unlink("/tmp/socket_server");

    return 0;
}
