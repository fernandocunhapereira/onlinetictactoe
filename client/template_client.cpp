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

    SOCKET conn_socket = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Socket number: " << conn_socket << std::endl;
    
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(conn_socket, (sockaddr*) &server_addr, sizeof(sockaddr_in))) {
        int err = WSAGetLastError();
        std::cout << "Connect error: " << err << std::endl;
        closesocket(conn_socket);
        WSACleanup();
        return -2;
    }

    game_state s;
    while (true) {
        // TODO:
        // Your code here.
        recv(conn_socket,buffer,BUFLEN,0);
        unwrap(buffer,BUFLEN,s);
        s.check_end();
        s.print_board();
        s.play_o(s.read_cell());
        s.print_board();
        s.get_winner();
        int tam = wrap(buffer,s);
        send(conn_socket,buffer,BUFLEN,0);
    }

    // Close connection
    shutdown(conn_socket, SD_BOTH);
    closesocket(conn_socket);

    WSACleanup();
    return 0;

}