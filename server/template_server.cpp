#include <Winsock2.h>
#include <Ws2tcpip.h>
#include "tictactoe.hpp"

#define SERVER_IP "127.0.0.1"
#define PORT 51234
#define BUFLEN 100

int wrap(char* buf, game_state &s) {
    game_state* buf2 = (game_state*) buf;
    buf2[0] = s;
    return sizeof(game_state);
}

void unwrap(char* buf, int n, game_state& s) {
    game_state* buf2 = (game_state*) buf;
    s = buf2[0];
}

int main () {
    char *buffer = new char[BUFLEN];

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        std::cout << "start error" << std::endl;
        return -1;
    }

    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Socket number: " << listen_socket << std::endl;
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    if (bind(listen_socket, (sockaddr*) &addr, sizeof(sockaddr_in))) {
        std::cout << "Binding error" << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return -2;
    }

    if (listen(listen_socket, SOMAXCONN)) {
        std::cout << "Listen error" << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return -3;
    }

    sockaddr_in client_addr;
    int addr_len = sizeof(sockaddr_in);
    std::cout << "Waiting for client connection..." << std::endl;
    SOCKET conn_socket = accept(listen_socket, (sockaddr*) &client_addr, &addr_len);
    if (conn_socket <= 0) {
        int err = WSAGetLastError();
        std::cout << "Accept error: " << err << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return -4;
    }

    game_state s;
    while (true) {
        // TODO:
        // Your code here.
        s.check_end();
        s.print_board();
        s.play_x(s.read_cell());
        s.print_board();
        s.get_winner();
        int tam = wrap(buffer,s);
        send(conn_socket,buffer,BUFLEN,0);
        recv(conn_socket,buffer,BUFLEN,0);
        unwrap(buffer,BUFLEN,s);
    }

    // Close connection
    shutdown(conn_socket, SD_BOTH);
    closesocket(conn_socket);

    // Close listening socket
    closesocket(listen_socket);
    WSACleanup();
    return 0;

}