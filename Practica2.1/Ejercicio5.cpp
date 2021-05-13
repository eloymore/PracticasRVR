#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv){
    if(argc < 3 || argc > 3){
        std::cout << "Uso: ./ejercicio5 <ipv4> <port>" << std::endl;
        return -1;
    }

    struct addrinfo* servidor;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        std::cerr << "Error socket: " << strerror(errno) << std::endl; 
        return -errno;
    }

    int err = getaddrinfo(argv[1], argv[2], &hints, &servidor);

    if(err != 0){
        std::cerr << "Error getaddrinfo server: " << gai_strerror(err) << std::endl;
        return err;
    }

    err = connect(sock, servidor->ai_addr, servidor->ai_addrlen);
    if(err != 0){
        std::cerr << "Error connect: " << strerror(errno) << std::endl; 
        return -errno;
    }

    char outbuffer[256];
    char inbuffer[256];

    while(strcmp(outbuffer, "Q\0") != 0){

        std::cin >> outbuffer;
        int err = send(sock, outbuffer, strlen(outbuffer), 0);
        if(err < 0){
            std::cerr << "Error send: " << strerror(errno) << std::endl; 
            return -errno;
        }

        if(strcmp(outbuffer, "Q\0") == 0) break;

        int bread = recv(sock, (void*)inbuffer, 255, 0);
        if(bread < 0){
            std::cerr << "Error recv: " << strerror(errno) << std::endl; 
            return -errno;
        } else if(bread == 0) break;

        std::cout << inbuffer << std::endl;
    }

    close(sock);
    freeaddrinfo(servidor);
    return 0;
}