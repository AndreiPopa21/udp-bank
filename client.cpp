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

using namespace std;

#define ARGC 3
#define BUFLEN 100
#define SENDTO "send_to"
#define SHOWSUM "show_sum"
#define GET "get"
#define EXIT "exit"

void close_client(int sockfd);

int main (int argc, char* argv[]) {

    int sockfd;
    fd_set read_fds;
    fd_set tmp_fds;
    struct sockaddr_in serv_addr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    char buf[BUFLEN];
    char payload[BUFLEN];

    if (argc != ARGC) {
        cout << "Wrong number of arguments (" << argc << ") should be "<< ARGC << endl;
        exit(0); 
    }

    //..........init UDP socket..........
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        printf("[client] Failed to create UDP socket\n");
        exit(0);
    }
    cout << "[client] Succesfully created UDP socket\n";


    //..........preparing connection......
    bzero(&serv_addr, sizeof(struct sockaddr_in));
    int port = atoi(argv[2]);
    char* ip = argv[1];
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_aton(ip, &serv_addr.sin_addr);


    //..........init multiplex............
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    FD_SET(0, &read_fds);
    FD_SET(sockfd, &read_fds);
    int fdmax = sockfd + 1;

    while (true) {
        tmp_fds = read_fds;
        if ((select(fdmax+1, &tmp_fds, NULL, NULL, NULL)) < 0) {
            cout << "[server] Select failed\n";
            close_client(sockfd);
        }

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {

                if (i == 0) {
                    bzero(buf, BUFLEN);
                    fgets(buf, BUFLEN, stdin);
                    strtok(buf,"\n");
                    printf("[client] S-a tastat: %s\n", buf);

                    char* command = strtok(buf," ");
                    if (strcmp(command, EXIT) == 0) {
                        close_client(sockfd);
                    }

                    if (strcmp(command, SENDTO) == 0) {
                        char* id = strtok(NULL, " ");
                        char* sum = strtok(NULL, " ");
                        if (id == NULL || sum == NULL) {
                            FD_CLR(0, &tmp_fds);
                            continue;
                        }
                        printf("[client] Sending to with id %s and sum %s\n", id, sum );
                        bzero(payload, BUFLEN);
                        strcpy(payload, "send_to ");
                        strcat(payload, id);
                        strcat(payload, " ");
                        strcat(payload, sum);
                        //printf("[client] Buffer: %s\n", payload);
                        int bytes_sent = sendto(sockfd, payload, strlen(payload), 0, (struct sockaddr*)&serv_addr, socklen);
                        cout << "[client] S-au trimis " << bytes_sent << endl;
                        bzero(payload, BUFLEN);
                        int bytes_recv = recvfrom(sockfd, payload, BUFLEN, 0,(struct sockaddr*)&serv_addr, &socklen);
                        printf("[client] From server: %s\n", payload);
                        FD_CLR(0, &tmp_fds);
                        continue;
                    }

                    

                    FD_CLR(0, &tmp_fds);
                }

                if (i == sockfd) {
                    int bytes_recv = recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&serv_addr, &socklen);
                    if (bytes_recv <= 0) {
                        cout << "[client] Server disconnected\n";
                        close_client(sockfd);
                    }
                    printf("[client] From server: %s\n", buf);
                    FD_CLR(sockfd, &tmp_fds);
                }
            }
        }
    }




    //..........dummy message.............
    /*bzero(buf, BUFLEN);
    sprintf(buf, "%s", "Buna, acesta e un mesaj frumos");
    int bytes_sent = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&serv_addr, socklen);
    cout << "[client] S-au trimis " << bytes_sent << " bytes\n";


    bzero(buf, BUFLEN);
    sprintf(buf, "%s", "Buna, acesta e un alt mesaj frumos");
    bytes_sent = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&serv_addr, socklen);
    cout << "[client] S-au trimis " << bytes_sent << " bytes\n";*/

    close_client(sockfd);
    
    return 0;
}

void close_client(int sockfd) {
    close(sockfd);
    exit(0);
}