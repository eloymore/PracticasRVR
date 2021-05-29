#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t data_size = 80 + 2 * sizeof(int16_t);
        alloc_data(data_size);

        char * bin = _data;

        memcpy(bin, name, MAX_NAME);
        bin += MAX_NAME;

        memcpy(bin, &x, sizeof(int16_t));
        bin += sizeof(int16_t);

        memcpy(bin, &y, sizeof(int16_t));
        bin += sizeof(int16_t);
    }

    int from_bin(char * data)
    {
        int32_t data_size = 80 + 2 * sizeof(int16_t);
        alloc_data(data_size);

        char * bin = data;
        memcpy(name, bin, MAX_NAME);
        bin += MAX_NAME;

        memcpy(&x, bin, sizeof(int16_t));
        bin += sizeof(int16_t);

        memcpy(&y, bin, sizeof(int16_t));
        bin += sizeof(int16_t);
        return 0;
    }


public:
    static const size_t MAX_NAME = 80;

    char name[MAX_NAME];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_w("Player_ONE", 123, 987);

    // Serializar el objeto one_w
    one_w.to_bin();
    // Escribir la serialización en un fichero
    int fd = open("./data_jugador", O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (fd == -1){
        std::cout << "Error open: " << strerror(errno) << std::endl;
        return -errno;
    }
    if(write(fd, one_w.data(), one_w.size()) == -1){
        std::cout << "Error write: " << strerror(errno) << std::endl;
        return -errno;
    }
    // Cerrar el archivo
    if(close(fd) == -1){
        std::cout << "Error close: " << strerror(errno) << std::endl;
        return -errno;
    }
    // Abre de nuevo el archivo, ahora para leer solo
    fd = open("./data_jugador", O_RDONLY, 0666);
    if (fd == -1){
        std::cout << "Error open2: " << strerror(errno) << std::endl;
        return -errno;
    }
    // Lee los datos
    char data[one_w.size()];
    if(read(fd, data, one_w.size()) == -1){
        std::cout << "Error read: " << strerror(errno) << std::endl;
        return -errno;
    }

    Jugador j2("", 0, 0);
    j2.from_bin(data);

    // Cerrar el archivo
    if(close(fd) == -1){
        std::cout << "Error close: " << strerror(errno) << std::endl;
        return -errno;
    }

    // Se muestra por consola
    std::cout << j2.name << "\t" << j2.x << "\t" << j2.y << std::endl;
    return 0;
}

