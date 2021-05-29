#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);
    
    //Serializar los campos type, nick y message en el buffer _data
    char* bin = _data;

    memcpy(bin, &type, 1);
    bin += 1;

    memcpy(bin, &nick, 8);
    bin += 8;

    memcpy(bin, &message, 80);
    bin += 80;
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char * bin = _data;

    memcpy(&type, bin, 1);
    bin += 1;

    memcpy(&nick, bin, 8);
    bin += 8;

    memcpy(&message, bin, 80);
    bin += 80;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
        ChatMessage recvMessage;
        Socket* sock;
        socket.recv(recvMessage, sock);
        switch (recvMessage.type)
        {
            case ChatMessage::LOGIN:
            {
                std::unique_ptr<Socket> u_ptr(sock);
                clients.push_back(std::move(u_ptr);
                break;
            }       
            case ChatMessage::MESSAGE:
            {
                for(auto it = clients.begin(); it != clients.end(); ++it){
                    if(!(*it->get() == *sock)){
                        socket.send(recvMessage, *it->get());
                    }
                }
                delete sock;
                break;
            }       
            case ChatMessage::LOGOUT:
            {
                auto it = clients.begin();
                while(it != clients.end() && !(*it->get() == *sock)) ++it;
                clients.erase(it);
                delete sock;
                break;
            }        
            default:
                std::cerr << "Error recv: El tipo de mensaje es incorrecto" << std::endl;
                break;
        }

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

