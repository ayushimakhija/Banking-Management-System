#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<string.h>
#include <arpa/inet.h>
#include<signal.h>

#define PORT 55769

struct communication{
    char message[1024];
    int flag; // 0 Read, 1 ReadWrite and 2 Close
};


int main(int argc, char const *argv[])
{
    struct sockaddr_in server;
    int sock_desc;
    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    connect(sock_desc, (void *)(&server), sizeof(server));
    struct communication msg;
    while(1){
        int k = read(sock_desc, &msg, sizeof(msg));
        printf("%s",&msg.message);
        if(msg.flag == 1){
            char input[1024];
            scanf("%s",input);
            write(sock_desc,input,sizeof(input));
        }else if(msg.flag == 2){
            close(sock_desc);
            break;
        }
    }
    
    return 0;
}
