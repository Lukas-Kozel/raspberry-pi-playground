#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

void func(int connfd){
    char buff[MAX];
    int n;
    
    while(1) {
        bzero(buff,MAX);

        read(connfd,buff,sizeof(buff));
        printf("From client: %s\t To client : ", buff); 
        bzero(buff, MAX); 
        n = 0;

        while ((buff[n++] = getchar()) != '\n'){};

        write(connfd,buff,sizeof(buff));

                // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    };
}

int main(){
    int sockfd, connfd, len;
    struct sockaddr_in serveraddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if(sockfd == -1){
        printf("socket creation failed...\n"); 
        exit(0);    
    }
    else{
        printf("Socket succesfully created");
    }
    bzero(&serveraddr, sizeof(serveraddr)); // first parameter is pointer to a memory area to be filled with zeroes, second is number of bytes

    // assign IP, Port
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification 
    if((bind(sockfd, (SA*)&serveraddr, sizeof(serveraddr))) != 0){
        printf("socket bind failed \n");
        exit(0);
    }
    else{
        printf("Socket successfully binded..\n");
    }

    if((listen(sockfd,5)) != 0){
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    
    len = sizeof(cli);

    //accept data packet from client
    connfd = accept(sockfd,(SA*) &cli, &len);
    if(connfd < 0) {
            printf("server accept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server accept the client...\n"); 

    func(connfd);

    close(sockfd);
}