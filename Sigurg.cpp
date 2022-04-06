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
#include<assert.h>
#include<sys/epoll.h>
#include<signal.h>
#define BUF_SIZE 1024
static int connfd;
void sig_urg(int sig)
{
    int save_errno = errno;
    char buffer[BUF_SIZE];
    memset(buffer,'\0',BUF_SIZE);
    int res=recv(connfd,buffer,BUF_SIZE-1,MSG_OOB);
    printf("got %d bytes of oob data '%s'\n",res,buffer);
    errno=save_errno;
}
void addsig(int sig,void (*sig_handler)(int))
{
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig,&sa,NULL) != -1);
}
int main(int argc,char* argv[])
{
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&address.sin_addr);
    address.sin_port = htons(port);
    int sock = socket(PF_INET,SOCK_STREAM,0);

    int res = bind(sock,(sockaddr*)&address,sizeof(address));
    assert(res != -1);
    res=listen(sock,5);
    sockaddr_in client;
    socklen_t len=sizeof(client);
    connfd=accept(sock,(sockaddr* )&client,&len);
    if(connfd<0)
    {
        printf("errno is: %d\n",errno);
    }
    else
    {
        addsig(SIGURG,sig_urg);
        fcntl(connfd,F_SETOWN,getpid());

        char buffer[BUF_SIZE];
        while(1)
        {
            memset(buffer,0,sizeof(buffer));
            res=recv(connfd,buffer,BUF_SIZE-1,0);
            if(res<=0)
            {
                break;
            }
            printf("got %d bytes of normal data '%s'\n",res,buffer);

        }
        close(connfd);
    }
    close(sock);

}