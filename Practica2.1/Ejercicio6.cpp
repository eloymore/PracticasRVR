#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <thread>

#define MAX_THREADS 5

class MessageThread {
public:
    MessageThread(int sockfd) : _sockfd(sockfd) {
    }

    void recvMessage(){

        struct sockaddr cliente;
        socklen_t clienteLength = sizeof(struct sockaddr);  
        char buffer[2];
        memset(&buffer, 0, 2);
        while(true){
            int bread = recvfrom(_sockfd, (void*)buffer, 2, 0, &cliente, &clienteLength);
            if(bread < 0){
                std::cerr << "Error recvfrom: " << strerror(errno) << std::endl; 
                continue;
            }

            char host[NI_MAXHOST];
            char serv[NI_MAXSERV];

            int err2 = getnameinfo(&cliente, clienteLength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
            if(err2 != 0){
                std::cerr << "Error getnameinfo: " << gai_strerror(err2) << std::endl;
                continue;
            }

            sleep(3);

            std::cout << "thread " << std::this_thread::get_id() << ": " << bread << " bytes de " << host << ":" << serv << std::endl;

            char sendToBuffer[256];
            size_t sendToSize = 0;
            switch (*buffer)
            {
            case 't':{
                time_t rawt = time(nullptr);
                struct tm* t;
                t = localtime(&rawt);
                sendToSize = strftime(sendToBuffer, 256, "%X %p", t);
                break;
                }
            case 'd':{
                time_t rawt = time(nullptr);
                struct tm* t;
                t = localtime(&rawt);
                sendToSize = strftime(sendToBuffer, 256, "%y-%m-%d", t);
                break;
                }
            default:{
                std::cerr << "Comando no soportado " << *buffer << std::endl;
                break;
                }
            }

            if(sendToSize > 0){
                sendToBuffer[sendToSize] = '\0';
                int err = sendto(_sockfd, sendToBuffer, sendToSize, 0, &cliente, clienteLength);
                if(err < 0){
                    std::cerr << "Error sendto: " << strerror(errno) << std::endl; 
                    continue;
                }
            }
        }
    }

private:
    int _sockfd;
};

int main(int argc, char** argv){
    if(argc < 3 || argc > 3){
        std::cout << "Uso: ./ejercicio6 <ipv4> <port>" << std::endl;
        return -1;
    }

    struct addrinfo* info;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int err = getaddrinfo(argv[1], argv[2], &hints, &info);
    if(err != 0){
        std::cerr << "Error getaddrinfo: " << gai_strerror(err) << std::endl;
        return err;
    }

    int sock = socket(info->ai_family, info->ai_socktype, 0);
    if(sock < 0){
        std::cerr << "Error socket: " << strerror(errno) << std::endl; 
        return -errno;
    }

    err = bind(sock, info->ai_addr, info->ai_addrlen);
    if(err < 0){
        std::cerr << "Error bind: " << strerror(errno) << std::endl; 
        return -errno;
    }

    freeaddrinfo(info);

    char mainbuffer[2];
    memset(&mainbuffer, 0, 2);

    for(int i = 0; i < MAX_THREADS; ++i){
        MessageThread* mthread = new MessageThread(sock);

        std::thread([mthread](){
            mthread->recvMessage();
            delete mthread;
        }).detach();
    }

    std::cin >> mainbuffer;
    while(strcmp(mainbuffer, "q\0") != 0){
        std::cin >> mainbuffer;
    }

    std::cout << "Saliendo...\n";
    err = close(sock);
    if(err < 0){
        std::cerr << "Error close: " << strerror(errno) << std::endl; 
        return -errno;
    }

    return 0;
}