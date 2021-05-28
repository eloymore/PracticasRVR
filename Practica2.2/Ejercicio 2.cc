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
        char * bin = _data;

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

    // El comando od muestra en la consola el contenido de un archivo en octal por defecto u otro formato
    // El comando muestra el archivo serializado

    return 0;
}

