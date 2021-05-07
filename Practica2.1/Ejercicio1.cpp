#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char** argv){
    if(argc < 2){
        std::cout << "Falta poner la direccion" << std::endl;
        return -1;
    }

    struct addrinfo* info;
    struct addrinfo hints;
    hints.ai_family = AF_UNSPEC;

    memset(&hints, 0, sizeof(struct addrinfo));

    int err = getaddrinfo(argv[1], NULL, &hints, &info);

    if(err != 0){
        std::cerr << "Error: " << gai_strerror(err) << std::endl;
        return err;
    }
    

    for(auto i = info; i != nullptr; i = i->ai_next){

        char host[NI_MAXHOST];

        int err2 = getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if(err2 != 0){
            std::cerr << "Error: " << gai_strerror(err2) << std::endl;
            return err2;
        }

        std::cout << host << " " << i->ai_family << " " << i->ai_socktype << " " << std::endl;
    }

    return 0;
}