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
#include<sys/epoll.h>
void addfd(int epollfd,int fd,bool is_et)
{
    epoll_event event;
    event.events=EPOLLIN;
    event.data.fd=fd;
    if(is_et) event.events |= EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    int oldop=fcntl(fd,F_GETFL);
    fcntl(fd,F_SETFL,oldop|O_NONBLOCK);
}
void et(epoll_event *events,int n,int epollfd,int listenfd)
{
    char buf[1024];
    for(int i=0;i<n;i++)
    {
        int sockfd=events[i].data.fd;
        if(sockfd==listenfd)
        {
            sockaddr_in client_address;
            socklen_t len=sizeof(client_address);
            int connfd=accept(sockfd,(sockaddr*)&client_address,&len);
            addfd(epollfd,connfd,true);
        }

        else if(events[i].events & EPOLLIN)
        {
            while(1)
            {
                printf("******");
                memset(buf,'\0',sizeof(buf));
                int res=recv(sockfd,buf,1023,0);
                if(res<0)
                {
                    if((errno==EAGAIN||errno==EWOULDBLOCK))
                    {
                        puts("read later\n");
                        break;
                    }
                    close(sockfd);
                    break;
                }
                else if(res==0)
                {
                    close(sockfd);
                }
                else
                {
                    printf("%s\n",buf);
                }
            }
        }
    }
}
void lt(epoll_event *events,int n,int epollfd,int listenfd)
{
    char buf[1024];
    for(int i=0;i<n;i++)
    {
        int sockfd=events[i].data.fd;
        if(sockfd==listenfd)
        {
            sockaddr_in client_address;
            socklen_t len=sizeof(client_address);
            int connfd=accept(sockfd,(sockaddr*)&client_address,&len);
            addfd(epollfd,connfd,false);
        }

        else if(events[i].events & EPOLLIN)
        {
            printf("******");
            memset(buf,'\0',sizeof(buf));
            int res=recv(sockfd,buf,1023,0);
            if(res<=0)
            {
                close(sockfd);
                continue;
            }
            printf("%s\n",buf);
        }
    }
}
int main(int argc,char* argv[])
{
    char *ip=argv[1];
    int port=atoi(argv[2]);
    sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    inet_pton(AF_INET,ip,&address.sin_addr);
    int listenfd=socket(PF_INET,SOCK_STREAM,0);

    int res=bind(listenfd,(sockaddr*)& address,sizeof(address));
    if(res==-1)
    {
        puts("error 1");
        return 0;
    }
    res=listen(listenfd,5);
    if(res==-2)
    {
        puts("error 2");
        return 0;
    }
    epoll_event events[10];
    int epollfd=epoll_create(5);
    addfd(epollfd,listenfd,true);
    while(1)
    {
        res=epoll_wait(epollfd,events,10,-1);
        if(res<0)
        {
            puts("error 3");
            return 0;
        }
        //lt(events,res,epollfd,listenfd);
        et(events,res,epollfd,listenfd);
    }
    close(listenfd);
    return 0;
}