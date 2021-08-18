#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h> 
#include <string>
#include <fstream>

#define PORT 2400

int main() {
    
    sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    // saddr.sin_addr.s_addr = INADDR_ANY;
    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == 0) {
        std::cout << "Socket error\n";
        return -1;
    }
    
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);
    
    if(connect(serverSocket, (sockaddr*)&saddr, sizeof(saddr)) < 0) {
        std::cout << "Connection error\n";
        return -1;
    }
    
    std::ofstream clientFile("client.txt", std::ios::app);
    char data[4096] = {0};
    char received[4096] = {0};
    
    read(serverSocket, received, 4096);
    
    std::cout << "Connected on port "<< received <<"\n\nRegister | Login\n\n";
    clientFile << "Connected on port "<< received <<"\n\nRegister | Login\n\n";
    
    while(true) {
        memset(data, 0, 4096);
        memset(received, 0, 4096);
        
        std::cin.getline(data, 4096);
        
        if((data[0] == 'q' && data[1] == '\0') || data[0] == '\0') {
            break;
        }
        send(serverSocket, data, strlen(data), 0);
        
        std::cout << "Message sent!\n";
        clientFile << "Message sent!\n";
        
        // read(serverSocket, received, 4096);
        
        int bytesRecv = recv(serverSocket, received, 4096, 0);
        if(bytesRecv == -1) {
            std::cout << "There is a connection issue\n";
            clientFile << "There is a connection issue\n";
            break;
        }
        if(bytesRecv == 0) {
            std::cout << "The server disconnected\n";
            clientFile << "The server disconnected\n";
            break;
        }
        
        std::cout << "server> " << received << std::endl << std::endl;
        clientFile << "server> " << received << std::endl << std::endl;
    }

    clientFile.close();
    close(serverSocket);
    
    return 0;
}
