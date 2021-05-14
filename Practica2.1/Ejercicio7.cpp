#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <thread>

class ClientThread{
public:
    ClientThread(int sock): _sock(sock){}

    void HandleClient(){
        char buffer[256];

        while(true){
            int bread = recv(_sock, (void*)buffer, 255, 0);
            if(bread < 0){
                std::cerr << "Error recv: " << strerror(errno) << std::endl; 
                continue;
            } else if(bread == 0) break;

            buffer[bread] = '\0';
            int err = send(_sock, buffer, bread + 1, 0);
            if(err < 0){
                std::cerr << "Error send: " << strerror(errno) << std::endl; 
                continue;
            }
        }

        std::cout << "Conexión terminada\n";
    }

    virtual ~ClientThread(){
        close(_sock);
    }
private:
    int _sock;
};

int main(int argc, char** argv){
    if(argc < 3 || argc > 3){
        std::cout << "Uso: ./ejercicio7 <ipv4> <port>" << std::endl;
        return -1;
    }

    struct addrinfo* servidor;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    int err = getaddrinfo(argv[1], argv[2], &hints, &servidor);

    if(err != 0){
        std::cerr << "Error getaddrinfo cliente: " << gai_strerror(err) << std::endl;
        return err;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        std::cerr << "Error socket: " << strerror(errno) << std::endl; 
        return -errno;
    }

    err = bind(sock, servidor->ai_addr, servidor->ai_addrlen);
    if(err < 0){
        std::cerr << "Error bind: " << strerror(errno) << std::endl; 
        return -errno;
    }

    freeaddrinfo(servidor);

    err = listen(sock, 0);
    if(err < 0){
        std::cerr << "Error listen: " << strerror(errno) << std::endl; 
        return -errno;
    }

    struct sockaddr cliente;
    socklen_t clienteLength = sizeof(sockaddr);

    while(true){
        int sockConn = accept(sock, &cliente, &clienteLength);
        if(sockConn < 0){
            std::cerr << "Error accept: " << strerror(errno) << std::endl; 
            return -errno;
        }

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        
        int err = getnameinfo(&cliente, clienteLength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        if(err != 0){
            std::cerr << "Error getnameinfo: " << gai_strerror(err) << std::endl;
            continue;
        }
        
        std::cout << "Conexión desde " << host << " " << serv << std::endl;

        ClientThread* mthread = new ClientThread(sockConn);

        std::thread([mthread](){
            mthread->HandleClient();
            delete mthread;
        }).detach();
    }

    close(sock);
}