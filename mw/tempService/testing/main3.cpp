#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

int main() {
    // Create a socket
    int clientSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error: Couldn't create socket\n";
        return 1;
    }

    // Set up server address
    sockaddr_un serverAddress;
    serverAddress.sun_family = AF_UNIX;
    strcpy(serverAddress.sun_path, "/tmp/socket_server");

    // Send message to server
    const char* message = "Hello from client";
    int bytesSent = sendto(clientSocket, message, strlen(message), 0, 
        reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));
    if (bytesSent == -1) {
        std::cerr << "Error: Couldn't send data to server\n";
        close(clientSocket);
        return 1;
    }

    std::cout << "Message sent to server\n";

    // Receive response from server
    char buffer[1024] = {0};
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        std::cerr << "Error: Couldn't receive data from server\n";
        close(clientSocket);
        return 1;
    }

    std::cout << "Received message from server: " << buffer << std::endl;

    // Close socket
    close(clientSocket);

    return 0;
}
