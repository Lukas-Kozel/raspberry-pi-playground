#include <iostream>
#include <netdb.h> 
#include <netinet/in.h> 
#include <cstdlib> 
#include <cstring> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

void func(int connfd){
    char buff[MAX];
    int n;

    while(true) {
        std::memset(buff, 0, MAX);

        read(connfd, buff, sizeof(buff));
        std::cout << "From client: " << buff << "\t To client : ";
        std::memset(buff, 0, MAX);
        n = 0;

        while ((buff[n++] = getchar()) != '\n') {};

        write(connfd, buff, sizeof(buff));

        // if msg contains "exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) {
            std::cout << "Server Exit...\n";
            break;
        }
    }
}

int main(){
    int sockfd, connfd, len;
    struct sockaddr_in serveraddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Socket creation failed...\n";
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Socket successfully created\n";
    }
    std::memset(&serveraddr, 0, sizeof(serveraddr)); // first parameter is pointer to a memory area to be filled with zeroes, second is number of bytes

    // assign IP, Port
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&serveraddr, sizeof(serveraddr))) != 0) {
        std::cerr << "Socket bind failed\n";
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Socket successfully binded..\n";
    }

    if ((listen(sockfd, 5)) != 0) {
        std::cerr << "Listen failed...\n";
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Server listening..\n";
    }

    len = sizeof(cli);

    //accept data packet from client
    connfd = accept(sockfd, (SA*)&cli, reinterpret_cast<socklen_t*>(&len));
    if (connfd < 0) {
        std::cerr << "Server accept failed...\n";
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Server accepted the client...\n";
    }

    func(connfd);

    close(sockfd);
}
