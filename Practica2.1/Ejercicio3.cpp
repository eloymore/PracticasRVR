#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv){
    if(argc < 4 || argc > 4){
        std::cout << "Uso: ./ejercicio3 <ipv4> <port> <comando>" << std::endl;
        return -1;
    }

    struct addrinfo* servidor;
    struct addrinfo* cliente;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        std::cerr << "Error socket: " << strerror(errno) << std::endl; 
        return -errno;
    }

    int err = getaddrinfo(argv[1], argv[2], &hints, &servidor);

    if(err != 0){
        std::cerr << "Error getaddrinfo: " << gai_strerror(err) << std::endl;
        return err;
    }

    err = sendto(sock, argv[3], 1, 0, servidor->ai_addr, servidor->ai_addrlen);
    if(err < 0){
        std::cerr << "Error sendto: " << strerror(errno) << std::endl; 
        return -errno;
    }

    char buffer[256];
    memset(&buffer, 0, 256);

    int bread = recvfrom(sock, (void*)buffer, 256, 0, servidor->ai_addr, &servidor->ai_addrlen);
    if(bread < 0){
        std::cerr << "Error recvfrom: " << strerror(errno) << std::endl; 
        return -errno;
    }

    std::cout << buffer << std::endl;

    freeaddrinfo(servidor);

    err = close(sock);
    if(err < 0){
        std::cerr << "Error close: " << strerror(errno) << std::endl; 
        return -errno;
    }

    return 0;
}