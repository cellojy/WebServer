#define _GNU_SOURCE 1
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<poll.h>
#include<sys/epoll.h>
int main(int argc,char* argv[])
{
    char* ip=argv[1];
    int port=atoi(argv[2]);
    sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    inet_pton(AF_INET,ip,&address.sin_addr);
    int sock=socket(PF_INET,SOCK_STREAM,0);
    if(connect(sock,(sockaddr*)&address,sizeof(address))<0)
    {
        puts("error 1");
        close(sock);
        return 0;
    }
    pollfd fd[2];
    fd[0].fd=0;
    fd[0].events=POLLIN;
    fd[0].revents=0;
    fd[1].fd=sock;
    fd[1].events=POLLIN|POLLRDHUP;
    fd[1].revents=0;
    char buf[64];
    int pipefd[2];
    int res=pipe(pipefd);
    while(1)
    {
        res=poll(fd,2,-1);
        if(res<0)
        {
            puts("error 2");
            break;
        }
        if(fd[0].revents & POLLIN)
        {

            res=splice(0,NULL,pipefd[1],NULL,32768,SPLICE_F_MORE);
            res=splice(pipefd[0],NULL,sock,NULL,32768,SPLICE_F_MORE);
        }
        if(fd[1].revents & POLLRDHUP)
        {
            printf("close\n");
            break;
        }
        else if(fd[1].revents & POLLIN)
        {
            memset(buf,'\0',sizeof(buf));
            recv(sock,buf,sizeof(sock),0);
            printf("%s\n",buf);
        }
        
    }

    close(sock);
    
}
//poll revents??