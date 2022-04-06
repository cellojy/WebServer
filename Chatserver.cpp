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
struct client_data
{
    sockaddr_in address;
    char* write_buf;
    char buf[64];
}users[65535];
int main(int argc,char* argv[])
{
    char* ip=argv[1];
    int port=atoi(argv[2]);
    sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    inet_pton(AF_INET,ip,&address.sin_addr);
    int listensock=socket(PF_INET,SOCK_STREAM,0);
    int res=bind(listensock,(sockaddr*)&address,sizeof(address));

    res=listen(listensock,5);
    if(res<0)
    {
        puts("error 1");
        return 0;
    }
    pollfd fds[6];
    int count=0;
    fds[0].fd=listensock;
    fds[0].events=POLLIN|POLLERR;
    fds[0].revents=0;
    for(int i=1;i<6;i++)
    {
        fds[i].fd=-1;
        fds[i].events=0;
        fds[i].revents=0;
    }
    puts("111");
    while(1)
    {
        
        res=poll(fds,count+1,-1);
   //  puts("111");
        if(res<0)
        {
            puts("error 2");
            return 0;
        }
        for(int i=0;i<=count;i++)
        {
            if(fds[i].fd==listensock && (fds[i].revents & POLLIN))
            {
                puts("connect!");
                sockaddr_in clientaddr;
                socklen_t len=sizeof(clientaddr);
                int clientsock=accept(listensock,(sockaddr*) &clientaddr,&len);
                if(clientsock<0)
                {
                    puts("error 3");
                    continue;
                }
                users[clientsock].address=clientaddr;
                fds[++count].fd=clientsock;
                fds[count].events=POLLIN|POLLERR|POLLRDHUP;
                fds[count].revents=0;
            }
            else if(fds[i].revents & POLLERR)
            {
                continue;
            }
            else if(fds[i].revents & POLLRDHUP)
            {
                users[fds[i].fd]=users[fds[count].fd];
                
                close(fds[i].fd);
                fds[i] = fds[count];
                i--;
                count--;
                puts("close!");
            }
            else if(fds[i].revents & POLLIN)
            {
                puts("recv!");
                int clientsock=fds[i].fd;
                memset(&users[clientsock].buf,'\0',sizeof(users[clientsock].buf));
                res = recv(clientsock,users[clientsock].buf,sizeof(users[clientsock].buf),0);
                printf("%s\n",users[clientsock].buf);
                for(int j=1;j<=count;j++)
                {
                    if(fds[j].fd==clientsock) continue;
                    fds[j].events |= ~POLLIN;
                    fds[j].events |= ~POLLOUT;
                    users[fds[j].fd].write_buf=users[clientsock].buf;
                }
            }
            else if(fds[i].revents & POLLOUT)
            {
                int clientsock=fds[i].fd;
                res = send(clientsock,users[clientsock].write_buf,sizeof(users[clientsock].write_buf),0);
                users[clientsock].write_buf=NULL;
                fds[i].events |= ~POLLOUT;
                fds[i].events |= ~POLLIN;
            }
            
        }
    }
    close(listensock);
    return 0;
    
}