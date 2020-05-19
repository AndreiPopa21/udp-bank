#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <map>
#include <string>

#include "Account.h"

using namespace std;

#define ARGC 2
#define BUFLEN 100
#define ACCOUNTS 20
#define SENDTO "send_to"
#define SHOWSUM "show_sum"
#define GET "get"
#define EXIT "exit"

void close_server(int sockfd);

int main (int argc, char* argv[]) {

    int sockfd;
    fd_set read_fds;
    fd_set tmp_fds;
    struct sockaddr_in serv_addr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    char buf[BUFLEN];
    map<int, Account*> bank;
    


    if (argc != ARGC) {
        cout << "Wrong number of arguments (" << argc << ") should be "<< ARGC << endl;
        exit(0); 
    }

    
    //..........init UDP socket..........
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        printf("[server] Failed to create UDP socket\n");
        exit(0);
    }
    cout << "[server] Succesfully created UDP client\n";


    //.........preparing connection........
    bzero(&serv_addr, sizeof(struct sockaddr_in));
    int port = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;


    //........bind UDP socket..............
    if ((bind(sockfd, (struct sockaddr*)&serv_addr, socklen)) < 0) {
        cout << "[server] Failed to bind UDP socket" << endl;
        exit(0);
    }
    cout << "[server] Succesfully bind UDP socket" << endl;


    //........creating bank accounts.........
    for (int i = 0; i < ACCOUNTS; i++) {
        Account* acc = new Account(i);
        bank.insert({i, acc});
    }


    //..........init multiplex............
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    FD_SET(sockfd, &read_fds);
    int fdmax = sockfd;

    while (true) {
        tmp_fds = read_fds;
        if ((select(fdmax+1, &tmp_fds, NULL, NULL, NULL)) < 0) {
            cout << "[server] Select failed\n";
            close_server(sockfd);
        }

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {
                if (i == sockfd) {
                    struct sockaddr_in cli_addr;
                    bzero(&cli_addr, sizeof(struct sockaddr));
                    bzero(buf, BUFLEN);
                    socklen_t clilen = sizeof(cli_addr);

                    int bytes_recv = recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &clilen);
                    if (bytes_recv <= 0) {
                        continue;
                    }
                    printf("[client] S-au primit %d bytes\n", bytes_recv);
                    char* client_ip = inet_ntoa(cli_addr.sin_addr);
                    int client_port = ntohs(cli_addr.sin_port);

                    printf("[server] Client IP: %s\n", client_ip);
                    printf("[server] Client Port: %d\n", client_port);
                    printf("[server] Client message: %s\n", buf);

                    char* command = strtok(buf, " ");
                    if (strcmp(command, SENDTO) == 0) {
                        int id = atoi(strtok(NULL, " "));
                        int sum = atoi(strtok(NULL, " "));
                        bank.at(id)->add(sum);
                        bank.at(id)->show_sum();
                        bzero(buf, BUFLEN);
                        strcpy(buf,"OK");
                        sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&cli_addr, clilen);
                    }

                    FD_CLR(sockfd, &tmp_fds);
                }
            }
        }
    }


    close_server(sockfd);


    //.......incoming clients...............
   /* struct sockaddr_in cli_addr;
    bzero(&cli_addr, sizeof(struct sockaddr));
    bzero(buf, BUFLEN);
    socklen_t clilen = sizeof(cli_addr);

    int bytes_recv = recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &clilen);
    char* client_ip = inet_ntoa(cli_addr.sin_addr);
    int client_port = ntohs(cli_addr.sin_port);

    printf("[server] Client IP: %s\n", client_ip);
    printf("[server] Client Port: %d\n", client_port);
    printf("[server] Client message: %s\n", buf);


    bzero(buf, BUFLEN);
    bytes_recv = recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &clilen);
    client_ip = inet_ntoa(cli_addr.sin_addr);
    client_port = ntohs(cli_addr.sin_port);

    printf("[server] Client IP: %s\n", client_ip);
    printf("[server] Client Port: %d\n", client_port);
    printf("[server] Client message: %s\n", buf);*/

    close_server(sockfd);
}

void close_server(int sockfd) {
    close(sockfd);
    exit(0);
}